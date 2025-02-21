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

public:
protected:
	/* 몬스터 스폰 주기 */
	float SpawnInterval;
	/* 몬스터 스폰 장소 */
	TArray<FVector> SpawnPoints;
	/* 스폰시킬 몬스터 */
	TArray<AMonster> SpawnEnemy;
	/* 스폰시킬 근거리 몬스터 클래스 */
	TSubclassOf<AMonster> MeleeMonsterClass;
	/* 스폰시킬 원거리 몬스터 클래스 */
	TSubclassOf<AMonster> RangedMonsterClass;
private:

public:	
	void Spawn(EMonsterType MonsterType);
	void SpawnMonsters(TArray<TTuple<EMonsterType, int32>> SpawnEnemy);
	void RandomSpawn(EMonsterType MonsterType);
	void RandomSpawnMonsters(TArray<TTuple<EMonsterType, int32>> SpawnEnemy);
	FVector GetSpawnPoint();
	FVector GetRandomSpawnPoint();
	TArray<TTuple<EMonsterType, int32>> FirstWaveSpawnData;
	TArray<TTuple<EMonsterType, int32>> SecondWaveSpawnData;
	TArray<TTuple<EMonsterType, int32>> ThirdWaveSpawnData;
protected:
	virtual void BeginPlay() override;
private:

};
