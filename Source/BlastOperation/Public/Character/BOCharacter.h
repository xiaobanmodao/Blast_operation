#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "BOCharacter.generated.h"

class UBOHealthComponent;
class UBOWeaponComponent;
class UCameraComponent;
class UInputAction;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class BLASTOPERATION_API ABOCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABOCharacter();

	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Camera")
	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Health")
	UBOHealthComponent* GetHealthComponent() const { return HealthComponent; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Weapon")
	UBOWeaponComponent* GetWeaponComponent() const { return WeaponComponent; }

protected:
	void Move(const struct FInputActionValue& Value);
	void Look(const struct FInputActionValue& Value);
	void StartFire();
	void StopFire();
	void FireOnce();
	void ReloadWeapon();
	void EquipPrimaryWeapon();
	void EquipSecondaryWeapon();
	void ApplyLocalFireFeedback();
	void UpdateWeaponViewModel();
	void ConfigureWeaponViewMesh(UStaticMeshComponent* MeshComponent, const FVector& RelativeLocation, const FRotator& RelativeRotation, const FVector& RelativeScale) const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Health", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBOHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Weapon", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UBOWeaponComponent> WeaponComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Weapon View", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> WeaponBodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Weapon View", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> WeaponBarrelMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Weapon View", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> WeaponGripMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Weapon View", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> WeaponSightMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Input")
	TObjectPtr<UInputAction> FireAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Input")
	TObjectPtr<UInputAction> ReloadAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Input")
	TObjectPtr<UInputAction> EquipPrimaryAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Input")
	TObjectPtr<UInputAction> EquipSecondaryAction;

	FTimerHandle FireTimerHandle;
	bool bWantsToFire;
};
