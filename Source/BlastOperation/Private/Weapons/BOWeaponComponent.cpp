#include "Weapons/BOWeaponComponent.h"

#include "Components/BOHealthComponent.h"
#include "Core/BOLogChannels.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapons/BOWeaponData.h"

UBOWeaponComponent::UBOWeaponComponent()
{
	SetIsReplicatedByDefault(true);

	FallbackMagazineSize = 30;
	FallbackFireRateRPM = 600.0f;
	bFallbackAutomatic = true;
	FallbackDamage = 30.0f;
	FallbackRange = 10000.0f;
	FallbackReloadDuration = 1.8f;
	FallbackStationarySpreadDegrees = 0.08f;
	FallbackMovingSpreadDegrees = 1.4f;
	FallbackRecoilPitchDegrees = 0.45f;
	FallbackRecoilYawDegrees = 0.18f;
	AmmoInMagazine = FallbackMagazineSize;
	bIsReloading = false;
	ReloadEndTime = 0.0f;
	LastFireTime = -1000.0f;
	LastHitConfirmTime = -1000.0f;
	LastConfirmedDamage = 0.0f;
	LastConfirmedRemainingHealth = -1.0f;
	bLastHitWasFatal = false;
	CurrentWeaponSlot = 0;

	static ConstructorHelpers::FObjectFinder<UBOWeaponData> RifleDataFinder(TEXT("/Game/BlastOperation/Weapons/Data/DA_BO_Rifle.DA_BO_Rifle"));
	if (RifleDataFinder.Succeeded())
	{
		WeaponData = RifleDataFinder.Object;
		WeaponSlots.Add(RifleDataFinder.Object);
	}

	static ConstructorHelpers::FObjectFinder<UBOWeaponData> PistolDataFinder(TEXT("/Game/BlastOperation/Weapons/Data/DA_BO_Pistol.DA_BO_Pistol"));
	if (PistolDataFinder.Succeeded())
	{
		WeaponSlots.Add(PistolDataFinder.Object);
	}
}

void UBOWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		InitializeAmmoSlots();
	}
}

void UBOWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBOWeaponComponent, AmmoInMagazine);
	DOREPLIFETIME(UBOWeaponComponent, CurrentWeaponSlot);
	DOREPLIFETIME(UBOWeaponComponent, AmmoBySlot);
	DOREPLIFETIME(UBOWeaponComponent, bIsReloading);
	DOREPLIFETIME(UBOWeaponComponent, ReloadEndTime);
}

void UBOWeaponComponent::Fire(const FVector& TraceStart, const FVector& AimDirection)
{
	if (!GetOwner())
	{
		return;
	}

	const FVector SafeDirection = AimDirection.GetSafeNormal();
	if (SafeDirection.IsNearlyZero())
	{
		return;
	}

	if (GetOwner()->HasAuthority())
	{
		HandleFire(TraceStart, SafeDirection);
	}
	else
	{
		ServerFire(TraceStart, SafeDirection);
	}
}

void UBOWeaponComponent::ServerFire_Implementation(FVector_NetQuantize TraceStart, FVector_NetQuantizeNormal AimDirection)
{
	HandleFire(TraceStart, FVector(AimDirection).GetSafeNormal());
}

void UBOWeaponComponent::Reload()
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (Owner->HasAuthority())
	{
		BeginReload();
	}
	else
	{
		ServerReload();
	}
}

void UBOWeaponComponent::ServerReload_Implementation()
{
	BeginReload();
}

void UBOWeaponComponent::EquipWeaponSlot(int32 SlotIndex)
{
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	if (Owner->HasAuthority())
	{
		HandleEquipWeaponSlot(SlotIndex);
	}
	else
	{
		ServerEquipWeaponSlot(SlotIndex);
	}
}

void UBOWeaponComponent::ServerEquipWeaponSlot_Implementation(int32 SlotIndex)
{
	HandleEquipWeaponSlot(SlotIndex);
}

int32 UBOWeaponComponent::GetMagazineSize() const
{
	const UBOWeaponData* ActiveWeaponData = GetActiveWeaponData();
	return ActiveWeaponData ? ActiveWeaponData->MagazineSize : FallbackMagazineSize;
}

float UBOWeaponComponent::GetDamage() const
{
	const UBOWeaponData* ActiveWeaponData = GetActiveWeaponData();
	return ActiveWeaponData ? ActiveWeaponData->Damage : FallbackDamage;
}

FText UBOWeaponComponent::GetDisplayName() const
{
	const UBOWeaponData* ActiveWeaponData = GetActiveWeaponData();
	if (ActiveWeaponData && !ActiveWeaponData->DisplayName.IsEmpty())
	{
		return ActiveWeaponData->DisplayName;
	}

	return FText::FromString(TEXT("BR-01 Rifle"));
}

bool UBOWeaponComponent::IsAutomatic() const
{
	const UBOWeaponData* ActiveWeaponData = GetActiveWeaponData();
	return ActiveWeaponData ? ActiveWeaponData->bIsAutomatic : bFallbackAutomatic;
}

