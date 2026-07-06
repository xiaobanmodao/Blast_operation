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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "1.0"))
	float FireRateRPM = 600.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float Damage = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "100.0"))
	float Range = 10000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float HeadshotMultiplier = 4.0f;
};

