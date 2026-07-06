#include "UI/BOHUD.h"

#include "CanvasItem.h"
#include "Character/BOCharacter.h"
#include "Components/BOHealthComponent.h"
#include "Engine/Canvas.h"
#include "Weapons/BOWeaponComponent.h"

void ABOHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!Canvas)
	{
		return;
	}

	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);
	const float CrosshairSize = 8.0f;
	const FLinearColor CrosshairColor = FLinearColor::White;

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

	const FString ReloadText = WeaponComponent && WeaponComponent->IsReloading()
		? FString::Printf(TEXT(" | RELOADING %.1fs"), WeaponComponent->GetReloadRemaining())
		: FString();
	const FString StatusText = FString::Printf(TEXT("%s | HP %.0f | Ammo %d / %d%s"), *WeaponName.ToString(), Health, Ammo, Magazine, *ReloadText);
	FCanvasTextItem TextItem(FVector2D(32.0f, Canvas->ClipY - 64.0f), FText::FromString(StatusText), GEngine->GetSmallFont(), FLinearColor::White);
	TextItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(TextItem);

	if (WeaponComponent && GetWorld() && GetWorld()->GetTimeSeconds() - WeaponComponent->GetLastHitConfirmTime() <= 0.18f)
	{
		const float HitmarkerSize = 14.0f;
		const float HitmarkerGap = 5.0f;
		const FLinearColor HitmarkerColor = FLinearColor(1.0f, 0.88f, 0.2f, 1.0f);

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