float UBOWeaponComponent::GetSecondsBetweenShots() const
{
	const UBOWeaponData* ActiveWeaponData = GetActiveWeaponData();
	const float FireRate = ActiveWeaponData ? ActiveWeaponData->FireRateRPM : FallbackFireRateRPM;
	return 60.0f / FMath::Max(1.0f, FireRate);
}

float UBOWeaponComponent::GetRange() const
{
	const UBOWeaponData* ActiveWeaponData = GetActiveWeaponData();
	return ActiveWeaponData ? ActiveWeaponData->Range : FallbackRange;
}

float UBOWeaponComponent::GetReloadDuration() const
{
	const UBOWeaponData* ActiveWeaponData = GetActiveWeaponData();
	return ActiveWeaponData ? ActiveWeaponData->ReloadDuration : FallbackReloadDuration;
}

float UBOWeaponComponent::GetReloadRemaining() const
{
	const UWorld* World = GetWorld();
	if (!World || !bIsReloading)
	{
		return 0.0f;
	}

	return FMath::Max(0.0f, ReloadEndTime - World->GetTimeSeconds());
}

float UBOWeaponComponent::GetRecoilPitchDegrees() const
{
	const UBOWeaponData* ActiveWeaponData = GetActiveWeaponData();
	return ActiveWeaponData ? ActiveWeaponData->RecoilPitchDegrees : FallbackRecoilPitchDegrees;
}

float UBOWeaponComponent::GetRecoilYawDegrees() const
{
	const UBOWeaponData* ActiveWeaponData = GetActiveWeaponData();
	return ActiveWeaponData ? ActiveWeaponData->RecoilYawDegrees : FallbackRecoilYawDegrees;
}

float UBOWeaponComponent::GetCurrentSpreadDegrees() const
{
	const AActor* Owner = GetOwner();
	const bool bMoving = Owner && Owner->GetVelocity().SizeSquared2D() > FMath::Square(80.0f);

	const UBOWeaponData* ActiveWeaponData = GetActiveWeaponData();
	if (ActiveWeaponData)
	{
		return bMoving ? ActiveWeaponData->MovingSpreadDegrees : ActiveWeaponData->StationarySpreadDegrees;
	}

	return bMoving ? FallbackMovingSpreadDegrees : FallbackStationarySpreadDegrees;
}

bool UBOWeaponComponent::CanFire() const
{
	const UWorld* World = GetWorld();
	if (!World || AmmoInMagazine <= 0 || bIsReloading)
	{
		return false;
	}

	return World->GetTimeSeconds() - LastFireTime >= GetSecondsBetweenShots();
}

FVector UBOWeaponComponent::ApplyServerSpread(const FVector& AimDirection) const
{
	const FVector SafeDirection = AimDirection.GetSafeNormal();
	const float SpreadRadians = FMath::DegreesToRadians(GetCurrentSpreadDegrees());
	if (SpreadRadians <= KINDA_SMALL_NUMBER)
	{
		return SafeDirection;
	}

	return FMath::VRandCone(SafeDirection, SpreadRadians).GetSafeNormal();
}

void UBOWeaponComponent::GetSanitizedTrace(const FVector& TraceStart, const FVector& AimDirection, FVector& OutTraceStart, FVector& OutAimDirection) const
{
	OutTraceStart = TraceStart;
	OutAimDirection = AimDirection.GetSafeNormal();

	const APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn)
	{
		return;
	}

	FVector EyeLocation;
	FRotator EyeRotation;
	OwnerPawn->GetActorEyesViewPoint(EyeLocation, EyeRotation);

	if (FVector::DistSquared(OutTraceStart, EyeLocation) > FMath::Square(220.0f))
	{
		OutTraceStart = EyeLocation;
	}

	const FVector EyeDirection = EyeRotation.Vector().GetSafeNormal();
	if (FVector::DotProduct(OutAimDirection, EyeDirection) < 0.35f)
	{
		OutAimDirection = EyeDirection;
	}
}

