#include "Player/BOPlayerState.h"

#include "Net/UnrealNetwork.h"

ABOPlayerState::ABOPlayerState()
	: Team(EBOTeam::None)
{
}

void ABOPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABOPlayerState, Team);
}

void ABOPlayerState::SetTeam(const EBOTeam NewTeam)
{
	if (HasAuthority())
	{
		Team = NewTeam;
	}
}

