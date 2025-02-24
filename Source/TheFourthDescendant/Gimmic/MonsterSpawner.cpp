// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterSpawner.h"

#include "TheFourthDescendant/Monster/Melee/MeleeMonster.h"
#include "TheFourthDescendant/Monster/Ranged/RangedMonster.h"

AMonsterSpawner::AMonsterSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	FirstWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, 8));
	
    SecondWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, 8));
	
    ThirdWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Melee, 8));
    ThirdWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, 8));
}

// (근접, 원거리) 스폰 위치에 몬스터 생성	
void AMonsterSpawner::Spawn(EMonsterType MonsterType, const FTransform& SpawnTransform)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("Spawn function called for %s"), *UEnum::GetValueAsString(MonsterType)));
    }

    if (GetWorld())
    {
        FActorSpawnParameters SpawnParams;
        SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

        AMonster* NewMonster = nullptr;

        switch (MonsterType)
        {
            case EMonsterType::Melee:
                if (MeleeMonsterClass)
                {
                    NewMonster = GetWorld()->SpawnActor<AMeleeMonster>(MeleeMonsterClass, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnParams);
                    if (NewMonster && GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Melee Monster Spawned"));
                    }
                    else if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to Spawn Melee Monster"));
                    }
                }
                else if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("MeleeMonsterClass is null"));
                }
                break;

            case EMonsterType::Ranged:
                if (RangedMonsterClass)
                {
                    NewMonster = GetWorld()->SpawnActor<ARangedMonster>(RangedMonsterClass, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnParams);
                    if (NewMonster && GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Ranged Monster Spawned"));
                    }
                    else if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Failed to Spawn Ranged Monster"));
                    }
                }
                else if (GEngine)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("RangedMonsterClass is null"));
                }
                break;

            case EMonsterType::Boss:
                // Boss 몬스터 생성 로직이 있는 경우 여기에 추가
                break;
        }

        if (NewMonster && GEngine)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Spawn function finished"));
        }
    }
    else if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GetWorld() returned NULL"));
    }
}






// (근접, 원거리) 몬스터 랜덤 위치 생성
void AMonsterSpawner::RandomSpawn(EMonsterType MonsterType)
{
	if (GetWorld())
	{
		FVector SpawnLocation = GetRandomSpawnPoint();
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		AMonster* NewMonster = nullptr;

		switch (MonsterType)
		{
		case EMonsterType::Melee:
			if (MeleeMonsterClass)
			{
				NewMonster = GetWorld()->SpawnActor<AMeleeMonster>(MeleeMonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			}
			break;

		case EMonsterType::Ranged:
			if (RangedMonsterClass)
			{
				NewMonster = GetWorld()->SpawnActor<ARangedMonster>(RangedMonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
			}
			break;

		case EMonsterType::Boss:
			// Boss 몬스터 생성 로직이 있는 경우 여기에 추가
				break;
		}

		if (NewMonster)
		{
			// 추가 설정 로직이 필요한 경우 여기에 작성합니다.
		}
	}
}


void AMonsterSpawner::SpawnMonsters(int32 LevelIndex)
{
    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("SpawnMonsters called with LevelIndex: %d"), LevelIndex));
    }

    TArray<TTuple<EMonsterType, int32>>* SpawnDataPtr = nullptr;
    TArray<FTransform>* MeleeTransformArrayPtr = nullptr;
    TArray<FTransform>* RangedTransformArrayPtr = nullptr;

    switch (LevelIndex)
    {
        case 0:
            SpawnDataPtr = &FirstWaveSpawnData;
            MeleeTransformArrayPtr = &FirstWaveTransformArray;
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Using FirstWaveSpawnData and FirstWaveTransformArray"));
            }
            break;
        case 1:
            SpawnDataPtr = &SecondWaveSpawnData;
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Using SecondWaveSpawnData and random spawn"));
            }
            break;
        case 2:
            SpawnDataPtr = &ThirdWaveSpawnData;
            MeleeTransformArrayPtr = &ThirdWaveMeleeTransformArray;
            RangedTransformArrayPtr = &ThirdWaveRangedTransformArray;
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("Using ThirdWaveSpawnData, ThirdWaveMeleeTransformArray, and ThirdWaveRangedTransformArray"));
            }
            break;
        default:
            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid LevelIndex"));
            }
            return;
    }

    if (SpawnDataPtr)
    {
        TArray<TTuple<EMonsterType, int32>>& SpawnData = *SpawnDataPtr;

        for (const TTuple<EMonsterType, int32>& Data : SpawnData)
        {
            EMonsterType MonsterType = Data.Key;
            int32 Count = Data.Value;

            if (GEngine)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, FString::Printf(TEXT("MonsterType: %s, Count: %d"), *UEnum::GetValueAsString(MonsterType), Count));
            }

            for (int32 i = 0; i < Count; ++i)
            {
                if (LevelIndex == 1)
                {
                    RandomSpawn(MonsterType);
                }
                else if (MonsterType == EMonsterType::Melee && MeleeTransformArrayPtr)
                {
                    TArray<FTransform>& MeleeTransformArray = *MeleeTransformArrayPtr;

                    if (i < MeleeTransformArray.Num())
                    {
                        const FTransform& SpawnTransform = MeleeTransformArray[i];
                        Spawn(MonsterType, SpawnTransform);
                    }
                    else
                    {
                        if (GEngine)
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough spawn points in MeleeTransformArray"));
                        }
                        return;
                    }
                }
                else if (MonsterType == EMonsterType::Ranged && RangedTransformArrayPtr)
                {
                    TArray<FTransform>& RangedTransformArray = *RangedTransformArrayPtr;

                    if (i < RangedTransformArray.Num())
                    {
                        const FTransform& SpawnTransform = RangedTransformArray[i];
                        Spawn(MonsterType, SpawnTransform);
                    }
                    else
                    {
                        if (GEngine)
                        {
                            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Not enough spawn points in RangedTransformArray"));
                        }
                        return;
                    }
                }
                else
                {
                    if (GEngine)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid MonsterType or missing TransformArrayPtr"));
                    }
                }
            }
        }
    }

    if (GEngine)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Yellow, TEXT("SpawnMonsters finished"));
    }
}






FVector AMonsterSpawner::GetSpawnPoint()
{
	return GetActorLocation();
}

FVector AMonsterSpawner::GetRandomSpawnPoint()
{
	return FVector::ZeroVector;
}

void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnMonsters(2);

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


