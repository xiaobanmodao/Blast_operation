#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Round/BORoundTypes.h"
#include "BOGameStateBase.generated.h"

UCLASS(Blueprintable)
class BLASTOPERATION_API ABOGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	ABOGameStateBase();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Blast Operation|Round")
	void SetRoundPhase(EBORoundPhase NewPhase);

	UFUNCTION(BlueprintPure, Category = "Blast Operation|Round")
	EBORoundPhase GetRoundPhase() const { return RoundPhase; }

protected:
	UFUNCTION()
	void OnRep_RoundPhase();

	UPROPERTY(ReplicatedUsing = OnRep_RoundPhase, BlueprintReadOnly, Category = "Blast Operation|Round")
	EBORoundPhase RoundPhase;
};

