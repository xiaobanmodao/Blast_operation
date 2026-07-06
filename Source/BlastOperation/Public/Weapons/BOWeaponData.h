#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "BOWeaponData.generated.h"

UCLASS(BlueprintType)
class BLASTOPERATION_API UBOWeaponData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	FGameplayTag WeaponTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "1"))
	int32 MagazineSize = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	bool bIsAutomatic = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "1.0"))
	float FireRateRPM = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float Damage = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "100.0"))
	float Range = 10000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float HeadshotMultiplier = 4.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float ReloadDuration = 1.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float StationarySpreadDegrees = 0.08f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float MovingSpreadDegrees = 1.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float AirborneSpreadDegrees = 2.8f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float ShotSpreadIncreaseDegrees = 0.12f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float MaxShotSpreadDegrees = 1.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float SpreadRecoveryDegreesPerSecond = 2.2f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float MovementAccuracySpeedThreshold = 90.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float RecoilPitchDegrees = 0.45f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float RecoilYawDegrees = 0.18f;
};
