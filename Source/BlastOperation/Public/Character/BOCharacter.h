#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BOCharacter.generated.h"

class UCameraComponent;

UCLASS(Blueprintable)
class BLASTOPERATION_API ABOCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABOCharacter();

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Camera")
	UCameraComponent* GetFirstPersonCamera() const { return FirstPersonCamera; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Blast Operation|Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;
};

