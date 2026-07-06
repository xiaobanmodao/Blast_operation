#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TimerManager.h"
#include "BOWeaponComponent.generated.h"

class UBOWeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FBOHitConfirmedSignature, AActor*, HitActor, float, Damage, float, RemainingHealth, bool, bFatalHit);

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

	UFUNCTION(BlueprintCallable, Category = "Blast Operation|Weapon")
	void Reload();

	UFUNCTION(BlueprintCallable, Category = "Blast Operation|Weapon")
	void EquipWeaponSlot(int32 SlotIndex);

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	int32 GetAmmoInMagazine() const { return AmmoInMagazine; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	int32 GetMagazineSize() const;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetDamage() const;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	int32 GetCurrentWeaponSlot() const { return CurrentWeaponSlot; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	int32 GetWeaponSlotCount() const { return WeaponSlots.Num(); }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	FText GetDisplayName() const;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	bool IsAutomatic() const;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	bool IsReloading() const { return bIsReloading; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetReloadRemaining() const;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetSecondsBetweenShots() const;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetRecoilPitchDegrees() const;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetRecoilYawDegrees() const;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetCurrentSpreadDegrees() const;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetNormalizedAccuracyPenalty() const;

	UFUNCTION(BlueprintCallable, Category = "Blast Operation|Weapon")
	void RecordLocalShotFeedback();

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetLastHitConfirmTime() const { return LastHitConfirmTime; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetLastConfirmedDamage() const { return LastConfirmedDamage; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	float GetLastConfirmedRemainingHealth() const { return LastConfirmedRemainingHealth; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	bool WasLastHitFatal() const { return bLastHitWasFatal; }

	UPROPERTY(BlueprintAssignable, Category = "Blast Operation|Weapon")
	FBOHitConfirmedSignature OnHitConfirmed;

protected:
	UFUNCTION(Server, Reliable)
	void ServerFire(FVector_NetQuantize TraceStart, FVector_NetQuantizeNormal AimDirection);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	UFUNCTION(Server, Reliable)
	void ServerEquipWeaponSlot(int32 SlotIndex);

	UFUNCTION(Client, Unreliable)
	void ClientConfirmHit(AActor* HitActor, float Damage, float RemainingHealth, bool bFatalHit);

	void HandleFire(const FVector& TraceStart, const FVector& AimDirection);
	void HandleEquipWeaponSlot(int32 SlotIndex);
	void BeginReload();
	void CompleteReload();
	void CancelReload();
	void InitializeAmmoSlots();
	void StoreCurrentAmmo();
	const UBOWeaponData* GetActiveWeaponData() const;
	bool CanFire() const;
	float GetRange() const;
	float GetReloadDuration() const;
	float GetBaseSpreadDegrees() const;
	float GetStationarySpreadDegrees() const;
	float GetMovingSpreadDegrees() const;
	float GetAirborneSpreadDegrees() const;
	float GetShotSpreadIncreaseDegrees() const;
	float GetMaxShotSpreadDegrees() const;
	float GetSpreadRecoveryDegreesPerSecond() const;
	float GetMovementAccuracySpeedThreshold() const;
	void UpdateShotSpreadRecovery() const;
	void AddShotSpreadPenalty();
	void ResetShotSpread();
	FVector ApplyServerSpread(const FVector& AimDirection) const;
	void GetSanitizedTrace(const FVector& TraceStart, const FVector& AimDirection, FVector& OutTraceStart, FVector& OutAimDirection) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	TObjectPtr<UBOWeaponData> WeaponData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	TArray<TObjectPtr<UBOWeaponData>> WeaponSlots;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "1"))
	int32 FallbackMagazineSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "1.0"))
	float FallbackFireRateRPM;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	bool bFallbackAutomatic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackDamage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "100.0"))
	float FallbackRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackReloadDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackStationarySpreadDegrees;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackMovingSpreadDegrees;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackAirborneSpreadDegrees;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackShotSpreadIncreaseDegrees;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackMaxShotSpreadDegrees;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackSpreadRecoveryDegreesPerSecond;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackMovementAccuracySpeedThreshold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackRecoilPitchDegrees;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (ClampMin = "0.0"))
	float FallbackRecoilYawDegrees;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	int32 AmmoInMagazine;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	int32 CurrentWeaponSlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	TArray<int32> AmmoBySlot;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	bool bIsReloading;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Blast Operation|Weapon")
	float ReloadEndTime;

	float LastFireTime;
	float LastHitConfirmTime;
	float LastConfirmedDamage;
	float LastConfirmedRemainingHealth;
	bool bLastHitWasFatal;
	mutable float CurrentShotSpreadDegrees;
	mutable float LastSpreadUpdateTime;
	FTimerHandle ReloadTimerHandle;
};
