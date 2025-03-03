// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class UNiagaraSystem;

UCLASS()
class THEFOURTHDESCENDANT_API AProjectileBase : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AProjectileBase();

	void LaunchProjectile(const FVector LaunchVelocity, float Damage, float ExplosionRadius);
	
protected:
	/** 충돌 발생 시의 이벤트 */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** 투사체 공격력*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Projectile")
	float ProjectileDamage;
	/** 투사체 폭발 범위*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Projectile")
	float ProjectileExplosionRadius;
	/** 투사체 폭발 Vfx */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Projectile")
	class UNiagaraSystem* ExplosionVfx;
	/** 투사체 폭발 Sfx */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Projectile")
	class USoundBase* ExplosionSfx;

private:
	UPROPERTY(VisibleDefaultsOnly, Category="Projectile")
	class USphereComponent* CollisionComponent;
	UPROPERTY(VisibleDefaultsOnly, Category="Projectile")
	class UStaticMeshComponent* MeshComponent;
	UPROPERTY(VisibleDefaultsOnly, Category="Projectile")
	class UProjectileMovementComponent* ProjectileMovementComponent;
};
