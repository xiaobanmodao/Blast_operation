#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BOCombatHUDWidget.generated.h"

class UBOCombatFeedbackData;
class UCanvasPanel;
class UImage;
class UTextBlock;
class UWidget;

UCLASS(Blueprintable)
class BLASTOPERATION_API UBOCombatHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	void BuildWidgetTree();
	void UpdateCombatReadout();
	void UpdateCrosshair(const UBOCombatFeedbackData* FeedbackData);
	void UpdateHitFeedback(const UBOCombatFeedbackData* FeedbackData);
	void SetBoxBrush(UImage* Image, const FLinearColor& Color, const FVector2D& Size) const;
	void SetCanvasSlot(UWidget* Widget, const FVector2D& Position, const FVector2D& Size, const FVector2D& Anchor, const FVector2D& Alignment) const;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> RootPanel;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> StatusText;

	UPROPERTY(Transient)
	TObjectPtr<UTextBlock> DamageText;

	UPROPERTY(Transient)
	TObjectPtr<UImage> CrosshairLeft;

	UPROPERTY(Transient)
	TObjectPtr<UImage> CrosshairRight;

	UPROPERTY(Transient)
	TObjectPtr<UImage> CrosshairTop;

	UPROPERTY(Transient)
	TObjectPtr<UImage> CrosshairBottom;

	UPROPERTY(Transient)
	TObjectPtr<UImage> HitTopLeft;

	UPROPERTY(Transient)
	TObjectPtr<UImage> HitTopRight;

	UPROPERTY(Transient)
	TObjectPtr<UImage> HitBottomLeft;

	UPROPERTY(Transient)
	TObjectPtr<UImage> HitBottomRight;
};
