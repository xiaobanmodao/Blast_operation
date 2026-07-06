#include "Player/BOPlayerController.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "UI/BOCombatHUDWidget.h"
#include "UObject/ConstructorHelpers.h"

ABOPlayerController::ABOPlayerController()
{
	bShowMouseCursor = false;
	CombatHUDWidgetClass = UBOCombatHUDWidget::StaticClass();

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> CombatContextFinder(TEXT("/Game/BlastOperation/Input/IMC_BO_Combat.IMC_BO_Combat"));
	if (CombatContextFinder.Succeeded())
	{
		CombatMappingContext = CombatContextFinder.Object;
	}
}

void ABOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ConfigureGameInputMode();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (CombatMappingContext)
			{
				Subsystem->AddMappingContext(CombatMappingContext, 0);
			}
		}
	}

	EnsureCombatHUD();
}

void ABOPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (CombatHUDWidget)
	{
		CombatHUDWidget->RemoveFromParent();
		CombatHUDWidget = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

void ABOPlayerController::ConfigureGameInputMode()
{
	bShowMouseCursor = false;

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);
}

void ABOPlayerController::EnsureCombatHUD()
{
	if (CombatHUDWidget || !CombatHUDWidgetClass || !IsLocalController())
	{
		return;
	}

	TArray<UUserWidget*> ExistingWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, ExistingWidgets, CombatHUDWidgetClass, false);
	for (UUserWidget* ExistingWidget : ExistingWidgets)
	{
		UBOCombatHUDWidget* ExistingCombatHUD = Cast<UBOCombatHUDWidget>(ExistingWidget);
		if (ExistingCombatHUD && ExistingCombatHUD->GetOwningPlayer() == this)
		{
			CombatHUDWidget = ExistingCombatHUD;
			return;
		}
	}

	CombatHUDWidget = CreateWidget<UBOCombatHUDWidget>(this, CombatHUDWidgetClass);
	if (CombatHUDWidget)
	{
		CombatHUDWidget->AddToViewport(20);
	}
}
