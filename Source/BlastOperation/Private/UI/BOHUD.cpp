#include "UI/BOHUD.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
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

	TArray<UUserWidget*> ExistingWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(PlayerController, ExistingWidgets, CombatHUDWidgetClass, false);
	for (UUserWidget* ExistingWidget : ExistingWidgets)
	{
		UBOCombatHUDWidget* ExistingCombatHUD = Cast<UBOCombatHUDWidget>(ExistingWidget);
		if (ExistingCombatHUD && ExistingCombatHUD->GetOwningPlayer() == PlayerController)
		{
			CombatHUDWidget = ExistingCombatHUD;
			return;
		}
	}

	CombatHUDWidget = CreateWidget<UBOCombatHUDWidget>(PlayerController, CombatHUDWidgetClass);
	if (CombatHUDWidget)
	{
		CombatHUDWidget->AddToViewport(20);
	}
}
