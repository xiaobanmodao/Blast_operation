#include "Weapons/BOWeaponComponent.h"

#include "Core/BOLogChannels.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/BOWeaponData.h"

UBOWeaponComponent::UBOWeaponComponent()
{
	SetIsReplicatedByDefault(true);

	FallbackMagazineSize = 30;
	FallbackFireRateRPM = 600.0f;
	FallbackDamage = 30.0f;
	FallbackRange = 10000.0f;
	AmmoInMagazine = FallbackMagazineSize;
	LastFireTime = -1000.0f;
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
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	AmmoInMagazine = GetMagazineSize();
}

int32 UBOWeaponComponent::GetMagazineSize() const
{
	return WeaponData ? WeaponData->MagazineSize : FallbackMagazineSize;
}

float UBOWeaponComponent::GetDamage() const
{
	return WeaponData ? WeaponData->Damage : FallbackDamage;
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

bool UBOWeaponComponent::CanFire() const
{
	const UWorld* World = GetWorld();
	if (!World || AmmoInMagazine <= 0)
	{
		return false;
	}

	return World->GetTimeSeconds() - LastFireTime >= GetSecondsBetweenShots();
}

void UBOWeaponComponent::HandleFire(const FVector& TraceStart, const FVector& AimDirection)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority() || !CanFire())
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
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

	const FVector TraceEnd = TraceStart + AimDirection.GetSafeNormal() * GetRange();
	FHitResult Hit;
	const bool bHit = World->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	if (bHit && Hit.GetActor())
	{
		APawn* OwnerPawn = Cast<APawn>(Owner);
		AController* InstigatorController = OwnerPawn ? OwnerPawn->GetController() : nullptr;
		UGameplayStatics::ApplyPointDamage(Hit.GetActor(), GetDamage(), AimDirection, Hit, InstigatorController, Owner, nullptr);

		UE_LOG(LogBOWeapons, Verbose, TEXT("%s hit %s for %.1f damage. Ammo: %d/%d"),
			*GetNameSafe(Owner),
			*GetNameSafe(Hit.GetActor()),
			GetDamage(),
			AmmoInMagazine,
			GetMagazineSize());
	}
}

