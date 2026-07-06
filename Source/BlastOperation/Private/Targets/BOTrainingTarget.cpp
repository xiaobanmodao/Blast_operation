#include "Targets/BOTrainingTarget.h"

#include "Components/BOHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "TimerManager.h"
#include "UObject/ConstructorHelpers.h"

ABOTrainingTarget::ABOTrainingTarget()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	SetRootComponent(MeshComponent);
	MeshComponent->SetCollisionProfileName(TEXT("BlockAllDynamic"));

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (CubeMesh.Succeeded())
	{
		MeshComponent->SetStaticMesh(CubeMesh.Object);
		MeshComponent->SetRelativeScale3D(FVector(0.6f, 0.12f, 1.4f));
	}

	HealthComponent = CreateDefaultSubobject<UBOHealthComponent>(TEXT("HealthComponent"));
	ResetDelay = 1.0f;
}

void ABOTrainingTarget::BeginPlay()
{
	Super::BeginPlay();

	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddDynamic(this, &ABOTrainingTarget::HandleDeath);
	}
}

void ABOTrainingTarget::HandleDeath(AActor* DeadActor)
{
	if (!HasAuthority())
	{
		return;
	}

	MeshComponent->SetVisibility(false, true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	FTimerHandle ResetTimerHandle;
	GetWorldTimerManager().SetTimer(ResetTimerHandle, this, &ABOTrainingTarget::ResetTarget, ResetDelay, false);
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

	MeshComponent->SetVisibility(true, true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

