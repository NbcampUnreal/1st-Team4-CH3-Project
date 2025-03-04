#include "MainGameStateBase.h"

#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/Gimmic/MonsterSpawner.h"
#include "TheFourthDescendant/Monster/Monster.h"

AMainGameStateBase::AMainGameStateBase()
{
	CurrentLevelIndex = 0;
	TotalWaveCount = 0;
	TempWaveIndex = 0;
	bIsBossLevel = false;
}

void AMainGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	FString LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld());
	GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("Current Level : %s"), *LevelName));

	if (LevelName.Equals("Boss"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Boss Level !");
		bIsBossLevel = true;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Normal Level !");
		bIsBossLevel = false;
		GetMonsterSpawner();
		LevelEnded.AddLambda([this]()
		{
			OnNormalLevelEnded();
		});
	}
}

void AMainGameStateBase::StartLevel()
{
	// 모든 레벨을 진행한 경우 return
	if (CurrentLevelIndex >= LevelWaveInfo.Num())
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, "Level OverFlow");
		return;
	}

	// 웨이브 전환 간격이 지난 후 몬스터 스폰
	GetWorldTimerManager().SetTimer(
		SpawnerTimer,
		this,
		&AMainGameStateBase::EnemySpawn,
		WaveInterval,
		false);
	
}




void AMainGameStateBase::EnemySpawn()
{
	// @To-do Wave에 맞는 몬스터 스폰 로직
	Spawner->SpawnMonsters(TotalWaveCount);
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Level %d Wave %d Started !"), CurrentLevelIndex + 1, TempWaveIndex + 1));
}


void AMainGameStateBase::OnEnemyKilled()
{
	// 보스 맵이면 return
	if (bIsBossLevel) return;
	
	// 스폰된 몬스터 수 감소
	--SpawnedEnemyCount;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("EnemyKilled ! Current : %d num!"), SpawnedEnemyCount));

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
		
		LevelEnded.Broadcast();
		
		++CurrentLevelIndex;
		TempWaveIndex = 0;
	}
	// 현재 레벨의 웨이브가 남은 경우
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Next Wave !")));
		++TempWaveIndex;
		StartLevel();
	}
}



void AMainGameStateBase::OnNormalLevelEnded()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Change To Boss !")));
	
	GetWorldTimerManager().SetTimer(
		LevelLoadTimer,
		[this]()
		{
			UGameplayStatics::OpenLevel(GetWorld(), LevelNames[1]);
		},
		LevelInterval[0],
		false
		);
}



void AMainGameStateBase::OnBossLevelEnded()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Change To Title !")));
	
	GetWorldTimerManager().SetTimer(
		LevelLoadTimer,
		[this]()
		{
			UGameplayStatics::OpenLevel(GetWorld(), LevelNames[0]);
		},
		LevelInterval[1],
		false
		);
}



void AMainGameStateBase::OnEnemySpawned()
{
	++SpawnedEnemyCount;
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("EnemySpawned %d num!"), SpawnedEnemyCount));
}


void AMainGameStateBase::GetMonsterSpawner()
{
	// 월드에서 AMonsterSpawner 클래스를 가진 액터를 찾아서 Spawner에 저장
	AActor* FoundSpawner = UGameplayStatics::GetActorOfClass(GetWorld(), AMonsterSpawner::StaticClass());
	if (FoundSpawner)
	{
		Spawner = Cast<AMonsterSpawner>(FoundSpawner);
		StartLevel();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("MonsterSpawner not found!"));
	}
}
