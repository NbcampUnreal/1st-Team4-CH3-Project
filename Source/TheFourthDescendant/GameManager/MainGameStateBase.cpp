#include "MainGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/Monster/Monster.h"

AMainGameStateBase::AMainGameStateBase()
{
	CurrentLevelIndex = 0;
	TotalWaveCount = 0;
	TempWaveIndex = 0;
}

void AMainGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	StartLevel();
}

void AMainGameStateBase::StartLevel()
{
	// 모든 레벨을 진행한 경우 return
	if (CurrentLevelIndex >= LevelWaveInfo.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Level OverFlow");
		return;
	}

	// @To-do Wave에 맞는 몬스터 스폰 로직
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Level %d Wave %d Started !"), CurrentLevelIndex + 1, TempWaveIndex + 1));
	
	// 스폰된 Enemy 수 반환
	TArray<AActor*> FoundMonsters;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AMonster::StaticClass(), FoundMonsters);
	SpawnedEnemyCount = FoundMonsters.Num();
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("EnemySpawned %d num!"), SpawnedEnemyCount));

	// 델리게이트 등록 (예: 레벨 시작 시 특정 함수 호출)
	if (LevelEnded.IsValidIndex(CurrentLevelIndex))
	{
		LevelEnded[CurrentLevelIndex].AddLambda([]()
		{
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Level Ended !")));
		});
	}
}



void AMainGameStateBase::OnEnemyKilled()
{
	// 스폰된 몬스터 수 감소
	--SpawnedEnemyCount;
	
	// 몬스터를 모두 처치하지 못 한 경우 return
	if (SpawnedEnemyCount > 0) return;

	// 다음 웨이브 혹은 레벨로 전환
	SetNextLevelWaveIndex();
}


void AMainGameStateBase::SetNextLevelWaveIndex()
{
	++TotalWaveCount;
	
	// 현재 레벨의 모든 웨이브가 진행된 경우
	if (TempWaveIndex >= LevelWaveInfo[CurrentLevelIndex] - 1)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Level Changed !")));
		
		// @To-do 레벨 델리게이트 함수
		if (LevelEnded.IsValidIndex(CurrentLevelIndex))
		{
			LevelEnded[CurrentLevelIndex].Broadcast();
		}
		
		++CurrentLevelIndex;
		TempWaveIndex = 0;
	}
	// 현재 레벨의 웨이브가 남은 경우
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Next Wave !")));
		++TempWaveIndex;
	}
}