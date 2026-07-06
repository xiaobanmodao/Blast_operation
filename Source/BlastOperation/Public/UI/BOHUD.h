#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BOHUD.generated.h"

UCLASS()
class BLASTOPERATION_API ABOHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
};

