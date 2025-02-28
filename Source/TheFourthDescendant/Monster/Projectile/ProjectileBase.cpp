#include "ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AProjectileBase::AProjectileBase()
{
	// 스피어 콜리전 컴포넌트 초기화
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(100.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	// 총알 위로 걸을 수 없게
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;
	RootComponent = CollisionComp;

	// 공격 반경 콜리전 컴포넌트 초기화
	AttackRangeCollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeSphereComp"));
	AttackRangeCollisionComp->InitSphereRadius(500.0f);
	AttackRangeCollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	AttackRangeCollisionComp->CanCharacterStepUpOn = ECB_No;
	AttackRangeCollisionComp->SetupAttachment(CollisionComp);
	
	// 스태틱 메시 컴포넌트 초기화
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	MeshComp->SetupAttachment(CollisionComp);

	// 스켈레탈 메시 컴포넌트 초기화
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(CollisionComp);
	
	// ProjectileMovement 컴포넌트 초기 설정
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	InitialLifeSpan = 3.0f;
}

void AProjectileBase::Fire(FVector& Direction)
{
	// 속도 설정
	ProjectileMovement->InitialSpeed = FireSpeed;
	ProjectileMovement->MaxSpeed = FireSpeed;

	// 해당 속도로 발사
	ProjectileMovement->Velocity = Direction * ProjectileMovement->InitialSpeed;
}


void AProjectileBase::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "P Damaged");

}
