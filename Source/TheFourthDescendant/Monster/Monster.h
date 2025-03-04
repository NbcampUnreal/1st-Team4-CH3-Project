// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TheFourthDescendant/Abstracts/CharacterBase.h"
#include "TheFourthDescendant/Player/PlayerCharacter.h"
#include "TheFourthDescendant/AI/EnemyController/EnemyController.h"
#include "TheFourthDescendant/ItemSpawnRow.h"
#include "Monster.generated.h"


UCLASS()
class THEFOURTHDESCENDANT_API AMonster : public ACharacterBase
{
	GENERATED_BODY()

public:
	AMonster();

protected:
	/** 몬스터의 공격력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	float AttackPower;
	/** 공격 가능 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bCanAttack;
	/** 스폰 애니메이션이 종료 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bIsSpawned;
	/** 사망 여부 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bIsDead;
	/** 공격할 대상 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	APlayerCharacter* Player;
	/** 빙의중인 AIController */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	AEnemyController* EnemyController;
	/** 변수 값 조정을 위한 블랙보드 컴포넌트 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Target")
	UBlackboardComponent* Blackboard;
	/** 아이템 확률 표 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "ItemSpawn")
	UDataTable* ItemDataTable;

public:
	/** 공격 */
	virtual void Attack();
	/** 이동 */
	virtual void Move();
	/** 사망 로직 처리 */
	virtual void OnDeath();
	/** 아이템 스폰 */
	virtual void SpawnItem(TSubclassOf<AActor> ItemClass);
	/** 아이템 랜덤 스폰 */
	UFUNCTION(BlueprintCallable)
	virtual void SpawnRandomItem(); 
	/** 아이템 랜덤 스폰 결과 */
	FItemSpawnRow* GetRandomItem() const;

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
};
