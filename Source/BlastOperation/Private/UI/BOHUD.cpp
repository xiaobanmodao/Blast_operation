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

	const FString StatusText = FString::Printf(TEXT("HP %.0f | Ammo %d / %d"), Health, Ammo, Magazine);
	FCanvasTextItem TextItem(FVector2D(32.0f, Canvas->ClipY - 64.0f), FText::FromString(StatusText), GEngine->GetSmallFont(), FLinearColor::White);
	TextItem.EnableShadow(FLinearColor::Black);
	Canvas->DrawItem(TextItem);
}

