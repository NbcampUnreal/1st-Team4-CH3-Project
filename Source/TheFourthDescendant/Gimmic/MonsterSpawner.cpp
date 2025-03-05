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

void AMonsterSpawner::Spawn(EMonsterType MonsterType, const FTransform& SpawnTransform)
{
    if (!GetWorld())
        return;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    switch (MonsterType)
    {
    case EMonsterType::Melee:
        if (MeleeMonsterClass)
        {
            GetWorld()->SpawnActor<AActor>(MeleeMonsterClass, 
                SpawnTransform.GetLocation(), 
                SpawnTransform.GetRotation().Rotator(), 
                SpawnParams);
            AMainGameStateBase* MainGameState = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
            if (MainGameState)
                MainGameState->OnEnemySpawned();
        }
        break;
    case EMonsterType::Ranged:
        if (RangedMonsterClass)
        {
            GetWorld()->SpawnActor<AActor>(RangedMonsterClass,
                SpawnTransform.GetLocation(), 
                SpawnTransform.GetRotation().Rotator(), 
                SpawnParams);
            AMainGameStateBase* MainGameState = Cast<AMainGameStateBase>(UGameplayStatics::GetGameState(GetWorld()));
            if (MainGameState)
                MainGameState->OnEnemySpawned();
        }
        break;
    case EMonsterType::Boss:
        // 필요 시 Boss 몬스터 생성 로직 추가
        break;
    }
}

void AMonsterSpawner::SpawnMonsters(int32 LevelIndex)
{
    CurrentMonsterCount = 0;
    Levelindex = LevelIndex;

    // 웨이브 3은 별도 처리
    if (LevelIndex == 2)
    {
        // CombinedInstructions 배열에 각 튜플의 소환 지시(몬스터 타입, Transform)를 모두 합칩니다.
        TArray<TPair<EMonsterType, FTransform>> CombinedInstructions;

        // ThirdWaveSpawnData에 들어있는 각 튜플 처리
        for (const TTuple<EMonsterType, int32>& Data : ThirdWaveSpawnData)
        {
            EMonsterType TupleType = Data.Key;
            int32 Count = Data.Value;
            if (TupleType == EMonsterType::Melee && ThirdWaveMeleeTransformArray.Num() > 0)
            {
                int32 ExecCount = FMath::Min(Count, ThirdWaveMeleeTransformArray.Num());
                for (int32 i = 0; i < ExecCount; i++)
                {
                    CombinedInstructions.Add(TPair<EMonsterType, FTransform>(TupleType, ThirdWaveMeleeTransformArray[i]));
                }
            }
            else if (TupleType == EMonsterType::Ranged && ThirdWaveRangedTransformArray.Num() > 0)
            {
                int32 ExecCount = FMath::Min(Count, ThirdWaveRangedTransformArray.Num());
                for (int32 i = 0; i < ExecCount; i++)
                {
                    CombinedInstructions.Add(TPair<EMonsterType, FTransform>(TupleType, ThirdWaveRangedTransformArray[i]));
                }
            }
        }

        // CombinedInstructions 배열에 지시가 있으면 하나의 타이머로 순차적으로 소환합니다.
        if (CombinedInstructions.Num() > 0)
        {
            TSharedRef<int32> SharedIndex = MakeShared<int32>(0);
            GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, FTimerDelegate::CreateLambda([this, CombinedInstructions, SharedIndex]() mutable
            {
                if (*SharedIndex < CombinedInstructions.Num())
                {
                    const TPair<EMonsterType, FTransform>& Instruction = CombinedInstructions[*SharedIndex];
                    Spawn(Instruction.Key, Instruction.Value);
                    (*SharedIndex)++;
                }
                else
                {
                    GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
                }
            }), SpawnInterval, true);
        }
        return;
    }

    // 웨이브 0 또는 웨이브 1에 대해서는 기존 방식 사용
    TArray<TTuple<EMonsterType, int32>>* SpawnDataPtr = nullptr;
    TArray<FTransform>* SpawnPoints = nullptr;

    switch (LevelIndex)
    {
    case 0:  // 웨이브 1 (예: 근접 몬스터)
        SpawnDataPtr = &FirstWaveSpawnData;
        SpawnPoints = &FirstWaveTransformArray;
        break;
    case 1:  // 웨이브 2 (예: 원거리 몬스터)
        SpawnDataPtr = &SecondWaveSpawnData;
        SpawnPoints = &SecondWaveTransformArray;
        break;
    default:
        return;
    }

    if (SpawnDataPtr && SpawnDataPtr->Num() > 0 && SpawnPoints && SpawnPoints->Num() > 0)
    {
        // 여기서는 SpawnDataPtr의 첫 번째 튜플만 사용합니다.
        EMonsterType MonsterType = (*SpawnDataPtr)[0].Key;
        int32 Count = (*SpawnDataPtr)[0].Value;

        TSharedRef<int32> SharedIndex = MakeShared<int32>(0);

        GetWorld()->GetTimerManager().SetTimer(SpawnTimerHandle, FTimerDelegate::CreateLambda([this, Count, MonsterType, SpawnPoints, SharedIndex]() mutable
        {
            if (*SharedIndex < Count && *SharedIndex < SpawnPoints->Num())
            {
                const FTransform& SpawnTransform = (*SpawnPoints)[*SharedIndex];
                Spawn(MonsterType, SpawnTransform);
                (*SharedIndex)++;
            }
            else
            {
                GetWorld()->GetTimerManager().ClearTimer(SpawnTimerHandle);
            }
        }), SpawnInterval, true);
    }
}


void AMonsterSpawner::BeginPlay()
{
    Super::BeginPlay();
    
    // Spawn 데이터 초기화
    FirstWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Melee, FirstWaveMonsterCount));
    SecondWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, SecondWaveMonsterCount));
    ThirdWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Melee, ThirdWaveMeleeCount));
    ThirdWaveSpawnData.Add(TTuple<EMonsterType, int32>(EMonsterType::Ranged, ThirdWaveRangedCount));
}