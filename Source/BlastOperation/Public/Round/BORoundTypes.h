#pragma once

#include "CoreMinimal.h"
#include "BORoundTypes.generated.h"

UENUM(BlueprintType)
enum class EBORoundPhase : uint8
{
	WaitingForPlayers UMETA(DisplayName = "Waiting For Players"),
	Warmup UMETA(DisplayName = "Warmup"),
	BuyPhase UMETA(DisplayName = "Buy Phase"),
	RoundLive UMETA(DisplayName = "Round Live"),
	BombPlanted UMETA(DisplayName = "Bomb Planted"),
	RoundEnding UMETA(DisplayName = "Round Ending"),
	MatchEnding UMETA(DisplayName = "Match Ending"),
	PostMatch UMETA(DisplayName = "Post Match")
};

UENUM(BlueprintType)
enum class EBOTeam : uint8
{
	None UMETA(DisplayName = "None"),
	Attacker UMETA(DisplayName = "Attacker"),
	Defender UMETA(DisplayName = "Defender"),
	Spectator UMETA(DisplayName = "Spectator")
};

UENUM(BlueprintType)
enum class EBORoundEndReason : uint8
{
	None UMETA(DisplayName = "None"),
	AttackersEliminated UMETA(DisplayName = "Attackers Eliminated"),
	DefendersEliminated UMETA(DisplayName = "Defenders Eliminated"),
	BombExploded UMETA(DisplayName = "Bomb Exploded"),
	BombDefused UMETA(DisplayName = "Bomb Defused"),
	RoundTimerExpired UMETA(DisplayName = "Round Timer Expired")
};
