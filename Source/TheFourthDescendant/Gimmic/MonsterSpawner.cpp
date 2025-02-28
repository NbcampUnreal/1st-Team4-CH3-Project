// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterSpawner.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "TheFourthDescendant/GameManager/MainGameStateBase.h"
#include "TheFourthDescendant/Monster/Melee/MeleeMonster.h"
#include "TheFourthDescendant/Monster/Ranged/RangedMonster.h"

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    Levelindex = 0;
    SpawnInterval = 0.2f;

    CurrentMonsterCount = 0;
}




// (근접, 원거리) 스폰 위치에 몬스터 생성	
void AMonsterSpawner::Spawn(EMonsterType MonsterType, const FTransform& SpawnTransform)
{
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
                AMainGameStateBase* MainGameState = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
                MainGameState->OnEnemySpawned();
            }
            break;

        case EMonsterType::Ranged:
            if (RangedMonsterClass)
            {
                NewMonster = GetWorld()->SpawnActor<ARangedMonster>(RangedMonsterClass, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnParams);
                AMainGameStateBase* MainGameState = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
                MainGameState->OnEnemySpawned();
            }
            break;

        case EMonsterType::Boss:
            // Boss 몬스터 생성 로직이 있는 경우 여기에 추가
                break;
        }
    }
}

// 랜덤한 위치에 몬스터 스폰하는 함수
void AMonsterSpawner::RandomSpawn(EMonsterType MonsterType)
{
    if (GetWorld())
    {
        FVector SpawnLocation;
        bool bIsLocationValid = false;
        int32 MaxAttempts = 100;
        int32 Attempts = 0;

        while (!bIsLocationValid && Attempts < MaxAttempts)
        {
            SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(SpawnArea.GetCenter(), SpawnArea.GetExtent());
            bIsLocationValid = true;

            TArray<FHitResult> HitResults;
            FCollisionQueryParams QueryParams;
            QueryParams.bTraceComplex = true;
            QueryParams.bReturnPhysicalMaterial = false;

            bool bHasCollision = GetWorld()->SweepMultiByChannel(
                HitResults,
                SpawnLocation,
                SpawnLocation + FVector(0.1f, 0.1f, 0.1f),
                FQuat::Identity,
                ECC_Pawn,
                FCollisionShape::MakeSphere(50.0f),
                QueryParams
            );

            if (bHasCollision)
            {
                bIsLocationValid = false;
            }

            Attempts++;
        }

        if (bIsLocationValid)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

            AMonster* NewMonster = nullptr;
            switch (MonsterType)
            {
                case EMonsterType::Melee:
                    if (MeleeMonsterClass)
                    {
                        NewMonster = GetWorld()->SpawnActor<AMeleeMonster>(MeleeMonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
                        AMainGameStateBase* MainGameState = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
                        MainGameState->OnEnemySpawned();
                    }
                    break;
                case EMonsterType::Ranged:
                    if (RangedMonsterClass)
                    {
                        NewMonster = GetWorld()->SpawnActor<ARangedMonster>(RangedMonsterClass, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
                        AMainGameStateBase* MainGameState = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
                        MainGameState->OnEnemySpawned();
                    }
                    break;
                case EMonsterType::Boss:
                    // Boss 몬스터 생성 로직 추가 시 여기에
                    break;
            }
        }
    }
}




// 여러 몬스터 스폰하는 함수
void AMonsterSpawner::SpawnMonsters(int32 LevelIndex)
{
    CurrentMonsterCount = 0;
    Levelindex = LevelIndex;

    TArray<TTuple<EMonsterType, int32>>* SpawnDataPtr = nullptr;
    TArray<FTransform>* MeleeTransformArrayPtr = nullptr;
    TArray<FTransform>* RangedTransformArrayPtr = nullptr;

    switch (LevelIndex)
    {
        case 0: // 웨이브 1
            SpawnDataPtr = &FirstWaveSpawnData;
            MeleeTransformArrayPtr = &FirstWaveTransformArray;
            break;
        case 1: // 웨이브 2 (랜덤 스폰)
            SpawnDataPtr = &SecondWaveSpawnData;
            break;
        case 2: // 웨이브 3
            SpawnDataPtr = &ThirdWaveSpawnData;
            MeleeTransformArrayPtr = &ThirdWaveMeleeTransformArray;
            RangedTransformArrayPtr = &ThirdWaveRangedTransformArray;
            break;
        default:
            return;
    }

    if (SpawnDataPtr)
    {
        for (const TTuple<EMonsterType, int32>& Data : *SpawnDataPtr)
        {
            CurrentMonsterType = Data.Key;
            int32 Count = Data.Value;  // 실제 생성해야 하는 몬스터 수

            if (LevelIndex == 1)
            {
                // 두 번째 웨이브: 타이머를 이용하여 일정 간격으로 랜덤 스폰
                GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, [this, Count]()
                {
                    if (CurrentMonsterCount < Count)
                    {
                        RandomSpawn(CurrentMonsterType);
                        CurrentMonsterCount++;
                    }
                    else
                    {
                        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
                    }
                }, SpawnInterval, true);
            }
            else
            {
                // 웨이브 1 및 웨이브 3: 지정된 스폰 포인트에서 동시에 스폰
                for (int32 i = 0; i < Count; ++i)
                {
                    if (CurrentMonsterType == EMonsterType::Melee && MeleeTransformArrayPtr)
                    {
                        TArray<FTransform>& MeleeTransformArray = *MeleeTransformArrayPtr;
                        if (i < MeleeTransformArray.Num())
                        {
                            const FTransform& SpawnTransform = MeleeTransformArray[i];
                            Spawn(CurrentMonsterType, SpawnTransform);
                        }
                    }
                    else if (CurrentMonsterType == EMonsterType::Ranged && RangedTransformArrayPtr)
                    {
                        TArray<FTransform>& RangedTransformArray = *RangedTransformArrayPtr;
                        if (i < RangedTransformArray.Num())
                        {
                            const FTransform& SpawnTransform = RangedTransformArray[i];
                            Spawn(CurrentMonsterType, SpawnTransform);
                        }
                    }
                }
            }
        }
    }
}

void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

    FirstWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Melee, FirstWaveMonsterCount));
    SecondWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, SecondWaveMonsterCount));
    ThirdWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Melee, ThirdWaveMeleeCount));
    ThirdWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, ThirdWaveRangedCount));

	//SpawnMonsters(Levelindex);
}



