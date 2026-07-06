#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BOHealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBOHealthChangedSignature, float, NewHealth, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBODeathSignature, AActor*, DeadActor);

UCLASS(ClassGroup = (BlastOperation), Blueprintable, meta = (BlueprintSpawnableComponent))
class BLASTOPERATION_API UBOHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBOHealthComponent();

	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Health")
	float GetHealth() const { return Health; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Health")
	float GetArmor() const { return Armor; }

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Health")
	bool IsAlive() const { return Health > 0.0f; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Blast Operation|Health")
	void ResetHealth();

	UPROPERTY(BlueprintAssignable, Category = "Blast Operation|Health")
	FBOHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Blast Operation|Health")
	FBODeathSignature OnDeath;

protected:
	UFUNCTION()
	void HandleOwnerDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void OnRep_Health(float OldHealth);

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Blast Operation|Health")
	float MaxHealth;

	UPROPERTY(EditDefaultsOnly, Replicated, BlueprintReadOnly, Category = "Blast Operation|Health")
	float Armor;

	UPROPERTY(ReplicatedUsing = OnRep_Health, BlueprintReadOnly, Category = "Blast Operation|Health")
	float Health;
};

