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
		MeshComponent->SetHiddenInGame(false);
		MeshComponent->SetRenderInMainPass(true);
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

	EnsureWeaponViewMeshes();
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
	AddControllerPitchInput(-LookAxisVector.Y);
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
	EnsureWeaponViewMeshes();

	const int32 WeaponSlot = WeaponComponent ? WeaponComponent->GetCurrentWeaponSlot() : 0;
	if (WeaponSlot == 1)
	{
		ConfigureWeaponViewMesh(WeaponBodyMesh, FVector(42.0f, 9.0f, -14.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.34f, 0.11f, 0.09f));
		ConfigureWeaponViewMesh(WeaponBarrelMesh, FVector(70.0f, 9.0f, -13.2f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.26f, 0.04f, 0.04f));
		ConfigureWeaponViewMesh(WeaponGripMesh, FVector(34.0f, 9.0f, -27.0f), FRotator(0.0f, 0.0f, -12.0f), FVector(0.09f, 0.07f, 0.17f));
		ConfigureWeaponViewMesh(WeaponSightMesh, FVector(47.0f, 9.0f, -6.0f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.13f, 0.04f, 0.035f));
		return;
	}

	ConfigureWeaponViewMesh(WeaponBodyMesh, FVector(52.0f, 10.0f, -14.5f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.52f, 0.12f, 0.10f));
	ConfigureWeaponViewMesh(WeaponBarrelMesh, FVector(98.0f, 10.0f, -13.6f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.44f, 0.045f, 0.045f));
	ConfigureWeaponViewMesh(WeaponGripMesh, FVector(41.0f, 10.0f, -29.0f), FRotator(0.0f, 0.0f, -10.0f), FVector(0.11f, 0.075f, 0.19f));
	ConfigureWeaponViewMesh(WeaponSightMesh, FVector(60.0f, 10.0f, -5.8f), FRotator(0.0f, 0.0f, 0.0f), FVector(0.20f, 0.045f, 0.035f));
}

void ABOCharacter::EnsureWeaponViewMeshes()
{
	UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube"));
	UStaticMeshComponent* WeaponMeshes[] = { WeaponBodyMesh, WeaponBarrelMesh, WeaponGripMesh, WeaponSightMesh };
	for (UStaticMeshComponent* MeshComponent : WeaponMeshes)
	{
		if (!MeshComponent)
		{
			continue;
		}

		if (!MeshComponent->GetStaticMesh() && CubeMesh)
		{
			MeshComponent->SetStaticMesh(CubeMesh);
		}

		MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComponent->SetGenerateOverlapEvents(false);
		MeshComponent->SetCastShadow(false);
		MeshComponent->SetReceivesDecals(false);
		MeshComponent->SetRenderInMainPass(true);
		MeshComponent->SetHiddenInGame(false);
		MeshComponent->SetVisibility(true, true);
		MeshComponent->MarkRenderStateDirty();
	}
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
	MeshComponent->SetHiddenInGame(false);
	MeshComponent->SetVisibility(true, true);
	MeshComponent->MarkRenderStateDirty();
}
