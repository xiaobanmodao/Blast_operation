#include "Components/BOHealthComponent.h"

#include "Core/BOLogChannels.h"
#include "Net/UnrealNetwork.h"

UBOHealthComponent::UBOHealthComponent()
{
	SetIsReplicatedByDefault(true);

	MaxHealth = 100.0f;
	Armor = 0.0f;
	Health = MaxHealth;
}

void UBOHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ResetHealth();
		GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UBOHealthComponent::HandleOwnerDamage);
	}
}

void UBOHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UBOHealthComponent, MaxHealth);
	DOREPLIFETIME(UBOHealthComponent, Armor);
	DOREPLIFETIME(UBOHealthComponent, Health);
}

void UBOHealthComponent::ResetHealth()
{
	if (!GetOwner() || !GetOwner()->HasAuthority())
	{
		return;
	}

	const float OldHealth = Health;
	Health = MaxHealth;
	OnHealthChanged.Broadcast(Health, Health - OldHealth);
}

void UBOHealthComponent::HandleOwnerDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (!DamagedActor || Damage <= 0.0f || Health <= 0.0f)
	{
		return;
	}

	const float OldHealth = Health;
	const float ArmorMitigation = FMath::Clamp(Armor * 0.35f, 0.0f, Damage * 0.5f);
	const float FinalDamage = FMath::Max(0.0f, Damage - ArmorMitigation);
	Health = FMath::Clamp(Health - FinalDamage, 0.0f, MaxHealth);

	UE_LOG(LogBlastOperation, Verbose, TEXT("%s took %.1f damage from %s. Health %.1f -> %.1f"),
		*GetNameSafe(DamagedActor),
		FinalDamage,
		*GetNameSafe(DamageCauser),
		OldHealth,
		Health);

	OnHealthChanged.Broadcast(Health, Health - OldHealth);

	if (Health <= 0.0f)
	{
		OnDeath.Broadcast(DamagedActor);
	}
}

void UBOHealthComponent::OnRep_Health(float OldHealth)
{
	OnHealthChanged.Broadcast(Health, Health - OldHealth);

	if (OldHealth > 0.0f && Health <= 0.0f)
	{
		OnDeath.Broadcast(GetOwner());
	}
}

