#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BOWeaponComponent.generated.h"

class UBOWeaponData;

UCLASS(ClassGroup = (BlastOperation), Blueprintable, meta = (BlueprintSpawnableComponent))
class BLASTOPERATION_API UBOWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBOWeaponComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, Category = "Blast Operation|Weapon")
	void Fire(const FVector& TraceStart, const FVector& AimDirection);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Blast Operation|Weapon")
	void Reload();

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	int32 GetAmmoInMagazine() const { return AmmoInMagazine; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	int32 GetMagazineSize() const;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetDamage() const;

protected:
	UFUNCTION(Server, Reliable)
	void ServerFire(FVector_NetQuantize TraceStart, FVector_NetQuantizeNormal AimDirection);

	void HandleFire(const FVector& TraceStart, const FVector& AimDirection);
	bool CanFire() const;
	float GetSecondsBetweenShots() const;
	float GetRange() const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	TObjectPtr<UBOWeaponData> WeaponData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "1"))
	int32 FallbackMagazineSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "1.0"))
	float FallbackFireRateRPM;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "100.0"))
	float FallbackRange;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	int32 AmmoInMagazine;

	float LastFireTime;
};

