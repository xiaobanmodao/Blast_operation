#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BOHUD.generated.h"

class UBOCombatHUDWidget;

UCLASS()
class BLASTOPERATION_API ABOHUD : public AHUD
{
	GENERATED_BODY()

public:
	ABOHUD();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	void EnsureCombatHUD();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TSubclassOf<UBOCombatHUDWidget> CombatHUDWidgetClass;

	UPROPERTY(Transient)
	TObjectPtr<UBOCombatHUDWidget> CombatHUDWidget;
};
