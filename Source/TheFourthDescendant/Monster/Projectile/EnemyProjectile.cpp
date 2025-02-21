// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/Monster/Ranged/RangedMonster.h"


AEnemyProjectile::AEnemyProjectile()
{
	// 스피어 콜리전 컴포넌트 초기화
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AEnemyProjectile::OnOverlap);

	// 총알 위로 걸을 수 없게
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	RootComponent = CollisionComp;

	// 스태틱 메시 컴포넌트 초기화
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComp->SetupAttachment(CollisionComp);
	
	// ProjectileMovement 컴포넌트 초기 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	InitialLifeSpan = 2.0f;
}


void AEnemyProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, 
								 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, 
								 bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (OtherActor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(OtherActor, ProjectileDamage, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}
    
	// 총알 비활성화
	SetActorEnableCollision(false);
	MeshComp->SetVisibility(false, true);
	CollisionComp->SetVisibility(false, true);
}

void AEnemyProjectile::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection * ProjectileMovement->InitialSpeed;
}



