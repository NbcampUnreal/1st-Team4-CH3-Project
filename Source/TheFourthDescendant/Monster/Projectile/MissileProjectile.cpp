#include "MissileProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AMissileProjectile::AMissileProjectile()
{
	// 스피어 콜리전 컴포넌트 초기화
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	// 총알 위로 걸을 수 없게
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	RootComponent = CollisionComp;
	
	// 스태틱 메시 컴포넌트 초기화
	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	MeshComp->SetupAttachment(CollisionComp);
	
	// ProjectileMovement 컴포넌트 초기 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	InitialLifeSpan = 4.0f;
}

void AMissileProjectile::FireMissileIntoTheSky(FVector& Direction)
{
	// 속도 설정
	FVector Velocity = Direction * FireSpeedIntoTheSky;

	// 해당 속도로 발사
	ProjectileMovement->Velocity = Velocity * ProjectileMovement->InitialSpeed;;
}


