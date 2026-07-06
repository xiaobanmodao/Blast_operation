#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/EngineTypes.h"
#include "BOImpactFeedbackData.generated.h"

class USoundBase;

USTRUCT(BlueprintType)
struct FBOImpactFeedback
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact")
	FLinearColor MarkerColor = FLinearColor(0.95f, 0.96f, 0.86f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact", meta = (ClampMin = "1.0"))
	float MarkerRadius = 12.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact", meta = (ClampMin = "0.01"))
	float MarkerDuration = 0.18f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact", meta = (ClampMin = "0.1"))
	float MarkerThickness = 1.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact")
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact", meta = (ClampMin = "0.0"))
	float SoundVolume = 0.45f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact", meta = (ClampMin = "0.1"))
	float SoundPitch = 1.0f;
};

USTRUCT(BlueprintType)
struct FBOImpactSurfaceFeedback
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact")
	TEnumAsByte<EPhysicalSurface> SurfaceType = SurfaceType_Default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact")
	FBOImpactFeedback Feedback;
};

UCLASS(BlueprintType)
class BLASTOPERATION_API UBOImpactFeedbackData : public UDataAsset
{
	GENERATED_BODY()

public:
	const FBOImpactFeedback& FindFeedback(EPhysicalSurface SurfaceType) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact")
	FBOImpactFeedback DefaultFeedback;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|Impact")
	TArray<FBOImpactSurfaceFeedback> SurfaceFeedback;
};
