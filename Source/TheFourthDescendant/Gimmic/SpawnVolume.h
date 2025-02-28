#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnVolume.generated.h"

class AMonster;
class UBoxComponent;

UCLASS()
class THEFOURTHDESCENDANT_API ASpawnVolume : public AActor
{
	GENERATED_BODY()
	
public:	
	ASpawnVolume();

protected:
	/** 씬 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	USceneComponent* Scene;
	/** 스폰 영역 반환을 위한 BoxComponent 선언 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawning")
	UBoxComponent* SpawningBox;
	/** 스폰할 Enemy 클래스 선언 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawning")
	TSubclassOf<AMonster> EnemyClass;

public:
	/** 잡몹 소환 */
	void SpawnEnemy();

private:
	/** 스폰 가능한 영역 반환 */
	FVector GetRandomPointInVolume() const;


};
