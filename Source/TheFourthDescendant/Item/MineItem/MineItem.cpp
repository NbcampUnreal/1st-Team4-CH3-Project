#include "MineItem.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"

AMineItem::AMineItem()
{
	ExplosionDelay = 5.0f;
	ExplosionDamage = 30.0f;
	ExplosionRadius = 300.0f;
	ItemType = "Mine";
	bHasExploded = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("ExplosionRadius"));
	Collision->InitSphereRadius(ExplosionRadius);
	Collision->SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	Collision->SetupAttachment(Scene);
}

void AMineItem::BeginPlay()
{
	Super::BeginPlay();

	// 폭발 타이머 활성화
	GetWorld()->GetTimerManager().SetTimer(ExplosionTimerHandle, this, &AMineItem::Explode, ExplosionDelay, false);
}



void AMineItem::Explode()
{
	UParticleSystemComponent* Particle = nullptr;

	if (ExplosionParticle)
	{
		Particle = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionParticle,
			GetActorLocation(),
			GetActorRotation(),
			false);
	}

	// 플레이어가 폭발 범위 내에 있는 지 확인
	TArray<AActor*> OverlappingActors;
	Collision->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		// 게임 인스턴스 캐스팅
		UGameInstance* GameInstance = GetWorld()->GetGameInstance();
		UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GameInstance);

		// 받은 데미지 증가
		MainGameInstance->AddReceivedDamageByEnemy(ExplosionDamage);
		
		if (Actor && Actor->ActorHasTag("Player"))
		{
			UGameplayStatics::ApplyDamage(
				Actor,
				ExplosionDamage,
				nullptr,
				this,
				UDamageType::StaticClass()
				);
		}
	}
	
	if (Particle)
	{
		FTimerHandle DestroyParticleTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(DestroyParticleTimerHandle,
			[Particle]()
			{
				Particle->DestroyComponent();
			},
			2.0f,
			false);
	}

	UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation());

	DestroyItem();

}
