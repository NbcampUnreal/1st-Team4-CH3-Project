#include "SpawnVolume.h"
#include "Components/BoxComponent.h"
#include "TheFourthDescendant/Monster/Monster.h"

ASpawnVolume::ASpawnVolume()
{
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene);

	SpawningBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SpawningBox"));
	SpawningBox->SetupAttachment(Scene);
}

void ASpawnVolume::SpawnEnemy()
{
	// 스폰할 위치 선정
	FVector SpawnLocation = GetRandomPointInVolume();

	// 몬스터 스폰
	GetWorld()->SpawnActor<AMonster>(EnemyClass, SpawnLocation, FRotator(0, 0, 0));
}

FVector ASpawnVolume::GetRandomPointInVolume() const
{
	// 박스 콜리전의 반지름 값을 가져옴
	FVector BoxExtent = SpawningBox->GetScaledBoxExtent();

	// 박스 콜리전의 중심 좌표를 가져옴
	FVector BoxOrigin = SpawningBox->GetComponentLocation();

	return BoxOrigin + FVector(
		FMath::FRandRange(-BoxExtent.X, BoxExtent.X),
		FMath::FRandRange(-BoxExtent.Y, BoxExtent.Y),
		FMath::FRandRange(-BoxExtent.Z ,BoxExtent.Z)
		);
}



