#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BOTrainingTarget.generated.h"

class UBOHealthComponent;
class UStaticMeshComponent;

UCLASS(Blueprintable)
class BLASTOPERATION_API ABOTrainingTarget : public AActor
{
	GENERATED_BODY()

public:
	ABOTrainingTarget();

	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDeath(AActor* DeadActor);

	UFUNCTION()
	void HandleHealthChanged(float NewHealth, float Delta);

	UFUNCTION()
	void OnRep_TargetActive();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastPlayDamageFeedback(float DamageAmount);

	void DeactivateTarget();
	void ResetTarget();
	void ApplyTargetActiveState();
	void PlayDamageFeedback(float DamageAmount);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Target")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Target")
	TObjectPtr<UBOHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Target")
	float ResetDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Target", meta = (ClampMin = "0.01"))
	float DamageFlashDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Target", meta = (ClampMin = "0.0"))
	float DamageFlashScaleBoost;

	UPROPERTY(ReplicatedUsing = OnRep_TargetActive)
	bool bTargetActive;

	FVector DefaultMeshScale;
	float DamageFlashEndTime;
	float DamageFlashStrength;
};
