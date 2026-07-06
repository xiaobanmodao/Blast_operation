#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BOPlayerController.generated.h"

class UInputMappingContext;
class UBOCombatHUDWidget;

UCLASS(Blueprintable)
class BLASTOPERATION_API ABOPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABOPlayerController();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	void ConfigureGameInputMode();
	void EnsureCombatHUD();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Input")
	TObjectPtr<UInputMappingContext> CombatMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TSubclassOf<UBOCombatHUDWidget> CombatHUDWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UBOCombatHUDWidget> CombatHUDWidget;
};
