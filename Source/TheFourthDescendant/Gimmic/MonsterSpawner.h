// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TheFourthDescendant/Monster/Monster.h"
#include "MonsterSpawner.generated.h"

UENUM(BlueprintType)
enum class EMonsterType : uint8
{
	Melee UMETA(DisplayName = "Melee"),
	Ranged UMETA(DisplayName = "Ranged"),
	Boss UMETA(DisplayName = "Boss")
};

UCLASS()
class THEFOURTHDESCENDANT_API AMonsterSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	AMonsterSpawner();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Wave")
	int32 Levelindex;

public:
protected:
	/* 몬스터 스폰 주기 */
	UPROPERTY(EditAnywhere, Category="Spawning")
	float SpawnInterval;
	/* FTimerHandle */
	FTimerHandle SpawnTimerHandle;
	/* 현재 스폰된 몬스터 수 */
	int32 CurrentMonsterCount;
	/* 스폰할 몬스터 타입 */
	EMonsterType CurrentMonsterType;
	TArray<TTuple<EMonsterType, int32>> FirstWaveSpawnData;
	TArray<TTuple<EMonsterType, int32>> SecondWaveSpawnData;
	TArray<TTuple<EMonsterType, int32>> ThirdWaveSpawnData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 FirstWaveMonsterCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 SecondWaveMonsterCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 ThirdWaveMeleeCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawning")
	int32 ThirdWaveRangedCount;

	/* 스폰시킬 근거리 몬스터 클래스 */	
	UPROPERTY(EditAnywhere, Category="Spawning")
	TSubclassOf<AMonster> MeleeMonsterClass;
	/* 스폰시킬 원거리 몬스터 클래스 */
    UPROPERTY(EditAnywhere, Category="Spawning")
	TSubclassOf<AMonster> RangedMonsterClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default", meta = (MakeEditWidget))
	FVector TestVector;
	/* 웨이브 1용 스폰 위치 배열 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Default", meta = (MakeEditWidget))
	TArray<FTransform> FirstWaveTransformArray;
	/* 웨이브 1용 스폰 위치 배열 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Default", meta = (MakeEditWidget))
	TArray<FTransform> SecondWaveTransformArray;
	/* 웨이브 3용 근거리 몬스터 스폰 위치 배열 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Default", meta = (MakeEditWidget))
	TArray<FTransform> ThirdWaveMeleeTransformArray;
	/* 웨이브 3용 원거리 몬스터 스폰 위치 배열 */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Default", meta = (MakeEditWidget))
	/* 몬스터 랜덤 스폰 구역 */
	TArray<FTransform> ThirdWaveRangedTransformArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spawn Config")
	FBox SpawnArea;
	
private:

public:
	/* 몬스터 하나를 스폰시키는 함수 */
	void Spawn(EMonsterType MonsterType, const FTransform& SpawnTransform);
	/* 몬스터 웨이브를 스폰시키는 함수 */
	void SpawnMonsters(int32 LevelIndex);
protected:
	virtual void BeginPlay() override;
private:

};
