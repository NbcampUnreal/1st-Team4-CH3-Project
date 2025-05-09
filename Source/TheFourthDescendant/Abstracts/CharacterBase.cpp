// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"

FStatus::FStatus()
{
	MaxHealth = 0;
	Health = MaxHealth;
	MaxShield = 0;
	Shield = MaxShield;
	Defense = 0.0f;
	WalkSpeed = 0.0f;
}

// Sets default values
ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	Status = FStatus();
}

int32 ACharacterBase::GetHealth() const
{
	return Status.Health;
}

int32 ACharacterBase::GetMaxHealth() const
{
	return Status.MaxHealth;
}

int32 ACharacterBase::GetShield() const
{
	return Status.Shield;
}

int32 ACharacterBase::GetMaxShield() const
{
	return Status.MaxShield;
}

float ACharacterBase::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	Status.Shield -= ActualDamage;
	if (Status.Shield < 0)
	{
		Status.Health += Status.Shield;
		Status.Shield = 0;
	}

	DamageDispatcher.Broadcast(Status.Health / static_cast<float>(Status.MaxHealth),
								Status.Shield / static_cast<float>(Status.MaxShield),
								ActualDamage);
	
	return ActualDamage;
}

