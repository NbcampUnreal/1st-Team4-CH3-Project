// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterSpawner.h"

#include "Kismet/KismetMathLibrary.h"
#include "TheFourthDescendant/Monster/Melee/MeleeMonster.h"
#include "TheFourthDescendant/Monster/Ranged/RangedMonster.h"

AMonsterSpawner::AMonsterSpawner()
{
    PrimaryActorTick.bCanEverTick = false;

    FirstWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Melee, 8));
    SecondWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, 8));
    ThirdWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Melee, 7));
    ThirdWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, 6));

    Levelindex = 0;
    SpawnInterval = 0.2f;

    CurrentMonsterCount = 0;
    TotalMonsterCount = 0;
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
            }
            break;

        case EMonsterType::Ranged:
            if (RangedMonsterClass)
            {
                NewMonster = GetWorld()->SpawnActor<ARangedMonster>(RangedMonsterClass, SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator(), SpawnParams);
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
            // 랜덤 위치 선택
            SpawnLocation = UKismetMathLibrary::RandomPointInBoundingBox(SpawnArea.GetCenter(), SpawnArea.GetExtent());
            bIsLocationValid = true;

            // 충돌 체크
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
                FCollisionShape::MakeSphere(50.0f), // 충돌 범위 설정
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
        }
    }
}



// 여러 몬스터 스폰하는 함수
void AMonsterSpawner::SpawnMonsters(int32 LevelIndex)
{
    TArray<TTuple<EMonsterType, int32>>* SpawnDataPtr = nullptr;
    TArray<FTransform>* MeleeTransformArrayPtr = nullptr;
    TArray<FTransform>* RangedTransformArrayPtr = nullptr;

    switch (LevelIndex)
    {
        case 0:
            SpawnDataPtr = &FirstWaveSpawnData;
            MeleeTransformArrayPtr = &FirstWaveTransformArray;
            break;
        case 1:
            SpawnDataPtr = &SecondWaveSpawnData;
            break;
        case 2:
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
            EMonsterType MonsterType = Data.Key;
            int32 Count = Data.Value;

            for (int32 i = 0; i < Count; ++i)
            {
                if (LevelIndex == 1)
                {
                    // 0.2초 간격으로 랜덤 스폰
                    GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, [this, MonsterType]()
                    {
                        RandomSpawn(MonsterType);
                    }, SpawnInterval, false);
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
                        return;
                    }
                }
            }
        }
    }
}


void AMonsterSpawner::SpawnNextMonster()
{
    if (CurrentMonsterCount < TotalMonsterCount)
    {
        RandomSpawn(CurrentMonsterType);
        CurrentMonsterCount++;
    }
    else
    {
        GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
    }
}


void AMonsterSpawner::BeginPlay()
{
	Super::BeginPlay();

	SpawnMonsters(Levelindex);
}



