#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BOCombatHUDWidget.generated.h"

class UBOCombatFeedbackData;
class UCanvasPanel;
class UImage;
class SWidget;
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
	virtual TSharedRef<SWidget> RebuildWidget() override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Blast Operation|HUD")
	void OnCombatReadoutUpdated(const FText& Status, float Health, int32 Ammo, int32 MagazineSize, int32 WeaponSlot);

	UFUNCTION(BlueprintImplementableEvent, Category = "Blast Operation|HUD")
	void OnCrosshairUpdated(float CurrentSpreadDegrees, float CrosshairGap);

	UFUNCTION(BlueprintImplementableEvent, Category = "Blast Operation|HUD")
	void OnHitFeedbackUpdated(float Damage, bool bFatalHit, float NormalizedAge);

	void BuildWidgetTree();
	void UpdateCombatReadout();
	void UpdateCrosshair(const UBOCombatFeedbackData* FeedbackData);
	void UpdateHitFeedback(const UBOCombatFeedbackData* FeedbackData);
	void SetBoxBrush(UImage* Image, const FLinearColor& Color, const FVector2D& Size) const;
	void SetCanvasSlot(UWidget* Widget, const FVector2D& Position, const FVector2D& Size, const FVector2D& Anchor, const FVector2D& Alignment) const;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UCanvasPanel> RootPanel;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UTextBlock> StatusText;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UTextBlock> DamageText;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UImage> CrosshairLeft;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UImage> CrosshairRight;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UImage> CrosshairTop;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UImage> CrosshairBottom;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UImage> HitTopLeft;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UImage> HitTopRight;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UImage> HitBottomLeft;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Blast Operation|HUD")
	TObjectPtr<UImage> HitBottomRight;
};
