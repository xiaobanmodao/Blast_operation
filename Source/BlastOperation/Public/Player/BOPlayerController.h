#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BOPlayerController.generated.h"

class UInputMappingContext;

UCLASS(Blueprintable)
class BLASTOPERATION_API ABOPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABOPlayerController();

	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Input")
	TObjectPtr<UInputMappingContext> CombatMappingContext;
};
