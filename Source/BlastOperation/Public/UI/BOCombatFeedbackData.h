#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BOCombatFeedbackData.generated.h"

UCLASS(BlueprintType)
class BLASTOPERATION_API UBOCombatFeedbackData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD")
	FLinearColor CrosshairColor = FLinearColor::White;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD")
	FLinearColor HitMarkerColor = FLinearColor(1.0f, 0.86f, 0.18f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD")
	FLinearColor FatalHitMarkerColor = FLinearColor(1.0f, 0.2f, 0.12f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD")
	FLinearColor DamageNumberColor = FLinearColor(1.0f, 0.95f, 0.72f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD")
	FLinearColor FatalDamageNumberColor = FLinearColor(1.0f, 0.28f, 0.16f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD")
	FLinearColor StatusTextColor = FLinearColor(0.92f, 0.96f, 1.0f, 1.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD")
	FLinearColor ShadowColor = FLinearColor::Black;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD", meta = (ClampMin = "0.01"))
	float HitMarkerDuration = 0.18f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD", meta = (ClampMin = "0.01"))
	float DamageNumberDuration = 0.58f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD", meta = (ClampMin = "1.0"))
	float CrosshairHalfLength = 8.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD", meta = (ClampMin = "0.0"))
	float CrosshairBaseGap = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD", meta = (ClampMin = "0.0"))
	float CrosshairSpreadScale = 10.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD", meta = (ClampMin = "0.0"))
	float CrosshairMaxDynamicGap = 30.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD", meta = (ClampMin = "0.1"))
	float CrosshairLineThickness = 1.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD", meta = (ClampMin = "1.0"))
	float HitMarkerLength = 15.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD", meta = (ClampMin = "0.0"))
	float HitMarkerGap = 5.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Blast Operation|HUD", meta = (ClampMin = "0.0"))
	float DamageNumberRise = 28.0f;
};
