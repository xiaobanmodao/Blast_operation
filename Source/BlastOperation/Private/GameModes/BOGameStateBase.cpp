#include "GameModes/BOGameStateBase.h"

#include "Core/BOLogChannels.h"
#include "Net/UnrealNetwork.h"

ABOGameStateBase::ABOGameStateBase()
	: RoundPhase(EBORoundPhase::WaitingForPlayers)
{
}

void ABOGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABOGameStateBase, RoundPhase);
}

void ABOGameStateBase::SetRoundPhase(const EBORoundPhase NewPhase)
{
	if (!HasAuthority() || RoundPhase == NewPhase)
	{
		return;
	}

	RoundPhase = NewPhase;
	OnRep_RoundPhase();
}

void ABOGameStateBase::OnRep_RoundPhase()
{
	UE_LOG(LogBORound, Verbose, TEXT("Round phase changed to %d."), static_cast<int32>(RoundPhase));
}

