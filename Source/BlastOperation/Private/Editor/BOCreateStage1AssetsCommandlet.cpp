#include "Editor/BOCreateStage1AssetsCommandlet.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Core/BOGameplayTags.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "InputModifiers.h"
#include "Misc/PackageName.h"
#include "UI/BOCombatFeedbackData.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"
#include "Weapons/BOWeaponData.h"

namespace
{
template <typename T>
T* LoadOrCreateAsset(const TCHAR* LongPackageName, const TCHAR* AssetName)
{
	const FString ObjectPath = FString::Printf(TEXT("%s.%s"), LongPackageName, AssetName);
	if (T* ExistingAsset = LoadObject<T>(nullptr, *ObjectPath))
	{
		return ExistingAsset;
	}

	UPackage* Package = CreatePackage(LongPackageName);
	Package->FullyLoad();

	T* Asset = NewObject<T>(Package, AssetName, RF_Public | RF_Standalone | RF_Transactional);
	FAssetRegistryModule::AssetCreated(Asset);
	Package->MarkPackageDirty();
	return Asset;
}

bool SaveAsset(UObject* Asset)
{
	if (!Asset)
	{
		return false;
	}

	UPackage* Package = Asset->GetPackage();
	Package->MarkPackageDirty();

	const FString PackageName = Package->GetName();
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(PackageName, FPackageName::GetAssetPackageExtension());

	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	SaveArgs.SaveFlags = SAVE_NoError;

	return UPackage::SavePackage(Package, Asset, *PackageFileName, SaveArgs);
}

UInputModifierSwizzleAxis* MakeSwizzle(UObject* Outer)
{
	UInputModifierSwizzleAxis* Modifier = NewObject<UInputModifierSwizzleAxis>(Outer);
	Modifier->Order = EInputAxisSwizzle::YXZ;
	return Modifier;
}

UInputModifierNegate* MakeNegate(UObject* Outer)
{
	return NewObject<UInputModifierNegate>(Outer);
}

void ConfigureAction(UInputAction* Action, EInputActionValueType ValueType)
{
	Action->ValueType = ValueType;
	Action->bReserveAllMappings = false;
	Action->AccumulationBehavior = EInputActionAccumulationBehavior::Cumulative;
}

FEnhancedActionKeyMapping& MapKey(UInputMappingContext* Context, UInputAction* Action, const FKey& Key)
{
	return Context->MapKey(Action, Key);
}

void ConfigureStage1InputAssets()
{
	UInputAction* Move = LoadOrCreateAsset<UInputAction>(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_Move"), TEXT("IA_BO_Move"));
	UInputAction* Look = LoadOrCreateAsset<UInputAction>(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_Look"), TEXT("IA_BO_Look"));
	UInputAction* Jump = LoadOrCreateAsset<UInputAction>(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_Jump"), TEXT("IA_BO_Jump"));
	UInputAction* Fire = LoadOrCreateAsset<UInputAction>(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_Fire"), TEXT("IA_BO_Fire"));
	UInputAction* Reload = LoadOrCreateAsset<UInputAction>(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_Reload"), TEXT("IA_BO_Reload"));
	UInputAction* EquipPrimary = LoadOrCreateAsset<UInputAction>(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_EquipPrimary"), TEXT("IA_BO_EquipPrimary"));
	UInputAction* EquipSecondary = LoadOrCreateAsset<UInputAction>(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_EquipSecondary"), TEXT("IA_BO_EquipSecondary"));

	ConfigureAction(Move, EInputActionValueType::Axis2D);
	ConfigureAction(Look, EInputActionValueType::Axis2D);
	ConfigureAction(Jump, EInputActionValueType::Boolean);
	ConfigureAction(Fire, EInputActionValueType::Boolean);
	ConfigureAction(Reload, EInputActionValueType::Boolean);
	ConfigureAction(EquipPrimary, EInputActionValueType::Boolean);
	ConfigureAction(EquipSecondary, EInputActionValueType::Boolean);

	UInputMappingContext* CombatContext = LoadOrCreateAsset<UInputMappingContext>(TEXT("/Game/BlastOperation/Input/IMC_BO_Combat"), TEXT("IMC_BO_Combat"));
	CombatContext->UnmapAll();

	MapKey(CombatContext, Jump, EKeys::SpaceBar);
	MapKey(CombatContext, Jump, EKeys::Gamepad_FaceButton_Bottom);

	FEnhancedActionKeyMapping& MoveForward = MapKey(CombatContext, Move, EKeys::W);
	MoveForward.Modifiers.Add(MakeSwizzle(CombatContext));

	FEnhancedActionKeyMapping& MoveBackward = MapKey(CombatContext, Move, EKeys::S);
	MoveBackward.Modifiers.Add(MakeSwizzle(CombatContext));
	MoveBackward.Modifiers.Add(MakeNegate(CombatContext));

	FEnhancedActionKeyMapping& MoveLeft = MapKey(CombatContext, Move, EKeys::A);
	MoveLeft.Modifiers.Add(MakeNegate(CombatContext));

	MapKey(CombatContext, Move, EKeys::D);

	FEnhancedActionKeyMapping& MoveUp = MapKey(CombatContext, Move, EKeys::Up);
	MoveUp.Modifiers.Add(MakeSwizzle(CombatContext));

	FEnhancedActionKeyMapping& MoveDown = MapKey(CombatContext, Move, EKeys::Down);
	MoveDown.Modifiers.Add(MakeSwizzle(CombatContext));
	MoveDown.Modifiers.Add(MakeNegate(CombatContext));

	FEnhancedActionKeyMapping& MoveArrowLeft = MapKey(CombatContext, Move, EKeys::Left);
	MoveArrowLeft.Modifiers.Add(MakeNegate(CombatContext));

	MapKey(CombatContext, Move, EKeys::Right);
	MapKey(CombatContext, Move, EKeys::Gamepad_Left2D);

	MapKey(CombatContext, Look, EKeys::Mouse2D);
	MapKey(CombatContext, Look, EKeys::Gamepad_Right2D);

	MapKey(CombatContext, Fire, EKeys::LeftMouseButton);
	MapKey(CombatContext, Fire, EKeys::Gamepad_RightTriggerAxis);
	MapKey(CombatContext, Fire, EKeys::Gamepad_RightShoulder);

	MapKey(CombatContext, Reload, EKeys::R);
	MapKey(CombatContext, Reload, EKeys::Gamepad_FaceButton_Left);

	MapKey(CombatContext, EquipPrimary, EKeys::One);
	MapKey(CombatContext, EquipPrimary, EKeys::Gamepad_DPad_Up);
	MapKey(CombatContext, EquipSecondary, EKeys::Two);
	MapKey(CombatContext, EquipSecondary, EKeys::Gamepad_DPad_Right);

	SaveAsset(Move);
	SaveAsset(Look);
	SaveAsset(Jump);
	SaveAsset(Fire);
	SaveAsset(Reload);
	SaveAsset(EquipPrimary);
	SaveAsset(EquipSecondary);
	SaveAsset(CombatContext);
}

void ConfigureStage1WeaponAssets()
{
	UBOWeaponData* Rifle = LoadOrCreateAsset<UBOWeaponData>(TEXT("/Game/BlastOperation/Weapons/Data/DA_BO_Rifle"), TEXT("DA_BO_Rifle"));
	Rifle->DisplayName = FText::FromString(TEXT("BR-01 Kestrel"));
	Rifle->WeaponTag = BOGameplayTags::Weapon_Rifle;
	Rifle->MagazineSize = 30;
	Rifle->bIsAutomatic = true;
	Rifle->FireRateRPM = 640.0f;
	Rifle->Damage = 30.0f;
	Rifle->Range = 12000.0f;
	Rifle->HeadshotMultiplier = 4.0f;
	Rifle->ReloadDuration = 1.95f;
	Rifle->StationarySpreadDegrees = 0.045f;
	Rifle->MovingSpreadDegrees = 1.45f;
	Rifle->RecoilPitchDegrees = 0.36f;
	Rifle->RecoilYawDegrees = 0.14f;
	SaveAsset(Rifle);

	UBOWeaponData* Pistol = LoadOrCreateAsset<UBOWeaponData>(TEXT("/Game/BlastOperation/Weapons/Data/DA_BO_Pistol"), TEXT("DA_BO_Pistol"));
	Pistol->DisplayName = FText::FromString(TEXT("BP-9 Sparrow"));
	Pistol->WeaponTag = BOGameplayTags::Weapon_Pistol;
	Pistol->MagazineSize = 12;
	Pistol->bIsAutomatic = false;
	Pistol->FireRateRPM = 360.0f;
	Pistol->Damage = 34.0f;
	Pistol->Range = 9000.0f;
	Pistol->HeadshotMultiplier = 3.0f;
	Pistol->ReloadDuration = 1.35f;
	Pistol->StationarySpreadDegrees = 0.095f;
	Pistol->MovingSpreadDegrees = 1.25f;
	Pistol->RecoilPitchDegrees = 0.48f;
	Pistol->RecoilYawDegrees = 0.2f;
	SaveAsset(Pistol);
}

void ConfigureStage1FeedbackAssets()
{
	UBOCombatFeedbackData* Feedback = LoadOrCreateAsset<UBOCombatFeedbackData>(TEXT("/Game/BlastOperation/UI/Data/DA_BO_CombatFeedback"), TEXT("DA_BO_CombatFeedback"));
	Feedback->CrosshairColor = FLinearColor(0.88f, 0.96f, 1.0f, 1.0f);
	Feedback->HitMarkerColor = FLinearColor(1.0f, 0.78f, 0.16f, 1.0f);
	Feedback->FatalHitMarkerColor = FLinearColor(1.0f, 0.18f, 0.1f, 1.0f);
	Feedback->DamageNumberColor = FLinearColor(1.0f, 0.94f, 0.66f, 1.0f);
	Feedback->FatalDamageNumberColor = FLinearColor(1.0f, 0.25f, 0.14f, 1.0f);
	Feedback->StatusTextColor = FLinearColor(0.90f, 0.96f, 1.0f, 1.0f);
	Feedback->ShadowColor = FLinearColor(0.0f, 0.0f, 0.0f, 0.85f);
	Feedback->HitMarkerDuration = 0.2f;
	Feedback->DamageNumberDuration = 0.58f;
	Feedback->CrosshairHalfLength = 7.0f;
	Feedback->HitMarkerLength = 16.0f;
	Feedback->HitMarkerGap = 4.5f;
	Feedback->DamageNumberRise = 28.0f;
	SaveAsset(Feedback);
}
}

UBOCreateStage1AssetsCommandlet::UBOCreateStage1AssetsCommandlet()
{
	IsClient = false;
	IsEditor = true;
	IsServer = false;
	LogToConsole = true;
}

int32 UBOCreateStage1AssetsCommandlet::Main(const FString& Params)
{
	UE_LOG(LogTemp, Display, TEXT("Creating Blast Operation Stage 1 project-owned assets."));

	ConfigureStage1InputAssets();
	ConfigureStage1WeaponAssets();
	ConfigureStage1FeedbackAssets();

	UE_LOG(LogTemp, Display, TEXT("Blast Operation Stage 1 assets created."));
	return 0;
}
