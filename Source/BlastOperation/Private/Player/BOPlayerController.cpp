#include "Player/BOPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "UObject/ConstructorHelpers.h"

ABOPlayerController::ABOPlayerController()
{
	bShowMouseCursor = false;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> CombatContextFinder(TEXT("/Game/BlastOperation/Input/IMC_BO_Combat.IMC_BO_Combat"));
	if (CombatContextFinder.Succeeded())
	{
		CombatMappingContext = CombatContextFinder.Object;
	}
}

void ABOPlayerController::BeginPlay()
{
	Super::BeginPlay();

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
}