void UBOWeaponComponent::HandleFire(const FVector& TraceStart, const FVector& AimDirection)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	if (AmmoInMagazine <= 0)
	{
		BeginReload();
		return;
	}

	if (!CanFire())
	{
		return;
	}

	LastFireTime = World->GetTimeSeconds();
	AmmoInMagazine = FMath::Max(0, AmmoInMagazine - 1);
	StoreCurrentAmmo();

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(BOWeaponTrace), true, Owner);
	if (const APawn* OwnerPawn = Cast<APawn>(Owner))
	{
		if (AController* Controller = OwnerPawn->GetController())
		{
			QueryParams.AddIgnoredActor(Controller);
		}
	}

	FVector SanitizedTraceStart;
	FVector SanitizedAimDirection;
	GetSanitizedTrace(TraceStart, AimDirection, SanitizedTraceStart, SanitizedAimDirection);

	const FVector ShotDirection = ApplyServerSpread(SanitizedAimDirection);
	const FVector TraceEnd = SanitizedTraceStart + ShotDirection * GetRange();
	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, SanitizedTraceStart, TraceEnd, ECC_Visibility, QueryParams);

	if (bHit && Hit.GetActor())
	{
		APawn* OwnerPawn = Cast<APawn>(Owner);
		AController* InstigatorController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
		const float Damage = GetDamage();
		UBOHealthComponent* TargetHealth = Hit.GetActor()->FindComponentByClass<UBOHealthComponent>();
		const float HealthBeforeDamage = TargetHealth ? TargetHealth->GetHealth() : -1.0f;

		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, ShotDirection, Hit, InstigatorController, Owner, nullptr);

		const float RemainingHealth = TargetHealth ? TargetHealth->GetHealth() : -1.0f;
		const float ConfirmedDamage = TargetHealth
			? FMath::Max(0.0f, HealthBeforeDamage - RemainingHealth)
			: Damage;
		const bool bFatalHit = TargetHealth && HealthBeforeDamage > 0.0f && RemainingHealth <= 0.0f;

		ClientConfirmHit(Hit.GetActor(), ConfirmedDamage, RemainingHealth, bFatalHit);

		UE_LOG(LogBOWeapons, Verbose, TEXT("%s hit %s for %.1f damage%s. Ammo: %d/%d"),
			*GetNameSafe(Owner),
			*GetNameSafe(Hit.GetActor()),
			ConfirmedDamage,
			bFatalHit ? TEXT(" and eliminated them") : TEXT(""),
			AmmoInMagazine,
			GetMagazineSize());
	}

	if (AmmoInMagazine <= 0)
	{
		BeginReload();
	}
}

void UBOWeaponComponent::BeginReload()
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !Owner->HasAuthority() || !World || bIsReloading || AmmoInMagazine >= GetMagazineSize())
	{
		return;
	}

	bIsReloading = true;
	ReloadEndTime = World->GetTimeSeconds() + GetReloadDuration();
	World->GetTimerManager().SetTimer(ReloadTimerHandle, this, &UBOWeaponComponent::CompleteReload, GetReloadDuration(), false);
}

void UBOWeaponComponent::CancelReload()
{
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().ClearTimer(ReloadTimerHandle);
	}

	bIsReloading = false;
	ReloadEndTime = 0.0f;
}

void UBOWeaponComponent::CompleteReload()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}

	AmmoInMagazine = GetMagazineSize();
	StoreCurrentAmmo();
	bIsReloading = false;
	ReloadEndTime = 0.0f;
}

void UBOWeaponComponent::HandleEquipWeaponSlot(int32 SlotIndex)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority() || !WeaponSlots.IsValidIndex(SlotIndex) || SlotIndex == CurrentWeaponSlot)
	{
		return;
	}

	StoreCurrentAmmo();
	CancelReload();
	CurrentWeaponSlot = SlotIndex;

	if (!AmmoBySlot.IsValidIndex(CurrentWeaponSlot))
	{
		InitializeAmmoSlots();
	}

	AmmoInMagazine = AmmoBySlot.IsValidIndex(CurrentWeaponSlot) ? AmmoBySlot[CurrentWeaponSlot] : GetMagazineSize();
	LastFireTime = -1000.0f;
}

void UBOWeaponComponent::InitializeAmmoSlots()
{
	AmmoBySlot.SetNum(WeaponSlots.Num());
	for (int32 SlotIndex = 0; SlotIndex < WeaponSlots.Num(); ++SlotIndex)
	{
		const UBOWeaponData* SlotWeaponData = WeaponSlots[SlotIndex];
		AmmoBySlot[SlotIndex] = SlotWeaponData ? SlotWeaponData->MagazineSize : FallbackMagazineSize;
	}

	CurrentWeaponSlot = WeaponSlots.IsValidIndex(CurrentWeaponSlot) ? CurrentWeaponSlot : 0;
	AmmoInMagazine = AmmoBySlot.IsValidIndex(CurrentWeaponSlot) ? AmmoBySlot[CurrentWeaponSlot] : GetMagazineSize();
}

void UBOWeaponComponent::StoreCurrentAmmo()
{
	if (AmmoBySlot.IsValidIndex(CurrentWeaponSlot))
	{
		AmmoBySlot[CurrentWeaponSlot] = AmmoInMagazine;
	}
}

const UBOWeaponData* UBOWeaponComponent::GetActiveWeaponData() const
{
	if (WeaponSlots.IsValidIndex(CurrentWeaponSlot) && WeaponSlots[CurrentWeaponSlot])
	{
		return WeaponSlots[CurrentWeaponSlot];
	}

	return WeaponData;
}

void UBOWeaponComponent::ClientConfirmHit_Implementation(AActor* HitActor, float Damage, float RemainingHealth, bool bFatalHit)
{
	if (const UWorld* World = GetWorld())
	{
		LastHitConfirmTime = World->GetTimeSeconds();
		LastConfirmedDamage = Damage;
		LastConfirmedRemainingHealth = RemainingHealth;
		bLastHitWasFatal = bFatalHit;
	}

	OnHitConfirmed.Broadcast(HitActor, Damage, RemainingHealth, bFatalHit);
}
