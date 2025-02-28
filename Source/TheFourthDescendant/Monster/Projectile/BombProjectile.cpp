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
		if (OtherActor->ActorHasTag("Player"))
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Damaged");

			// 플레이어에게 데미지 적용
			UGameplayStatics::ApplyDamage(OtherActor, ProjectileDamage, GetInstigatorController(), this, UDamageType::StaticClass());

			// 게임 인스턴스 캐스팅
			UGameInstance* GameInstance = GetWorld()->GetGameInstance();
			UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GameInstance);
			
			// 게임 인스턴스에 플레이어에게 가한 데미지 최신화
			MainGameInstance->AddReceivedDamageByEnemy(ProjectileDamage);

			// 회피한 횟수 1 감소
			MainGameInstance->SubtractEvasionAttackCount(1);

			bIsDamaged = true;
		}
	}
	
}
