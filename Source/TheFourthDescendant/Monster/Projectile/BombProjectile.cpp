#include "BombProjectile.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"

ABombProjectile::ABombProjectile()
{
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &ABombProjectile::OnOverlap);
}

void ABombProjectile::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//Super::OnOverlap(OverlappedComp, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	if (bIsDamaged) return;
	
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
