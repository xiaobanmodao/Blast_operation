#include "Character/BOCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/BOHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"
#include "InputCoreTypes.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"
#include "Weapons/BOWeaponComponent.h"

ABOCharacter::ABOCharacter()
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = true;

	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCamera->SetupAttachment(GetCapsuleComponent());
	FirstPersonCamera->SetRelativeLocation(FVector(-10.0f, 0.0f, 64.0f));
	FirstPersonCamera->bUsePawnControlRotation = true;

	HealthComponent = CreateDefaultSubobject<UBOHealthComponent>(TEXT("HealthComponent"));
	WeaponComponent = CreateDefaultSubobject<UBOWeaponComponent>(TEXT("WeaponComponent"));
	bWantsToFire = false;

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveActionFinder(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_Move.IA_BO_Move"));
	if (MoveActionFinder.Succeeded())
	{
		MoveAction = MoveActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> LookActionFinder(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_Look.IA_BO_Look"));
	if (LookActionFinder.Succeeded())
	{
		LookAction = LookActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> JumpActionFinder(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_Jump.IA_BO_Jump"));
	if (JumpActionFinder.Succeeded())
	{
		JumpAction = JumpActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> FireActionFinder(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_Fire.IA_BO_Fire"));
	if (FireActionFinder.Succeeded())
	{
		FireAction = FireActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> ReloadActionFinder(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_Reload.IA_BO_Reload"));
	if (ReloadActionFinder.Succeeded())
	{
		ReloadAction = ReloadActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> EquipPrimaryActionFinder(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_EquipPrimary.IA_BO_EquipPrimary"));
	if (EquipPrimaryActionFinder.Succeeded())
	{
		EquipPrimaryAction = EquipPrimaryActionFinder.Object;
	}

	static ConstructorHelpers::FObjectFinder<UInputAction> EquipSecondaryActionFinder(TEXT("/Game/BlastOperation/Input/Actions/IA_BO_EquipSecondary.IA_BO_EquipSecondary"));
	if (EquipSecondaryActionFinder.Succeeded())
	{
		EquipSecondaryAction = EquipSecondaryActionFinder.Object;
	}

	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->bOrientRotationToMovement = false;
		MoveComp->MaxWalkSpeed = 620.0f;
	}
}

void ABOCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (!EnhancedInput)
	{
		return;
	}

	if (MoveAction)
	{
		EnhancedInput->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABOCharacter::Move);
	}

	if (LookAction)
	{
		EnhancedInput->BindAction(LookAction, ETriggerEvent::Triggered, this, &ABOCharacter::Look);
	}

	if (JumpAction)
	{
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInput->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}

	if (FireAction)
	{
		EnhancedInput->BindAction(FireAction, ETriggerEvent::Started, this, &ABOCharacter::StartFire);
		EnhancedInput->BindAction(FireAction, ETriggerEvent::Completed, this, &ABOCharacter::StopFire);
		EnhancedInput->BindAction(FireAction, ETriggerEvent::Canceled, this, &ABOCharacter::StopFire);
	}

	if (ReloadAction)
	{
		EnhancedInput->BindAction(ReloadAction, ETriggerEvent::Started, this, &ABOCharacter::ReloadWeapon);
	}

	if (EquipPrimaryAction)
	{
		EnhancedInput->BindAction(EquipPrimaryAction, ETriggerEvent::Started, this, &ABOCharacter::EquipPrimaryWeapon);
	}

	if (EquipSecondaryAction)
	{
		EnhancedInput->BindAction(EquipSecondaryAction, ETriggerEvent::Started, this, &ABOCharacter::EquipSecondaryWeapon);
	}
}

void ABOCharacter::Move(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	if (!Controller)
	{
		return;
	}

	const FRotator YawRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}

void ABOCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerYawInput(LookAxisVector.X);
	AddControllerPitchInput(LookAxisVector.Y);
}

void ABOCharacter::StartFire()
{
	if (!WeaponComponent || !FirstPersonCamera)
	{
		return;
	}

	bWantsToFire = true;
	FireOnce();

	if (WeaponComponent->IsAutomatic())
	{
		GetWorldTimerManager().SetTimer(FireTimerHandle, this, &ABOCharacter::FireOnce, WeaponComponent->GetSecondsBetweenShots(), true);
	}
}

void ABOCharacter::StopFire()
{
	bWantsToFire = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ABOCharacter::FireOnce()
{
	if (!bWantsToFire || !WeaponComponent || !FirstPersonCamera || WeaponComponent->IsReloading())
	{
		return;
	}

	if (WeaponComponent->GetAmmoInMagazine() <= 0)
	{
		WeaponComponent->Reload();
		StopFire();
		return;
	}

	WeaponComponent->Fire(FirstPersonCamera->GetComponentLocation(), FirstPersonCamera->GetForwardVector());
	ApplyLocalFireFeedback();
}

void ABOCharacter::ReloadWeapon()
{
	if (!WeaponComponent)
	{
		return;
	}

	StopFire();
	WeaponComponent->Reload();
}

void ABOCharacter::EquipPrimaryWeapon()
{
	if (!WeaponComponent)
	{
		return;
	}

	StopFire();
	WeaponComponent->EquipWeaponSlot(0);
}

void ABOCharacter::EquipSecondaryWeapon()
{
	if (!WeaponComponent)
	{
		return;
	}

	StopFire();
	WeaponComponent->EquipWeaponSlot(1);
}

void ABOCharacter::ApplyLocalFireFeedback()
{
	if (!IsLocallyControlled() || !Controller || !WeaponComponent)
	{
		return;
	}

	WeaponComponent->RecordLocalShotFeedback();
	const FVector2D RecoilOffset = WeaponComponent->ConsumeRecoilOffsetDegrees();
	AddControllerPitchInput(-RecoilOffset.X);
	AddControllerYawInput(RecoilOffset.Y);
}
