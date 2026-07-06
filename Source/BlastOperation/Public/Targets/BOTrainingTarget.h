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

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDeath(AActor* DeadActor);

	void ResetTarget();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Target")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Target")
	TObjectPtr<UBOHealthComponent> HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Target")
	float ResetDelay;
};

