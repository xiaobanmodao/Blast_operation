#include "Character/BOCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/BOHealthComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/StaticMesh.h"
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

	auto CreateWeaponViewMesh = [this](const FName ComponentName) -> UStaticMeshComponent*
	{
		UStaticMeshComponent* MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(ComponentName);
		MeshComponent->SetupAttachment(FirstPersonCamera);
		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComponent->SetCastShadow(false);
		MeshComponent->SetReceivesDecals(false);
		return MeshComponent;
	};

	WeaponBodyMesh = CreateWeaponViewMesh(TEXT("WeaponBody"));
	WeaponBarrelMesh = CreateWeaponViewMesh(TEXT("WeaponBarrel"));
	WeaponGripMesh = CreateWeaponViewMesh(TEXT("WeaponGrip"));
	WeaponSightMesh = CreateWeaponViewMesh(TEXT("WeaponSight"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMeshFinder(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMeshFinder.Succeeded())
	{
		WeaponBodyMesh->SetStaticMesh(CubeMeshFinder.Object);
		WeaponBarrelMesh->SetStaticMesh(CubeMeshFinder.Object);
		WeaponGripMesh->SetStaticMesh(CubeMeshFinder.Object);
		WeaponSightMesh->SetStaticMesh(CubeMeshFinder.Object);
	}

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

void ABOCharacter::BeginPlay()
{
	Super::BeginPlay();

	UpdateWeaponViewModel();
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
	UpdateWeaponViewModel();
}

void ABOCharacter::EquipSecondaryWeapon()
{
	if (!WeaponComponent)
	{
		return;
	}

	StopFire();
	WeaponComponent->EquipWeaponSlot(1);
	UpdateWeaponViewModel();
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

void ABOCharacter::UpdateWeaponViewModel()
{
	const int32 WeaponSlot = WeaponComponent ? WeaponComponent->GetCurrentWeaponSlot() : 0;
	if (WeaponSlot == 1)
	{
		ConfigureWeaponViewMesh(WeaponBodyMesh, FVector(34.0f, 16.0f, -18.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.28f, 0.075f, 0.065f));
		ConfigureWeaponViewMesh(WeaponBarrelMesh, FVector(57.0f, 16.0f, -17.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.20f, 0.032f, 0.032f));
		ConfigureWeaponViewMesh(WeaponGripMesh, FVector(27.0f, 16.0f, -27.0f), FRotator(0.0f, 0.0f, -12.0f), FVector(0.07f, 0.055f, 0.14f));
		ConfigureWeaponViewMesh(WeaponSightMesh, FVector(38.0f, 16.0f, -11.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.10f, 0.026f, 0.025f));
		return;
	}

	ConfigureWeaponViewMesh(WeaponBodyMesh, FVector(42.0f, 17.0f, -18.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.42f, 0.075f, 0.068f));
	ConfigureWeaponViewMesh(WeaponBarrelMesh, FVector(78.0f, 17.0f, -17.8f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.32f, 0.028f, 0.028f));
	ConfigureWeaponViewMesh(WeaponGripMesh, FVector(34.0f, 17.0f, -28.5f), FRotator(0.0f, 0.0f, -10.0f), FVector(0.085f, 0.055f, 0.16f));
	ConfigureWeaponViewMesh(WeaponSightMesh, FVector(46.0f, 17.0f, -11.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.16f, 0.026f, 0.024f));
}

void ABOCharacter::ConfigureWeaponViewMesh(UStaticMeshComponent* MeshComponent, const FVector& RelativeLocation, const FRotator& RelativeRotation, const FVector& RelativeScale) const
{
	if (!MeshComponent)
	{
		return;
	}

	MeshComponent->SetRelativeLocation(RelativeLocation);
	MeshComponent->SetRelativeRotation(RelativeRotation);
	MeshComponent->SetRelativeScale3D(RelativeScale);
	MeshComponent->SetVisibility(true, true);
}
