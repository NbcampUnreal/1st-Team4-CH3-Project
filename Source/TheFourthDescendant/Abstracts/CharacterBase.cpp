// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterBase.h"

// Sets default values
ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
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
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

