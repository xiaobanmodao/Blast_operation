#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Round/BORoundTypes.h"
#include "BOPlayerState.generated.h"

UCLASS(Blueprintable)
class BLASTOPERATION_API ABOPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ABOPlayerState();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Team")
	EBOTeam GetTeam() const { return Team; }

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Blast Operation|Team")
	void SetTeam(EBOTeam NewTeam);

protected:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Blast Operation|Team")
	EBOTeam Team;
};

