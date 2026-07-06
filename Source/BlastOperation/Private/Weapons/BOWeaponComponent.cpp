#include "Weapons/BOWeaponComponent.h"

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

	static ConstructorHelpers::FObjectFinder<UBOWeaponData> RifleDataFinder(TEXT("/Game/BlastOperation/Weapons/Data/DA_BO_Rifle.DA_BO_Rifle"));
	if (RifleDataFinder.Succeeded())
	{
		WeaponData = RifleDataFinder.Object;
	}
}

void UBOWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		Reload();
	}
}

void UBOWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBOWeaponComponent, AmmoInMagazine);
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

int32 UBOWeaponComponent::GetMagazineSize() const
{
	return WeaponData ? WeaponData->MagazineSize : FallbackMagazineSize;
}

float UBOWeaponComponent::GetDamage() const
{
	return WeaponData ? WeaponData->Damage : FallbackDamage;
}

FText UBOWeaponComponent::GetDisplayName() const
{
	if (WeaponData && !WeaponData->DisplayName.IsEmpty())
	{
		return WeaponData->DisplayName;
	}

	return FText::FromString(TEXT("BR-01 Rifle"));
}

bool UBOWeaponComponent::IsAutomatic() const
{
	return WeaponData ? WeaponData->bIsAutomatic : bFallbackAutomatic;
}

float UBOWeaponComponent::GetSecondsBetweenShots() const
{
	const float FireRate = WeaponData ? WeaponData->FireRateRPM : FallbackFireRateRPM;
	return 60.0f / FMath::Max(1.0f, FireRate);
}

float UBOWeaponComponent::GetRange() const
{
	return WeaponData ? WeaponData->Range : FallbackRange;
}

float UBOWeaponComponent::GetReloadDuration() const
{
	return WeaponData ? WeaponData->ReloadDuration : FallbackReloadDuration;
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
	return WeaponData ? WeaponData->RecoilPitchDegrees : FallbackRecoilPitchDegrees;
}

float UBOWeaponComponent::GetRecoilYawDegrees() const
{
	return WeaponData ? WeaponData->RecoilYawDegrees : FallbackRecoilYawDegrees;
}

float UBOWeaponComponent::GetCurrentSpreadDegrees() const
{
	const AActor* Owner = GetOwner();
	const bool bMoving = Owner && Owner->GetVelocity().SizeSquared2D() > FMath::Square(80.0f);

	if (WeaponData)
	{
		return bMoving ? WeaponData->MovingSpreadDegrees : WeaponData->StationarySpreadDegrees;
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
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), Damage, ShotDirection, Hit, InstigatorController, Owner, nullptr);
		ClientConfirmHit(Hit.GetActor(), Damage);

		UE_LOG(LogBOWeapons, Verbose, TEXT("%s hit %s for %.1f damage. Ammo: %d/%d"),
			*GetNameSafe(Owner),
			*GetNameSafe(Hit.GetActor()),
			Damage,
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

void UBOWeaponComponent::CompleteReload()
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}

	AmmoInMagazine = GetMagazineSize();
	bIsReloading = false;
	ReloadEndTime = 0.0f;
}

void UBOWeaponComponent::ClientConfirmHit_Implementation(AActor* HitActor, float Damage)
{
	if (const UWorld* World = GetWorld())
	{
		LastHitConfirmTime = World->GetTimeSeconds();
		LastConfirmedDamage = Damage;
	}

	OnHitConfirmed.Broadcast(HitActor, Damage);
}
