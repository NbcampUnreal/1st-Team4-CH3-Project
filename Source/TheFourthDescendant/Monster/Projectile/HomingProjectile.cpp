#include "HomingProjectile.h"

#include "Components/SphereComponent.h"
#include "Engine/StaticMeshActor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"
#include "TheFourthDescendant/Player/PlayerCharacter.h"

AHomingProjectile::AHomingProjectile()
{

	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AHomingProjectile::OnOverlap);
	// 유도 기능 활성화
	ProjectileMovement->bIsHomingProjectile = true;

	// 유도 강도 설정
	ProjectileMovement->HomingAccelerationMagnitude = 1000.0f;
}

void AHomingProjectile::Fire(FVector& Direction)
{
	Super::Fire(Direction);

	GetWorldTimerManager().SetTimer(
		SetHomingTimer,
		this,
		&AHomingProjectile::SetHomingTarget,
		0.05f,
		false);
}

void AHomingProjectile::SetHomingTarget()
{
	
	// Player 위치에 임시 객체 생성
	AActor* Player = UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass());
	
	FVector PlayerLocation = Player->GetActorLocation();

	FVector TempLocation = PlayerLocation;
	TempLocation.Z -= 100;
	
	
	AStaticMeshActor* TempActor = GetWorld()->SpawnActor<AStaticMeshActor>(
			AStaticMeshActor::StaticClass(),
			TempLocation,
			FRotator::ZeroRotator
		);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Homing !");


	// 임시 객체 위치 추적
	ProjectileMovement->HomingTargetComponent = TempActor->GetRootComponent();
}

void AHomingProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
								int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != this)
	{
		if (OtherActor->ActorHasTag("Ground"))
		{
			// 폭발 파티클 스폰
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				ExplosionParticle,
				GetActorLocation());
			
			// 플레이어가 폭발 범위 내에 있는 지 확인
			TArray<AActor*> OverlappingActors;
			AttackRangeCollisionComp->GetOverlappingActors(OverlappingActors);

			for (AActor* Actor : OverlappingActors)
			{
				if (Actor && Actor->ActorHasTag("Player"))
				{
					UGameplayStatics::ApplyDamage(
						Actor,
						ProjectileDamage,
						nullptr,
						this,
						UDamageType::StaticClass()
						);
				}
			}

			Destroy();
		}

		
	}
}