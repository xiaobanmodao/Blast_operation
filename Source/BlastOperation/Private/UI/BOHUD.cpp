#include "UI/BOHUD.h"

#include "Blueprint/UserWidget.h"
#include "GameFramework/PlayerController.h"
#include "UI/BOCombatHUDWidget.h"

ABOHUD::ABOHUD()
{
	CombatHUDWidgetClass = UBOCombatHUDWidget::StaticClass();
}

void ABOHUD::BeginPlay()
{
	Super::BeginPlay();

	EnsureCombatHUD();
}

void ABOHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CombatHUDWidget)
	{
		CombatHUDWidget->RemoveFromParent();
		CombatHUDWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void ABOHUD::EnsureCombatHUD()
{
	if (CombatHUDWidget || !CombatHUDWidgetClass)
	{
		return;
	}

	APlayerController* PlayerController = GetOwningPlayerController();
	if (!PlayerController || !PlayerController->IsLocalController())
	{
		return;
	}

	CombatHUDWidget = CreateWidget<UBOCombatHUDWidget>(PlayerController, CombatHUDWidgetClass);
	if (CombatHUDWidget)
	{
		CombatHUDWidget->AddToViewport(0);
	}
}
