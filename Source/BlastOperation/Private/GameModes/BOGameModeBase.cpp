#include "GameModes/BOGameModeBase.h"

#include "Character/BOCharacter.h"
#include "GameModes/BOGameStateBase.h"
#include "Player/BOPlayerController.h"
#include "Player/BOPlayerState.h"
#include "UI/BOHUD.h"

ABOGameModeBase::ABOGameModeBase()
{
	DefaultPawnClass = ABOCharacter::StaticClass();
	GameStateClass = ABOGameStateBase::StaticClass();
	HUDClass = ABOHUD::StaticClass();
	PlayerControllerClass = ABOPlayerController::StaticClass();
	PlayerStateClass = ABOPlayerState::StaticClass();
}
