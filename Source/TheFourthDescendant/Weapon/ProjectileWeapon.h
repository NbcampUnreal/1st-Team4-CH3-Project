// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponBase.h"
#include "ProjectileWeapon.generated.h"

class AProjectileBase;

UCLASS()
class THEFOURTHDESCENDANT_API AProjectileWeapon : public AWeaponBase
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Weapon|Projectile")
	TSubclassOf<class AProjectileBase> ProjectileClass;
	UPROPERTY(EditAnywhere, Category = "Weapon|Projectile")
	float AttackRange;
	UPROPERTY(EditAnywhere, Category="Weapon|Projectile")
	float ProjectileSpeed;
	UPROPERTY(EditAnywhere, Category="Weapon|Projectile")
	float ExplosionRadius;
public:
	// Sets default values for this actor's properties
	AProjectileWeapon();
	virtual FRotator GetAimRotation(const APawn* TargetPawn) const override;

protected:
	virtual FShootResult PerformAttack() override;
};
