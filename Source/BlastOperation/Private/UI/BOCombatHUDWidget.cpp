#include "UI/BOCombatHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Character/BOCharacter.h"
#include "Components/BOHealthComponent.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Engine/Font.h"
#include "Styling/CoreStyle.h"
#include "UI/BOCombatFeedbackData.h"
#include "Weapons/BOWeaponComponent.h"

namespace
{
const UBOCombatFeedbackData* LoadFeedbackData()
{
	static const UBOCombatFeedbackData* FeedbackData = LoadObject<UBOCombatFeedbackData>(nullptr, TEXT("/Game/BlastOperation/UI/Data/DA_BO_CombatFeedback.DA_BO_CombatFeedback"));
	return FeedbackData ? FeedbackData : GetDefault<UBOCombatFeedbackData>();
}

ABOCharacter* GetHUDCharacter(const UUserWidget* Widget)
{
	const APlayerController* OwningPlayer = Widget ? Widget->GetOwningPlayer() : nullptr;
	return OwningPlayer ? Cast<ABOCharacter>(OwningPlayer->GetPawn()) : nullptr;
}

void ConfigureText(UTextBlock* TextBlock, float FontSize, const FLinearColor& Color, ETextJustify::Type Justification = ETextJustify::Left)
{
	if (!TextBlock)
	{
		return;
	}

	FSlateFontInfo FontInfo = TextBlock->GetFont();
	FontInfo.Size = FontSize;
	TextBlock->SetFont(FontInfo);
	TextBlock->SetJustification(Justification);
	TextBlock->SetColorAndOpacity(FSlateColor(Color));
	TextBlock->SetShadowOffset(FVector2D(1.2f, 1.2f));
	TextBlock->SetShadowColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.82f));
}
}

void UBOCombatHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (!RootPanel)
	{
		BuildWidgetTree();
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);
	UpdateCombatReadout();
}

void UBOCombatHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateCombatReadout();
}

void UBOCombatHUDWidget::BuildWidgetTree()
{
	RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CombatHUDRoot"));
	WidgetTree->RootWidget = RootPanel;

	StatusText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("CombatStatusText"));
	RootPanel->AddChild(StatusText);
	SetCanvasSlot(StatusText, FVector2D(32.0f, -32.0f), FVector2D(760.0f, 38.0f), FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f));

	DamageText = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), TEXT("DamageConfirmText"));
	RootPanel->AddChild(DamageText);
	SetCanvasSlot(DamageText, FVector2D(95.0f, -78.0f), FVector2D(240.0f, 48.0f), FVector2D(0.5f, 0.5f), FVector2D(0.5f, 0.5f));

	auto AddImage = [this](const FName Name) -> UImage*
	{
		UImage* Image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), Name);
		RootPanel->AddChild(Image);
		Image->SetVisibility(ESlateVisibility::HitTestInvisible);
		return Image;
	};

	CrosshairLeft = AddImage(TEXT("CrosshairLeft"));
	CrosshairRight = AddImage(TEXT("CrosshairRight"));
	CrosshairTop = AddImage(TEXT("CrosshairTop"));
	CrosshairBottom = AddImage(TEXT("CrosshairBottom"));

	HitTopLeft = AddImage(TEXT("HitTopLeft"));
	HitTopRight = AddImage(TEXT("HitTopRight"));
	HitBottomLeft = AddImage(TEXT("HitBottomLeft"));
	HitBottomRight = AddImage(TEXT("HitBottomRight"));

	ConfigureText(StatusText, 18.0f, FLinearColor::White);
	ConfigureText(DamageText, 20.0f, FLinearColor::White, ETextJustify::Center);
	DamageText->SetVisibility(ESlateVisibility::Hidden);
}

