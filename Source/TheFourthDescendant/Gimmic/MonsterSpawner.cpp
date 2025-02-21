// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterSpawner.h"

#include "TheFourthDescendant/Monster/Melee/MeleeMonster.h"
#include "TheFourthDescendant/Monster/Ranged/RangedMonster.h"

AMonsterSpawner::AMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	FirstWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Melee, 8));
    SecondWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, 4));
    ThirdWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Melee, 12));
    ThirdWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, 6));
}

// (근접, 원거리) 스폰 위치에 몬스터 생성
void AMonsterSpawner::Spawn(EMonsterType MonsterType)
{
	if (GetWorld())
	{
		FVector SpawnLocation = GetActorLocation(); 
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
        
		AMonster* NewMonster = nullptr;

		switch (MonsterType)
		{
		case EMonsterType::Melee:
			if (MeleeMonsterClass)
			{
				NewMonster = GetWorld()->SpawnActor<AMeleeMonster>(MeleeMonsterClass, SpawnLocation,FRotator::ZeroRotator, SpawnParams);
				if (NewMonster && GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Melee Monster Spawned"));
				}
				else if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to Spawn Melee Monster"));
				}
			}
			break;

		case EMonsterType::Ranged:
			if (RangedMonsterClass)
			{
				NewMonster = GetWorld()->SpawnActor<ARangedMonster>(RangedMonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			}
			break;

		case EMonsterType::Boss:
			if (false)
			{
				NewMonster = GetWorld()->SpawnActor<ARangedMonster>(nullptr, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			}
			break;
			
		}
		
		if (NewMonster)
		{
			// 추가 설정 로직이 필요한 경우 여기에 작성합니다.
		}
	}
}

// 몬스터 스폰 지점에 한 번에 생성
void AMonsterSpawner::SpawnMonsters(TArray<TTuple<EMonsterType, int32>> SpawnData)
{
	for (const TTuple<EMonsterType, int32>& Data : SpawnData)
	{
		EMonsterType MonsterType = Data.Key;
		int32 Count = Data.Value;

		for (int32 i = 0; i < Count; ++i)
		{
			Spawn(MonsterType);
		}
	}
}

// 몬스터 랜덤 지점에 한 번에 생성
void AMonsterSpawner::RandomSpawnMonsters(TArray<TTuple<EMonsterType, int32>> SpawnData)
{
	for (const TTuple<EMonsterType, int32>& Data : SpawnData)
	{
		EMonsterType MonsterType = Data.Key;
		int32 Count = Data.Value;

		for (int32 i = 0; i < Count; ++i)
		{
			RandomSpawn(MonsterType);
		}
	}
}

// (근접, 원거리) 몬스터 랜덤 위치 생성
void AMonsterSpawner::RandomSpawn(EMonsterType MonsterType)
{
	if (GetWorld())
	{
		FVector SpawnLocation = GetRandomSpawnPoint(); 
		FActorSpawnParameters SpawnParams;
        
		AMonster* NewMonster = nullptr;

		switch (MonsterType)
		{
		case EMonsterType::Melee:
			if (MeleeMonsterClass)
			{
				NewMonster = GetWorld()->SpawnActor<AMeleeMonster>(MeleeMonsterClass, SpawnLocation,FRotator::ZeroRotator, SpawnParams);
			}
			break;

		case EMonsterType::Ranged:
			if (RangedMonsterClass)
			{
				NewMonster = GetWorld()->SpawnActor<ARangedMonster>(RangedMonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			}
			break;

		case EMonsterType::Boss:
			if (false)
			{
				NewMonster = GetWorld()->SpawnActor<ARangedMonster>(nullptr, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			}
			break;
			
		}
		
		if (NewMonster)
		{
			// 추가 설정 로직이 필요한 경우 여기에 작성합니다.
		}
	}
}

FVector AMonsterSpawner::GetSpawnPoint()
{
	return GetActorLocation();
}

FVector AMonsterSpawner::GetRandomSpawnPoint()
{
	return FVector::ZeroVector;
	// tarray fvector uproperty makeedit
}

void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnMonsters(FirstWaveSpawnData);

	Spawn(EMonsterType::Melee);

	// 디버깅 로그 출력
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("BeginPlay Called"));
	}

	// 클래스 변수 확인 로그
	if (MeleeMonsterClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("MeleeMonsterClass is set"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MeleeMonsterClass is NOT set"));
	}

	if (RangedMonsterClass)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("RangedMonsterClass is set"));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RangedMonsterClass is NOT set"));
	}
}


