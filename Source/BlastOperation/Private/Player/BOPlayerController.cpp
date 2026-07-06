#include "Player/BOPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "UObject/ConstructorHelpers.h"

ABOPlayerController::ABOPlayerController()
{
	bShowMouseCursor = false;

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> DefaultContextFinder(TEXT("/Game/Input/IMC_Default.IMC_Default"));
	if (DefaultContextFinder.Succeeded())
	{
		DefaultMappingContext = DefaultContextFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> WeaponContextFinder(TEXT("/Game/Variant_Shooter/Input/IMC_Weapons.IMC_Weapons"));
	if (WeaponContextFinder.Succeeded())
	{
		WeaponMappingContext = WeaponContextFinder.Object;
	}
}

void ABOPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}

			if (WeaponMappingContext)
			{
				Subsystem->AddMappingContext(WeaponMappingContext, 1);
			}
		}
	}
}
