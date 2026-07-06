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
	const float CrosshairSize = FeedbackData ? FeedbackData->CrosshairHalfLength : 8.0f;
	const FLinearColor CrosshairColor = FeedbackData ? FeedbackData->CrosshairColor : FLinearColor::White;

	FCanvasLineItem Horizontal(FVector2D(Center.X - CrosshairSize, Center.Y), FVector2D(Center.X + CrosshairSize, Center.Y));
	Horizontal.SetColor(CrosshairColor);
	Canvas->DrawItem(Horizontal);

	FCanvasLineItem Vertical(FVector2D(Center.X, Center.Y - CrosshairSize), FVector2D(Center.X, Center.Y + CrosshairSize));
	Vertical.SetColor(CrosshairColor);
	Canvas->DrawItem(Vertical);

	const ABOCharacter* Character = GetOwningPawn() ? Cast<ABOCharacter>(GetOwningPawn()) : nullptr;
	if (!Character)
	{
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
	const FString StatusText = FString::Printf(TEXT("[%d] %s | HP %.0f | Ammo %d / %d%s"), WeaponSlot, *WeaponName.ToString(), Health, Ammo, Magazine, *ReloadText);
	FCanvasTextItem TextItem(FVector2D(32.0f, Canvas->ClipY - 64.0f), FText::FromString(StatusText), GEngine->GetSmallFont(), FeedbackData ? FeedbackData->StatusTextColor : FLinearColor::White);
	TextItem.EnableShadow(FeedbackData ? FeedbackData->ShadowColor : FLinearColor::Black);
	Canvas->DrawItem(TextItem);

	if (WeaponComponent && GetWorld() && FeedbackData && GetWorld()->GetTimeSeconds() - WeaponComponent->GetLastHitConfirmTime() <= FeedbackData->HitMarkerDuration)
	{
		const float HitmarkerSize = FeedbackData->HitMarkerLength;
		const float HitmarkerGap = FeedbackData->HitMarkerGap;
		const FLinearColor HitmarkerColor = FeedbackData->HitMarkerColor;

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
}
