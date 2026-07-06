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
#include "Widgets/SWidget.h"

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

TSharedRef<SWidget> UBOCombatHUDWidget::RebuildWidget()
{
	if (!WidgetTree)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
	}

	if (!RootPanel)
	{
		BuildWidgetTree();
	}

	return Super::RebuildWidget();
}

void UBOCombatHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	UpdateCombatReadout();
}

void UBOCombatHUDWidget::BuildWidgetTree()
{
	if (!WidgetTree)
	{
		WidgetTree = NewObject<UWidgetTree>(this, TEXT("WidgetTree"));
	}

	RootPanel = WidgetTree->ConstructWidget<UCanvasPanel>(UCanvasPanel::StaticClass(), TEXT("CombatHUDRoot"));
	WidgetTree->RootWidget = RootPanel;

	auto AddImage = [this](const FName Name) -> UImage*
	{
		UImage* Image = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), Name);
		RootPanel->AddChild(Image);
		Image->SetVisibility(ESlateVisibility::HitTestInvisible);
		return Image;
	};

	auto AddText = [this](const FName Name, float FontSize, const FLinearColor& Color, ETextJustify::Type Justification = ETextJustify::Left) -> UTextBlock*
	{
		UTextBlock* TextBlock = WidgetTree->ConstructWidget<UTextBlock>(UTextBlock::StaticClass(), Name);
		RootPanel->AddChild(TextBlock);
		TextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
		ConfigureText(TextBlock, FontSize, Color, Justification);
		return TextBlock;
	};

	const FLinearColor PanelColor(0.02f, 0.025f, 0.03f, 0.72f);
	const FLinearColor PanelColorSoft(0.03f, 0.04f, 0.045f, 0.58f);
	const FLinearColor AccentCyan(0.18f, 0.92f, 0.88f, 0.95f);
	const FLinearColor AccentAmber(1.0f, 0.72f, 0.22f, 0.95f);
	const FLinearColor MutedText(0.62f, 0.72f, 0.74f, 1.0f);

	TopBarPanel = AddImage(TEXT("TopBarPanel"));
	SetBoxBrush(TopBarPanel, PanelColor, FVector2D(620.0f, 58.0f));
	SetCanvasSlot(TopBarPanel, FVector2D(0.0f, 18.0f), FVector2D(620.0f, 58.0f), FVector2D(0.5f, 0.0f), FVector2D(0.5f, 0.0f));

	TopBarAccent = AddImage(TEXT("TopBarAccent"));
	SetBoxBrush(TopBarAccent, AccentCyan, FVector2D(620.0f, 3.0f));
	SetCanvasSlot(TopBarAccent, FVector2D(0.0f, 18.0f), FVector2D(620.0f, 3.0f), FVector2D(0.5f, 0.0f), FVector2D(0.5f, 0.0f));

	ModeText = AddText(TEXT("ModeText"), 15.0f, FLinearColor::White, ETextJustify::Center);
	SetCanvasSlot(ModeText, FVector2D(0.0f, 25.0f), FVector2D(260.0f, 24.0f), FVector2D(0.5f, 0.0f), FVector2D(0.5f, 0.0f));
	ModeText->SetText(FText::FromString(TEXT("BLAST OPERATION")));

	ScoreText = AddText(TEXT("ScoreText"), 22.0f, AccentAmber, ETextJustify::Center);
	SetCanvasSlot(ScoreText, FVector2D(0.0f, 46.0f), FVector2D(320.0f, 30.0f), FVector2D(0.5f, 0.0f), FVector2D(0.5f, 0.0f));
	ScoreText->SetText(FText::FromString(TEXT("ATTACK 0  |  0 DEFENSE")));

	RoundText = AddText(TEXT("RoundText"), 12.0f, MutedText, ETextJustify::Center);
	SetCanvasSlot(RoundText, FVector2D(0.0f, 76.0f), FVector2D(360.0f, 22.0f), FVector2D(0.5f, 0.0f), FVector2D(0.5f, 0.0f));
	RoundText->SetText(FText::FromString(TEXT("TEST RANGE - LIVE FIRE")));

	HealthPanel = AddImage(TEXT("HealthPanel"));
	SetBoxBrush(HealthPanel, PanelColor, FVector2D(292.0f, 96.0f));
	SetCanvasSlot(HealthPanel, FVector2D(28.0f, -36.0f), FVector2D(292.0f, 96.0f), FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f));

	HealthBarBack = AddImage(TEXT("HealthBarBack"));
	SetBoxBrush(HealthBarBack, PanelColorSoft, FVector2D(228.0f, 8.0f));
	SetCanvasSlot(HealthBarBack, FVector2D(52.0f, -58.0f), FVector2D(228.0f, 8.0f), FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f));

	HealthBarFill = AddImage(TEXT("HealthBarFill"));
	SetBoxBrush(HealthBarFill, AccentCyan, FVector2D(228.0f, 8.0f));
	SetCanvasSlot(HealthBarFill, FVector2D(52.0f, -58.0f), FVector2D(228.0f, 8.0f), FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f));

	HealthLabelText = AddText(TEXT("HealthLabelText"), 12.0f, MutedText);
	SetCanvasSlot(HealthLabelText, FVector2D(52.0f, -113.0f), FVector2D(180.0f, 20.0f), FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f));
	HealthLabelText->SetText(FText::FromString(TEXT("VITALS")));

	HealthValueText = AddText(TEXT("HealthValueText"), 34.0f, FLinearColor::White);
	SetCanvasSlot(HealthValueText, FVector2D(52.0f, -98.0f), FVector2D(110.0f, 42.0f), FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f));

	ArmorText = AddText(TEXT("ArmorText"), 13.0f, MutedText, ETextJustify::Right);
	SetCanvasSlot(ArmorText, FVector2D(158.0f, -90.0f), FVector2D(122.0f, 24.0f), FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f));

	AmmoPanel = AddImage(TEXT("AmmoPanel"));
	SetBoxBrush(AmmoPanel, PanelColor, FVector2D(330.0f, 104.0f));
	SetCanvasSlot(AmmoPanel, FVector2D(-28.0f, -36.0f), FVector2D(330.0f, 104.0f), FVector2D(1.0f, 1.0f), FVector2D(1.0f, 1.0f));

	WeaponNameText = AddText(TEXT("WeaponNameText"), 15.0f, MutedText, ETextJustify::Right);
	SetCanvasSlot(WeaponNameText, FVector2D(-56.0f, -120.0f), FVector2D(280.0f, 24.0f), FVector2D(1.0f, 1.0f), FVector2D(1.0f, 1.0f));

	AmmoValueText = AddText(TEXT("AmmoValueText"), 40.0f, FLinearColor::White, ETextJustify::Right);
	SetCanvasSlot(AmmoValueText, FVector2D(-118.0f, -96.0f), FVector2D(186.0f, 50.0f), FVector2D(1.0f, 1.0f), FVector2D(1.0f, 1.0f));

	AmmoLabelText = AddText(TEXT("AmmoLabelText"), 16.0f, AccentAmber, ETextJustify::Left);
	SetCanvasSlot(AmmoLabelText, FVector2D(-112.0f, -80.0f), FVector2D(70.0f, 24.0f), FVector2D(1.0f, 1.0f), FVector2D(1.0f, 1.0f));

	ReloadText = AddText(TEXT("ReloadText"), 14.0f, AccentAmber, ETextJustify::Right);
	SetCanvasSlot(ReloadText, FVector2D(-56.0f, -58.0f), FVector2D(260.0f, 24.0f), FVector2D(1.0f, 1.0f), FVector2D(1.0f, 1.0f));

	SlotPanel = AddImage(TEXT("SlotPanel"));
	SetBoxBrush(SlotPanel, PanelColorSoft, FVector2D(330.0f, 42.0f));
	SetCanvasSlot(SlotPanel, FVector2D(0.0f, -34.0f), FVector2D(330.0f, 42.0f), FVector2D(0.5f, 1.0f), FVector2D(0.5f, 1.0f));

	SlotPrimaryText = AddText(TEXT("SlotPrimaryText"), 14.0f, FLinearColor::White, ETextJustify::Center);
	SetCanvasSlot(SlotPrimaryText, FVector2D(-82.0f, -65.0f), FVector2D(146.0f, 26.0f), FVector2D(0.5f, 1.0f), FVector2D(0.5f, 1.0f));

	SlotSecondaryText = AddText(TEXT("SlotSecondaryText"), 14.0f, FLinearColor::White, ETextJustify::Center);
	SetCanvasSlot(SlotSecondaryText, FVector2D(82.0f, -65.0f), FVector2D(146.0f, 26.0f), FVector2D(0.5f, 1.0f), FVector2D(0.5f, 1.0f));

	AccuracyText = AddText(TEXT("AccuracyText"), 12.0f, MutedText, ETextJustify::Center);
	SetCanvasSlot(AccuracyText, FVector2D(0.0f, -100.0f), FVector2D(240.0f, 24.0f), FVector2D(0.5f, 1.0f), FVector2D(0.5f, 1.0f));

	StatusText = AddText(TEXT("CombatStatusText"), 12.0f, MutedText, ETextJustify::Center);
	SetCanvasSlot(StatusText, FVector2D(0.0f, -126.0f), FVector2D(520.0f, 24.0f), FVector2D(0.5f, 1.0f), FVector2D(0.5f, 1.0f));

	DamageText = AddText(TEXT("DamageConfirmText"), 22.0f, FLinearColor::White, ETextJustify::Center);
	SetCanvasSlot(DamageText, FVector2D(95.0f, -78.0f), FVector2D(240.0f, 48.0f), FVector2D(0.5f, 0.5f), FVector2D(0.5f, 0.5f));

	CrosshairLeft = AddImage(TEXT("CrosshairLeft"));
	CrosshairRight = AddImage(TEXT("CrosshairRight"));
	CrosshairTop = AddImage(TEXT("CrosshairTop"));
	CrosshairBottom = AddImage(TEXT("CrosshairBottom"));

	HitTopLeft = AddImage(TEXT("HitTopLeft"));
	HitTopRight = AddImage(TEXT("HitTopRight"));
	HitBottomLeft = AddImage(TEXT("HitBottomLeft"));
	HitBottomRight = AddImage(TEXT("HitBottomRight"));

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
			StatusText->SetText(FText::FromString(TEXT("WAITING FOR PLAYER")));
		}
		return;
	}

	const UBOHealthComponent* HealthComponent = Character->GetHealthComponent();
	const UBOWeaponComponent* WeaponComponent = Character->GetWeaponComponent();
	const float Health = HealthComponent ? HealthComponent->GetHealth() : 0.0f;
	const float MaxHealth = HealthComponent ? HealthComponent->GetMaxHealth() : 100.0f;
	const float Armor = HealthComponent ? HealthComponent->GetArmor() : 0.0f;
	const int32 Ammo = WeaponComponent ? WeaponComponent->GetAmmoInMagazine() : 0;
	const int32 Magazine = WeaponComponent ? WeaponComponent->GetMagazineSize() : 0;
	const FText WeaponName = WeaponComponent ? WeaponComponent->GetDisplayName() : FText::FromString(TEXT("No Weapon"));
	const int32 WeaponSlot = WeaponComponent ? WeaponComponent->GetCurrentWeaponSlot() + 1 : 0;
	const float Spread = WeaponComponent ? WeaponComponent->GetCurrentSpreadDegrees() : 0.0f;
	const float Accuracy = WeaponComponent ? (1.0f - WeaponComponent->GetNormalizedAccuracyPenalty()) : 1.0f;

	const FString ReloadStatus = WeaponComponent && WeaponComponent->IsReloading()
		? FString::Printf(TEXT("RELOADING %.1fs"), WeaponComponent->GetReloadRemaining())
		: FString();
	const FString Status = FString::Printf(TEXT("[%d] %s | HP %.0f | Ammo %d / %d%s"), WeaponSlot, *WeaponName.ToString(), Health, Ammo, Magazine, *ReloadStatus);
	const FLinearColor TextColor = FeedbackData ? FeedbackData->StatusTextColor : FLinearColor::White;
	const FLinearColor ShadowColor = FeedbackData ? FeedbackData->ShadowColor : FLinearColor::Black;
	const float HealthAlpha = FMath::Clamp(Health / FMath::Max(1.0f, MaxHealth), 0.0f, 1.0f);
	const FLinearColor HealthColor = HealthAlpha > 0.35f
		? FLinearColor(0.18f, 0.92f, 0.88f, 0.95f)
		: FLinearColor(1.0f, 0.26f, 0.18f, 0.95f);
	const FLinearColor ActiveSlotColor(1.0f, 0.72f, 0.22f, 1.0f);
	const FLinearColor InactiveSlotColor(0.62f, 0.72f, 0.74f, 1.0f);

	if (StatusText)
	{
		StatusText->SetText(FText::FromString(TEXT("LIVE FIRE READY")));
		StatusText->SetColorAndOpacity(FSlateColor(TextColor));
		StatusText->SetShadowColorAndOpacity(ShadowColor);
	}

	if (RoundText)
	{
		RoundText->SetText(FText::FromString(TEXT("TEST RANGE - LIVE FIRE")));
	}

	if (HealthValueText)
	{
		HealthValueText->SetText(FText::AsNumber(FMath::RoundToInt(Health)));
		HealthValueText->SetColorAndOpacity(FSlateColor(HealthColor));
		HealthValueText->SetShadowColorAndOpacity(ShadowColor);
	}

	if (ArmorText)
	{
		ArmorText->SetText(FText::FromString(FString::Printf(TEXT("ARMOR %.0f"), Armor)));
	}

	if (HealthBarFill)
	{
		const float HealthBarWidth = 228.0f * HealthAlpha;
		SetBoxBrush(HealthBarFill, HealthColor, FVector2D(HealthBarWidth, 8.0f));
		SetCanvasSlot(HealthBarFill, FVector2D(52.0f, -58.0f), FVector2D(HealthBarWidth, 8.0f), FVector2D(0.0f, 1.0f), FVector2D(0.0f, 1.0f));
	}

	if (WeaponNameText)
	{
		FString WeaponNameString = WeaponName.ToString();
		WeaponNameString.ToUpperInline();
		WeaponNameText->SetText(FText::FromString(WeaponNameString));
		WeaponNameText->SetColorAndOpacity(FSlateColor(TextColor));
		WeaponNameText->SetShadowColorAndOpacity(ShadowColor);
	}

	if (AmmoValueText)
	{
		AmmoValueText->SetText(FText::AsNumber(Ammo));
		AmmoValueText->SetColorAndOpacity(FSlateColor(Ammo <= FMath::Max(1, Magazine / 4) ? ActiveSlotColor : FLinearColor::White));
		AmmoValueText->SetShadowColorAndOpacity(ShadowColor);
	}

	if (AmmoLabelText)
	{
		AmmoLabelText->SetText(FText::FromString(FString::Printf(TEXT("/ %d"), Magazine)));
	}

	if (this->ReloadText)
	{
		this->ReloadText->SetText(ReloadStatus.IsEmpty() ? FText::FromString(TEXT("READY")) : FText::FromString(ReloadStatus));
		this->ReloadText->SetColorAndOpacity(FSlateColor(ReloadStatus.IsEmpty() ? InactiveSlotColor : ActiveSlotColor));
	}

	if (SlotPrimaryText)
	{
		SlotPrimaryText->SetText(FText::FromString(TEXT("[1] RIFLE")));
		SlotPrimaryText->SetColorAndOpacity(FSlateColor(WeaponSlot == 1 ? ActiveSlotColor : InactiveSlotColor));
	}

	if (SlotSecondaryText)
	{
		SlotSecondaryText->SetText(FText::FromString(TEXT("[2] PISTOL")));
		SlotSecondaryText->SetColorAndOpacity(FSlateColor(WeaponSlot == 2 ? ActiveSlotColor : InactiveSlotColor));
	}

	if (AccuracyText)
	{
		const int32 AccuracyPercent = FMath::RoundToInt(FMath::Clamp(Accuracy, 0.0f, 1.0f) * 100.0f);
		AccuracyText->SetText(FText::FromString(FString::Printf(TEXT("ACCURACY %d%%  |  SPREAD %.1f"), AccuracyPercent, Spread)));
	}

	OnCombatReadoutUpdated(FText::FromString(Status), Health, Ammo, Magazine, WeaponSlot);
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

	OnCrosshairUpdated(SpreadDegrees, Gap);
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
	const float Damage = WeaponComponent->GetLastConfirmedDamage();
	const FLinearColor HitColor = bFatalHit ? FeedbackData->FatalHitMarkerColor : FeedbackData->HitMarkerColor;
	const bool bShowHitMarker = HitAge <= FeedbackData->HitMarkerDuration;
	const float HitFeedbackDuration = FMath::Max(FeedbackData->DamageNumberDuration, FeedbackData->HitMarkerDuration);
	const float NormalizedAge = FMath::Clamp(HitAge / FMath::Max(0.01f, HitFeedbackDuration), 0.0f, 1.0f);

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

	const bool bShowDamage = HitAge <= FeedbackData->DamageNumberDuration && Damage > 0.0f;
	if (!DamageText)
	{
		OnHitFeedbackUpdated(Damage, bFatalHit, NormalizedAge);
		return;
	}

	DamageText->SetVisibility(bShowDamage ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Hidden);
	if (!bShowDamage)
	{
		OnHitFeedbackUpdated(Damage, bFatalHit, NormalizedAge);
		return;
	}

	const float Progress = FMath::Clamp(HitAge / FeedbackData->DamageNumberDuration, 0.0f, 1.0f);
	FLinearColor DamageColor = bFatalHit ? FeedbackData->FatalDamageNumberColor : FeedbackData->DamageNumberColor;
	DamageColor.A *= 1.0f - Progress;

	const FString DamageString = bFatalHit
		? FString::Printf(TEXT("ELIM +%.0f"), Damage)
		: FString::Printf(TEXT("+%.0f"), Damage);
	DamageText->SetText(FText::FromString(DamageString));
	DamageText->SetColorAndOpacity(FSlateColor(DamageColor));
	DamageText->SetRenderOpacity(DamageColor.A);
	DamageText->SetRenderScale(FVector2D(bFatalHit ? 1.2f : 1.0f, bFatalHit ? 1.2f : 1.0f));
	SetCanvasSlot(DamageText, FVector2D(95.0f, -78.0f - FeedbackData->DamageNumberRise * Progress), FVector2D(240.0f, 48.0f), FVector2D(0.5f, 0.5f), FVector2D(0.5f, 0.5f));
	OnHitFeedbackUpdated(Damage, bFatalHit, NormalizedAge);
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
