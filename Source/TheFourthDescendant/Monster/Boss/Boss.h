// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheFourthDescendant/Abstracts/CharacterBase.h"
#include "Boss.generated.h"

class ABossController;
class APlayerCharacter;
class AEnemyController;
class UBlackboardComponent;


UENUM(BlueprintType)
enum class EBossMovementState : uint8
{
	Approaching UMETA(DisplayName = "Approaching"),
	Surrounding UMETA(DisplayName = "Surrounding"),
	BackMoving UMETA(DisplayName = "Backmoving"),
	Idle UMETA(DisplayName = "Idle"),
};

UCLASS()
class THEFOURTHDESCENDANT_API ABoss : public ACharacterBase
{
	GENERATED_BODY()

public:
	ABoss();
	
public:
	/** 현재 적의 이동 상태 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	EBossMovementState MovementState;

protected:
	/** 몬스터의 공격력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	float AttackPower;
	/** 보스 회전 속도 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Status")
	float RotationSpeed;
	/** 공격 가능 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bCanAttack;
	/** 스폰 애니메이션이 종료 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Locomotion")
	bool bIsSpawned;
	/** 사망 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Locomotion")
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
	/** 플레이어와 유지하는 최소거리.
 * 이보다 작아질 경우 후진 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	float MinRadius;
	/** 플레이어와 유지하는 최소거리.
* 후진 상태의 AcceptanceRadius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	float BackMovingAcceptance;
	/** 플레이어와 유지하는 적정거리. 전진 상태의 AcceptanceRadius */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	float ApproachAcceptance;
	/** 플레이어와 유지하는 최대거리.
	 * 이보다 커질 경우 플레이어에게 전진 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	float MaxRadius;
	/** Move 상태를 유지할 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	int32 MoveTime;
	/** Idle 상태를 유지할 시간 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	int32 IdleTime;
	/** 잡몹 패턴 간격 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	int32 SummonPatternInterval;
	/** 하늘로 미사일 발사하는 패턴 간격 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat")
	int32 FlameExplosionPatternInterval;

private:
	/** Move 상태의 시간을 측정할 타이머 */
	FTimerHandle MoveTimer;
	/** Idle 상태의 시간을 측정할 타이머 */
	FTimerHandle IdleTimer;
	/** Summon 패턴을 측정할 타이머 */
	FTimerHandle SummonPatternTimer;
	/** Flame 패턴을 측정할 타이머 */
	FTimerHandle FlamePatternTimer;
	/** Possess 중인 AAIContoller */
	ABossController* BossController;

public:
	/** 잡몹 소환 패턴 시작을 알리는 함수 */
	void SummonPatternStart();
	/** 잡몹 소환 로직 함수 */
	UFUNCTION(BlueprintCallable)
	void SummonMinions();
	/** Flame 패턴 시작을 알리는 함수 */
	void FlamePatternStart();
	/** Flame 로직 함수 */
	UFUNCTION(BlueprintCallable)
	void FlameExplosion();
	/** 전방 이동 */
	void MoveToTarget();
	/** 좌우 이동 */
	void MoveHorizontal(int32& Direction);
	/** 후방 이동 */
	void MoveBack();
	/** 플레이어를 향해 회전 */
	void RotationToTarget(float DeltaSeconds);
	/** 사망 로직 처리 */
	void OnDeath();
	/** 플레이어와 거리를 측정하는 함수 */
	float GetDistanceToPlayer();
	/** 거리에 따라 Move 상태 전이 */
	void SetMoveState();
	/** 이동 상태로 전이 */
	void InitMovementStateToMove();
	/** 상태 초기화 함수.
	* 스폰 이후 블루프린트에서 호출 */
	UFUNCTION(BlueprintCallable)
	void InitMovementStateToIdle();
	
private:
	/** Blackboard의 이동 상태 관련 bool 변수 초기화 */
	void InitBlackboardMovementFlag(const EBossMovementState State);

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
