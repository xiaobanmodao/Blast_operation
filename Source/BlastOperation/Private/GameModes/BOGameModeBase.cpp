#include "GameModes/BOGameModeBase.h"

#include "GameModes/BOGameStateBase.h"
#include "Player/BOPlayerController.h"
#include "Player/BOPlayerState.h"

ABOGameModeBase::ABOGameModeBase()
{
	GameStateClass = ABOGameStateBase::StaticClass();
	PlayerControllerClass = ABOPlayerController::StaticClass();
	PlayerStateClass = ABOPlayerState::StaticClass();
}

