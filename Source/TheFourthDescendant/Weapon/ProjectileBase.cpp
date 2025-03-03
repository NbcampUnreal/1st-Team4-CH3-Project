// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBase.h"

#include "NiagaraFunctionLibrary.h"
#include "Chaos/Deformable/ChaosDeformableCollisionsProxy.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
AProjectileBase::AProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);
	SetRootComponent(CollisionComponent);

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);
	MeshComponent->CanCharacterStepUpOn = ECB_No;
	MeshComponent->SetGenerateOverlapEvents(false);
	MeshComponent->SetupAttachment(CollisionComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->UpdatedComponent = CollisionComponent;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

	ProjectileDamage = 0.0f;
	ProjectileExplosionRadius = 0.0f;
}

void AProjectileBase::LaunchProjectile(const FVector LaunchVelocity, const float Damage, const float ExplosionRadius)
{
	ProjectileMovementComponent->Velocity = LaunchVelocity;
	ProjectileDamage = Damage;
	ProjectileExplosionRadius = ExplosionRadius;
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            FVector NormalImpulse, const FHitResult& Hit)
{
	if (ExplosionVfx)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
					GetWorld(),
					ExplosionVfx,
					GetActorLocation(),
					FRotator::ZeroRotator
		);
	}

	if (ExplosionSfx)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSfx,
			GetActorLocation(),
			FRotator::ZeroRotator
		);
	}

	UGameplayStatics::ApplyRadialDamage(
			GetOwner(),
			ProjectileDamage,
			GetActorLocation(),
			ProjectileExplosionRadius,
			UDamageType::StaticClass(),
			TArray<AActor*>({GetOwner(), this}), // 자기 자신이 Ignore 되지 않으면 제대로 데미지 발생이 일어나지 않는다.
			GetOwner(),
			GetInstigatorController(),
			true
	);
	
	Destroy();
}