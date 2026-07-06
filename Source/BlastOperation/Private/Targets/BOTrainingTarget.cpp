#include "Targets/BOTrainingTarget.h"

#include "Components/BOHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Net/UnrealNetwork.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"

ABOTrainingTarget::ABOTrainingTarget()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	MeshComponent->SetRelativeScale3D(FVector(0.6f, 0.12f, 1.4f));

	HealthComponent = CreateDefaultSubobject<UBOHealthComponent>(TEXT("HealthComponent"));
	ResetDelay = 1.0f;
	DamageFlashDuration = 0.12f;
	DamageFlashScaleBoost = 0.1f;
	bTargetActive = true;
	DamageFlashEndTime = 0.0f;
	DamageFlashStrength = 0.0f;
}

void ABOTrainingTarget::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ApplyDefaultVisualSetup();
	DefaultMeshScale = MeshComponent ? MeshComponent->GetRelativeScale3D() : FVector::OneVector;
	ApplyTargetActiveState();
}

void ABOTrainingTarget::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!MeshComponent || !bTargetActive)
	{
		SetActorTickEnabled(false);
		return;
	}

	const UWorld* World = GetWorld();
	if (!World || DamageFlashDuration <= 0.0f)
	{
		MeshComponent->SetRelativeScale3D(DefaultMeshScale);
		SetActorTickEnabled(false);
		return;
	}

	const float Remaining = DamageFlashEndTime - World->GetTimeSeconds();
	if (Remaining <= 0.0f)
	{
		MeshComponent->SetRelativeScale3D(DefaultMeshScale);
		SetActorTickEnabled(false);
		return;
	}

	const float Alpha = FMath::Clamp(Remaining / DamageFlashDuration, 0.0f, 1.0f);
	const float ScaleMultiplier = 1.0f + DamageFlashScaleBoost * DamageFlashStrength * Alpha;
	MeshComponent->SetRelativeScale3D(DefaultMeshScale * ScaleMultiplier);
}

void ABOTrainingTarget::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABOTrainingTarget, bTargetActive);
}

void ABOTrainingTarget::BeginPlay()
{
	Super::BeginPlay();

	ApplyDefaultVisualSetup();
	DefaultMeshScale = MeshComponent ? MeshComponent->GetRelativeScale3D() : FVector::OneVector;

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ABOTrainingTarget::HandleDeath);
		HealthComponent->OnHealthChanged.AddDynamic(this, &ABOTrainingTarget::HandleHealthChanged);
	}

	ApplyTargetActiveState();
}

void ABOTrainingTarget::ApplyDefaultVisualSetup()
{
	if (!MeshComponent)
	{
		return;
	}

	if (!MeshComponent->GetStaticMesh())
	{
		if (UStaticMesh* CubeMesh = LoadObject<UStaticMesh>(nullptr, TEXT("/Engine/BasicShapes/Cube.Cube")))
		{
			MeshComponent->SetStaticMesh(CubeMesh);
		}
	}

	if (UPhysicalMaterial* TargetPhysicalMaterial = LoadObject<UPhysicalMaterial>(nullptr, TEXT("/Game/BlastOperation/Materials/Physics/PM_BO_TrainingTarget.PM_BO_TrainingTarget")))
	{
		MeshComponent->SetPhysMaterialOverride(TargetPhysicalMaterial);
	}
}

void ABOTrainingTarget::HandleDeath(AActor* DeadActor)
{
	if (!HasAuthority())
	{
		return;
	}

	FTimerHandle DeactivateTimerHandle;
	GetWorldTimerManager().SetTimer(DeactivateTimerHandle, this, &ABOTrainingTarget::DeactivateTarget, DamageFlashDuration, false);

	FTimerHandle ResetTimerHandle;
	GetWorldTimerManager().SetTimer(ResetTimerHandle, this, &ABOTrainingTarget::ResetTarget, ResetDelay, false);
}

void ABOTrainingTarget::HandleHealthChanged(float NewHealth, float Delta)
{
	if (Delta < -KINDA_SMALL_NUMBER)
	{
		const float DamageAmount = FMath::Abs(Delta);
		if (HasAuthority())
		{
			MulticastPlayDamageFeedback(DamageAmount);
		}
		else
		{
			PlayDamageFeedback(DamageAmount);
		}
	}
}

void ABOTrainingTarget::OnRep_TargetActive()
{
	ApplyTargetActiveState();
}

void ABOTrainingTarget::DeactivateTarget()
{
	if (!HasAuthority())
	{
		return;
	}

	bTargetActive = false;
	ApplyTargetActiveState();
}

void ABOTrainingTarget::MulticastPlayDamageFeedback_Implementation(float DamageAmount)
{
	PlayDamageFeedback(DamageAmount);
}

void ABOTrainingTarget::ResetTarget()
{
	if (!HasAuthority())
	{
		return;
	}

	if (HealthComponent)
	{
		HealthComponent->ResetHealth();
	}

	bTargetActive = true;
	ApplyTargetActiveState();
}

void ABOTrainingTarget::ApplyTargetActiveState()
{
	if (!MeshComponent)
	{
		return;
	}

	if (DefaultMeshScale.IsNearlyZero())
	{
		DefaultMeshScale = MeshComponent->GetRelativeScale3D();
	}

	MeshComponent->SetVisibility(bTargetActive, true);
	MeshComponent->SetCollisionEnabled(bTargetActive ? ECollisionEnabled::QueryAndPhysics : ECollisionEnabled::NoCollision);
	MeshComponent->SetRelativeScale3D(DefaultMeshScale);
	DamageFlashEndTime = 0.0f;
	DamageFlashStrength = 0.0f;
	SetActorTickEnabled(false);
}

void ABOTrainingTarget::PlayDamageFeedback(float DamageAmount)
{
	if (!MeshComponent || !bTargetActive)
	{
		return;
	}

	const UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	DamageFlashStrength = FMath::Clamp(DamageAmount / 60.0f, 0.35f, 1.0f);
	DamageFlashEndTime = World->GetTimeSeconds() + DamageFlashDuration;
	SetActorTickEnabled(true);
}