void UBOCombatHUDWidget::UpdateCombatReadout()
{
	const UBOCombatFeedbackData* FeedbackData = LoadFeedbackData();
	UpdateCrosshair(FeedbackData);
	UpdateHitFeedback(FeedbackData);

	ABOCharacter* Character = GetHUDCharacter(this);
	if (!Character)
	{
		if (StatusText)
		{
			StatusText->SetText(FText::GetEmpty());
		}
		return;
	}

	const UBOHealthComponent* HealthComponent = Character->GetHealthComponent();
	const UBOWeaponComponent* WeaponComponent = Character->GetWeaponComponent();
	const float Health = HealthComponent ? HealthComponent->GetHealth() : 0.0f;
	const int32 Ammo = WeaponComponent ? WeaponComponent->GetAmmoInMagazine() : 0;
	const int32 Magazine = WeaponComponent ? WeaponComponent->GetMagazineSize() : 0;
	const FText WeaponName = WeaponComponent ? WeaponComponent->GetDisplayName() : FText::FromString(TEXT("No Weapon"));
	const int32 WeaponSlot = WeaponComponent ? WeaponComponent->GetCurrentWeaponSlot() + 1 : 0;

	const FString ReloadText = WeaponComponent && WeaponComponent->IsReloading()
		? FString::Printf(TEXT(" | RELOADING %.1fs"), WeaponComponent->GetReloadRemaining())
		: FString();
	const FString Status = FString::Printf(TEXT("[%d] %s | HP %.0f | Ammo %d / %d%s"), WeaponSlot, *WeaponName.ToString(), Health, Ammo, Magazine, *ReloadText);

	if (StatusText)
	{
		StatusText->SetText(FText::FromString(Status));
		StatusText->SetColorAndOpacity(FSlateColor(FeedbackData ? FeedbackData->StatusTextColor : FLinearColor::White));
		StatusText->SetShadowColorAndOpacity(FeedbackData ? FeedbackData->ShadowColor : FLinearColor::Black);
	}
}

void UBOCombatHUDWidget::UpdateCrosshair(const UBOCombatFeedbackData* FeedbackData)
{
	const ABOCharacter* Character = GetHUDCharacter(this);
	const UBOWeaponComponent* WeaponComponent = Character ? Character->GetWeaponComponent() : nullptr;

	const float LineLength = FeedbackData ? FeedbackData->CrosshairHalfLength : 8.0f;
	const float BaseGap = FeedbackData ? FeedbackData->CrosshairBaseGap : 5.0f;
	const float SpreadScale = FeedbackData ? FeedbackData->CrosshairSpreadScale : 10.0f;
	const float MaxDynamicGap = FeedbackData ? FeedbackData->CrosshairMaxDynamicGap : 30.0f;
	const float Thickness = FeedbackData ? FeedbackData->CrosshairLineThickness : 1.0f;
	const float SpreadDegrees = WeaponComponent ? WeaponComponent->GetCurrentSpreadDegrees() : 0.0f;
	const float Gap = BaseGap + FMath::Min(MaxDynamicGap, SpreadDegrees * SpreadScale);
	const FLinearColor Color = FeedbackData ? FeedbackData->CrosshairColor : FLinearColor::White;

	SetBoxBrush(CrosshairLeft, Color, FVector2D(LineLength, Thickness));
	SetCanvasSlot(CrosshairLeft, FVector2D(-Gap - LineLength, -Thickness * 0.5f), FVector2D(LineLength, Thickness), FVector2D(0.5f, 0.5f), FVector2D::ZeroVector);

	SetBoxBrush(CrosshairRight, Color, FVector2D(LineLength, Thickness));
	SetCanvasSlot(CrosshairRight, FVector2D(Gap, -Thickness * 0.5f), FVector2D(LineLength, Thickness), FVector2D(0.5f, 0.5f), FVector2D::ZeroVector);

	SetBoxBrush(CrosshairTop, Color, FVector2D(Thickness, LineLength));
	SetCanvasSlot(CrosshairTop, FVector2D(-Thickness * 0.5f, -Gap - LineLength), FVector2D(Thickness, LineLength), FVector2D(0.5f, 0.5f), FVector2D::ZeroVector);

	SetBoxBrush(CrosshairBottom, Color, FVector2D(Thickness, LineLength));
	SetCanvasSlot(CrosshairBottom, FVector2D(-Thickness * 0.5f, Gap), FVector2D(Thickness, LineLength), FVector2D(0.5f, 0.5f), FVector2D::ZeroVector);
}

