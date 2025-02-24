#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MainGameStateBase.generated.h"

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
	TArray<FLevelEnded> LevelEnded;


public:
	virtual void BeginPlay() override;
 	/** 레벨을 시작하는 함수 */
	UFUNCTION(BlueprintCallable, Category = "Level")
	void StartLevel();
	/** 레벨과 웨이브 Index를 설정 */
	UFUNCTION(BlueprintCallable, Category = "Level")
	void SetNextLevelWaveIndex();
	/** 몬스터 처치 시 호출하는 함수 */
	UFUNCTION(BlueprintCallable, Category = "Level")
	void OnEnemyKilled();
};
