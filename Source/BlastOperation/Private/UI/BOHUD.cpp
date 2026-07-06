#include "UI/BOHUD.h"

#include "CanvasItem.h"
#include "Character/BOCharacter.h"
#include "Components/BOHealthComponent.h"
#include "Engine/Canvas.h"
#include "UI/BOCombatFeedbackData.h"
#include "Weapons/BOWeaponComponent.h"

namespace
{
const UBOCombatFeedbackData* LoadFeedbackData()
{
	static const UBOCombatFeedbackData* FeedbackData = LoadObject<UBOCombatFeedbackData>(nullptr, TEXT("/Game/BlastOperation/UI/Data/DA_BO_CombatFeedback.DA_BO_CombatFeedback"));
	return FeedbackData ? FeedbackData : GetDefault<UBOCombatFeedbackData>();
}
}

void ABOHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const UBOCombatFeedbackData* FeedbackData = LoadFeedbackData();
	const ABOCharacter* Character = GetOwningPawn() ? Cast<ABOCharacter>(GetOwningPawn()) : nullptr;
	const UBOWeaponComponent* WeaponComponent = Character ? Character->GetWeaponComponent() : nullptr;

	const float CrosshairSize = FeedbackData ? FeedbackData->CrosshairHalfLength : 8.0f;
	const float CrosshairBaseGap = FeedbackData ? FeedbackData->CrosshairBaseGap : 5.0f;
	const float CrosshairSpreadScale = FeedbackData ? FeedbackData->CrosshairSpreadScale : 10.0f;
	const float CrosshairMaxDynamicGap = FeedbackData ? FeedbackData->CrosshairMaxDynamicGap : 30.0f;
	const float CrosshairThickness = FeedbackData ? FeedbackData->CrosshairLineThickness : 1.0f;
	const float CurrentSpreadDegrees = WeaponComponent ? WeaponComponent->GetCurrentSpreadDegrees() : 0.0f;
	const float CrosshairGap = CrosshairBaseGap + FMath::Min(CrosshairMaxDynamicGap, CurrentSpreadDegrees * CrosshairSpreadScale);
	const FLinearColor CrosshairColor = FeedbackData ? FeedbackData->CrosshairColor : FLinearColor::White;

	auto DrawCrosshairLine = [this, CrosshairColor, CrosshairThickness](const FVector2D& Start, const FVector2D& End)
	{
		FCanvasLineItem LineItem(Start, End);
		LineItem.SetColor(CrosshairColor);
		LineItem.LineThickness = CrosshairThickness;
		Canvas->DrawItem(LineItem);
	};

	DrawCrosshairLine(FVector2D(Center.X - CrosshairGap - CrosshairSize, Center.Y), FVector2D(Center.X - CrosshairGap, Center.Y));
	DrawCrosshairLine(FVector2D(Center.X + CrosshairGap, Center.Y), FVector2D(Center.X + CrosshairGap + CrosshairSize, Center.Y));
	DrawCrosshairLine(FVector2D(Center.X, Center.Y - CrosshairGap - CrosshairSize), FVector2D(Center.X, Center.Y - CrosshairGap));
	DrawCrosshairLine(FVector2D(Center.X, Center.Y + CrosshairGap), FVector2D(Center.X, Center.Y + CrosshairGap + CrosshairSize));

	if (!Character)
	{
		return;
	}

	const UBOHealthComponent* HealthComponent = Character->GetHealthComponent();
	const float Health = HealthComponent ? HealthComponent->GetHealth() : 0.0f;
	const int32 Ammo = WeaponComponent ? WeaponComponent->GetAmmoInMagazine() : 0;
	const int32 Magazine = WeaponComponent ? WeaponComponent->GetMagazineSize() : 0;
	const FText WeaponName = WeaponComponent ? WeaponComponent->GetDisplayName() : FText::FromString(TEXT("No Weapon"));
	const int32 WeaponSlot = WeaponComponent ? WeaponComponent->GetCurrentWeaponSlot() + 1 : 0;

	const FString ReloadText = WeaponComponent && WeaponComponent->IsReloading()
		? FString::Printf(TEXT(" | RELOADING %.1fs"), WeaponComponent->GetReloadRemaining())
		: FString();
	const FString StatusText = FString::Printf(TEXT("[%d] %s | HP %.0f | Ammo %d / %d%s"), WeaponSlot, *WeaponName.ToString(), Health, Ammo, Magazine, *ReloadText);
	FCanvasTextItem TextItem(FVector2D(32.0f, Canvas->ClipY - 64.0f), FText::FromString(StatusText), GEngine->GetSmallFont(), FeedbackData ? FeedbackData->StatusTextColor : FLinearColor::White);
	TextItem.EnableShadow(FeedbackData ? FeedbackData->ShadowColor : FLinearColor::Black);
	Canvas->DrawItem(TextItem);

	if (WeaponComponent && GetWorld() && FeedbackData)
	{
		const float HitAge = GetWorld()->GetTimeSeconds() - WeaponComponent->GetLastHitConfirmTime();
		const bool bFatalHit = WeaponComponent->WasLastHitFatal();

		if (HitAge <= FeedbackData->HitMarkerDuration)
		{
			const float HitmarkerSize = FeedbackData->HitMarkerLength;
			const float HitmarkerGap = FeedbackData->HitMarkerGap;
			const FLinearColor HitmarkerColor = bFatalHit ? FeedbackData->FatalHitMarkerColor : FeedbackData->HitMarkerColor;

			FCanvasLineItem HitTopLeft(FVector2D(Center.X - HitmarkerGap, Center.Y - HitmarkerGap), FVector2D(Center.X - HitmarkerSize, Center.Y - HitmarkerSize));
			HitTopLeft.SetColor(HitmarkerColor);
			Canvas->DrawItem(HitTopLeft);

			FCanvasLineItem HitTopRight(FVector2D(Center.X + HitmarkerGap, Center.Y - HitmarkerGap), FVector2D(Center.X + HitmarkerSize, Center.Y - HitmarkerSize));
			HitTopRight.SetColor(HitmarkerColor);
			Canvas->DrawItem(HitTopRight);

			FCanvasLineItem HitBottomLeft(FVector2D(Center.X - HitmarkerGap, Center.Y + HitmarkerGap), FVector2D(Center.X - HitmarkerSize, Center.Y + HitmarkerSize));
			HitBottomLeft.SetColor(HitmarkerColor);
			Canvas->DrawItem(HitBottomLeft);

			FCanvasLineItem HitBottomRight(FVector2D(Center.X + HitmarkerGap, Center.Y + HitmarkerGap), FVector2D(Center.X + HitmarkerSize, Center.Y + HitmarkerSize));
			HitBottomRight.SetColor(HitmarkerColor);
			Canvas->DrawItem(HitBottomRight);
		}

		if (HitAge <= FeedbackData->DamageNumberDuration && WeaponComponent->GetLastConfirmedDamage() > 0.0f)
		{
			const float Progress = FMath::Clamp(HitAge / FeedbackData->DamageNumberDuration, 0.0f, 1.0f);
			const float Alpha = 1.0f - Progress;
			FLinearColor DamageColor = bFatalHit ? FeedbackData->FatalDamageNumberColor : FeedbackData->DamageNumberColor;
			DamageColor.A *= Alpha;

			const FString DamageText = bFatalHit
				? FString::Printf(TEXT("ELIM +%.0f"), WeaponComponent->GetLastConfirmedDamage())
				: FString::Printf(TEXT("+%.0f"), WeaponComponent->GetLastConfirmedDamage());
			FCanvasTextItem DamageItem(
				FVector2D(Center.X + 22.0f, Center.Y - 48.0f - FeedbackData->DamageNumberRise * Progress),
				FText::FromString(DamageText),
				GEngine->GetSmallFont(),
				DamageColor);
			DamageItem.EnableShadow(FeedbackData->ShadowColor);
			DamageItem.Scale = FVector2D(bFatalHit ? 1.2f : 1.0f, bFatalHit ? 1.2f : 1.0f);
			Canvas->DrawItem(DamageItem);
		}
	}
}