void UBOCombatHUDWidget::UpdateHitFeedback(const UBOCombatFeedbackData* FeedbackData)
{
	ABOCharacter* Character = GetHUDCharacter(this);
	const UBOWeaponComponent* WeaponComponent = Character ? Character->GetWeaponComponent() : nullptr;
	const UWorld* World = GetWorld();
	if (!WeaponComponent || !World || !FeedbackData)
	{
		if (DamageText)
		{
			DamageText->SetVisibility(ESlateVisibility::Hidden);
		}
		return;
	}

	const float HitAge = World->GetTimeSeconds() - WeaponComponent->GetLastHitConfirmTime();
	const bool bFatalHit = WeaponComponent->WasLastHitFatal();
	const FLinearColor HitColor = bFatalHit ? FeedbackData->FatalHitMarkerColor : FeedbackData->HitMarkerColor;
	const bool bShowHitMarker = HitAge <= FeedbackData->HitMarkerDuration;

	UImage* HitLines[] = { HitTopLeft, HitTopRight, HitBottomLeft, HitBottomRight };
	for (UImage* HitLine : HitLines)
	{
		if (HitLine)
		{
			HitLine->SetVisibility(bShowHitMarker ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
		}
	}

	if (bShowHitMarker)
	{
		const float SegmentLength = FMath::Max(1.0f, (FeedbackData->HitMarkerLength - FeedbackData->HitMarkerGap) * UE_SQRT_2);
		const float Thickness = FMath::Max(1.0f, FeedbackData->CrosshairLineThickness);
		const float CenterOffset = (FeedbackData->HitMarkerLength + FeedbackData->HitMarkerGap) * 0.5f;
		const FVector2D SegmentSize(SegmentLength, Thickness);

		SetBoxBrush(HitTopLeft, HitColor, SegmentSize);
		SetCanvasSlot(HitTopLeft, FVector2D(-CenterOffset, -CenterOffset), SegmentSize, FVector2D(0.5f, 0.5f), FVector2D(0.5f, 0.5f));
		HitTopLeft->SetRenderTransformAngle(45.0f);

		SetBoxBrush(HitTopRight, HitColor, SegmentSize);
		SetCanvasSlot(HitTopRight, FVector2D(CenterOffset, -CenterOffset), SegmentSize, FVector2D(0.5f, 0.5f), FVector2D(0.5f, 0.5f));
		HitTopRight->SetRenderTransformAngle(-45.0f);

		SetBoxBrush(HitBottomLeft, HitColor, SegmentSize);
		SetCanvasSlot(HitBottomLeft, FVector2D(-CenterOffset, CenterOffset), SegmentSize, FVector2D(0.5f, 0.5f), FVector2D(0.5f, 0.5f));
		HitBottomLeft->SetRenderTransformAngle(-45.0f);

		SetBoxBrush(HitBottomRight, HitColor, SegmentSize);
		SetCanvasSlot(HitBottomRight, FVector2D(CenterOffset, CenterOffset), SegmentSize, FVector2D(0.5f, 0.5f), FVector2D(0.5f, 0.5f));
		HitBottomRight->SetRenderTransformAngle(45.0f);
	}

	const bool bShowDamage = HitAge <= FeedbackData->DamageNumberDuration && WeaponComponent->GetLastConfirmedDamage() > 0.0f;
	if (!DamageText)
	{
		return;
	}

	DamageText->SetVisibility(bShowDamage ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	if (!bShowDamage)
	{
		return;
	}

	const float Progress = FMath::Clamp(HitAge / FeedbackData->DamageNumberDuration, 0.0f, 1.0f);
	FLinearColor DamageColor = bFatalHit ? FeedbackData->FatalDamageNumberColor : FeedbackData->DamageNumberColor;
	DamageColor.A *= 1.0f - Progress;

	const FString DamageString = bFatalHit
		? FString::Printf(TEXT("ELIM +%.0f"), WeaponComponent->GetLastConfirmedDamage())
		: FString::Printf(TEXT("+%.0f"), WeaponComponent->GetLastConfirmedDamage());
	DamageText->SetText(FText::FromString(DamageString));
	DamageText->SetColorAndOpacity(FSlateColor(DamageColor));
	DamageText->SetRenderOpacity(DamageColor.A);
	DamageText->SetRenderScale(FVector2D(bFatalHit ? 1.2f : 1.0f, bFatalHit ? 1.2f : 1.0f));
	SetCanvasSlot(DamageText, FVector2D(95.0f, -78.0f - FeedbackData->DamageNumberRise * Progress), FVector2D(240.0f, 48.0f), FVector2D(0.5f, 0.5f), FVector2D(0.5f, 0.5f));
}

void UBOCombatHUDWidget::SetBoxBrush(UImage* Image, const FLinearColor& Color, const FVector2D& Size) const
{
	if (!Image)
	{
		return;
	}

	FSlateBrush Brush = *FCoreStyle::Get().GetBrush(TEXT("WhiteBrush"));
	Brush.DrawAs = ESlateBrushDrawType::Box;
	Brush.ImageSize = Size;
	Image->SetBrush(Brush);
	Image->SetColorAndOpacity(Color);
}

void UBOCombatHUDWidget::SetCanvasSlot(UWidget* Widget, const FVector2D& Position, const FVector2D& Size, const FVector2D& Anchor, const FVector2D& Alignment) const
{
	if (!Widget)
	{
		return;
	}

	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Widget->Slot);
	if (!CanvasSlot)
	{
		return;
	}

	CanvasSlot->SetAnchors(FAnchors(Anchor.X, Anchor.Y));
	CanvasSlot->SetAlignment(Alignment);
	CanvasSlot->SetPosition(Position);
	CanvasSlot->SetSize(Size);
}
