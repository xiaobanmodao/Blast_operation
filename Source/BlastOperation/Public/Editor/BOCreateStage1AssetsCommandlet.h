#pragma once

#include "Commandlets/Commandlet.h"
#include "BOCreateStage1AssetsCommandlet.generated.h"

UCLASS()
class BLASTOPERATION_API UBOCreateStage1AssetsCommandlet : public UCommandlet
{
	GENERATED_BODY()

public:
	UBOCreateStage1AssetsCommandlet();

	virtual int32 Main(const FString& Params) override;
};
