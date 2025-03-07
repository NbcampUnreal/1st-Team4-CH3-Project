#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameStateBase.generated.h"

class AMonsterSpawner;
DECLARE_MULTICAST_DELEGATE(FLevelEnded);

UCLASS()
class THEFOURTHDESCENDANT_API AMainGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	AMainGameStateBase();

public:
	/** <Level, Wave수> ex) <1, 2> : 1레벨에는 Wave가 2개 존재 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TMap<int32, int32> LevelWaveInfo;
	/** 현재 레벨 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	int32 CurrentLevelIndex;
	/** 지금까지 진행한 웨이브 수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	int32 TotalWaveCount;
	/** 각 레벨에 존재하는 웨이브 수를 체크하기 위한 변수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	int32 TempWaveIndex;
	/** 스폰된 몬스터 수 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Level")
	int32 SpawnedEnemyCount;
	/** 레벨 전환 시 호출될 델리게이트 함수 */
	FLevelEnded LevelEnded;
	/** 레벨에서 사용할 몬스터 스포너 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Level")
	AMonsterSpawner* Spawner;
	/** 웨이브 전환 간격 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	float WaveInterval;
	/** 레벨 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TArray<FName> LevelNames;
	/** 레벨 전환 간격 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	TArray<float> LevelInterval;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBossDead, bool, bBossDead);
	/** 보스가 죽었을 때 타이머 종료에 의해 호출되는 이벤트 */
	UPROPERTY(BlueprintAssignable, Category = "Level")
	FOnBossDead OnBossDead;

private:
	/** 스포너에 사용할 타이머 핸들러 */
	FTimerHandle SpawnerTimer;
	/** 씬 전환에 사용될 타이머 핸들러 */
	FTimerHandle LevelLoadTimer;
	/** 보스 레벨 여부 */
	bool bIsBossLevel;


public:
	virtual void BeginPlay() override;
 	/** 레벨을 시작하는 함수 */
	UFUNCTION(BlueprintCallable, Category = "Level")
	void StartLevel();
	/** 스폰 시작하는 함수 */
	UFUNCTION(BlueprintCallable, Category = "Level")
	void EnemySpawn();
	/** 레벨과 웨이브 Index를 설정 */
	UFUNCTION(BlueprintCallable, Category = "Level")
	void SetNextLevelWaveIndex();
	/** 몬스터 처치 시 호출하는 함수 */
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnEnemyKilled();
	/** 몬스터 스폰 시 호출하는 함수 */
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnEnemySpawned();
	/** 현재 레벨이 끝나는 조건을 만족했을 때 호출 */
	void OnNormalLevelEnded();
	/** 보스 레벨이 끝나는 조건을 만족했을 때 호출 */
	void OnBossLevelEnded();

private:
	/** 일반 몬스터 필드의 스포너를 가져오는 함수 */
	void GetMonsterSpawner();
};
