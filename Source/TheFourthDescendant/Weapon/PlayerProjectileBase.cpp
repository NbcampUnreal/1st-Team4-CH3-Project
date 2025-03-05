// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerProjectileBase.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "TrailEffect.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
APlayerProjectileBase::APlayerProjectileBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->CanCharacterStepUpOn = ECB_No;
	CollisionComponent->SetCollisionProfileName(UCollisionProfile::BlockAllDynamic_ProfileName);
	CollisionComponent->OnComponentHit.AddDynamic(this, &APlayerProjectileBase::OnHit);
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

void APlayerProjectileBase::LaunchProjectile(const FVector LaunchVelocity, const float Damage, const float ExplosionRadius)
{
	ProjectileMovementComponent->Velocity = LaunchVelocity;
	ProjectileDamage = Damage;
	ProjectileExplosionRadius = ExplosionRadius;
}

void APlayerProjectileBase::BeginPlay()
{
	Super::BeginPlay();

	if (TrailEffectClass)
	{
		TrailEffect = GetWorld()->SpawnActor<ATrailEffect>(TrailEffectClass);
		TrailEffect->AttachToComponent(MeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TEXT("TrailSocket"));
	}
}

void APlayerProjectileBase::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
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

	if (TrailEffect.IsValid())
	{
		TrailEffect->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		TrailEffect->StopAndDestroy(2.0f);
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

	UE_LOG(LogTemp, Display, TEXT("Destroy"));
	Destroy();
}
