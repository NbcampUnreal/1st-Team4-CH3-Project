// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TheFourthDescendant/Abstracts/CharacterBase.h"
#include "Boss.generated.h"

class AHomingProjectile;
class ABombProjectile;
class AProjectileBase;
class ASpawnVolume;
class AMineItem;
class AMissileProjectile;
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

UENUM(BlueprintType)
enum class EBossGroggyType : uint8
{
	Default UMETA(DisplayName = "Default"),
	Triggered UMETA(DisplayName = "Triggered"),
	Actioned UMETA(DisplayName = "Actioned"),
};

UCLASS()
class THEFOURTHDESCENDANT_API ABoss : public ACharacterBase
{
	GENERATED_BODY()

public:
	ABoss();
	
public:
	/** Bone 회전을 위한 SkeletalMesh 선언 */
	USkeletalMeshComponent* Mesh;
	/** 현재 적의 이동 상태 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Locomotion")
	EBossMovementState MovementState;
	/** 잡몹 소환 패턴 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion")
	bool bIsSummon;
	/** FlameExplosion 패턴 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion")
	bool bIsFlame;
	/** Buster 패턴 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion")
	bool bIsBuster;
	/** 왼쪽 SlugShot 패턴 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion")
	bool bIsLSlugShot;
	/** 오른쪽 SlugShot 패턴 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion")
	bool bIsRSlugShot;
	/** 왼쪽 JavelinShot 패턴 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion")
	bool bIsLJavelinShot;
	/** 오른쪽 JavelinShot 패턴 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion")
	bool bIsRJavelinShot;
	/** 일반 공격 패턴 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion")
	bool bIsAttacking;
	/** 그로기 여부 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Locomotion")
	bool bIsGroggy;

	
	
protected:
	/** 부위 파괴를 적용할 부위 이름 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region")
	TArray<FString> RegionNames;
	/** 부위 파괴를 적용할 최소 데미지 횟수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region")
	int32 RegionDestroyCount;
	/** 부위 파괴 시 적용할 추가 데미지 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Region")
	int32 RegionDestroyDamage;
	/** Javelin 발사 사운드*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern|Sound")
	class USoundCue* JavelinSound;
	/** Javelin 발사 사운드*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pattern|Sound")
	class USoundBase* FlameSound;
	/** 발사할 유도 미사일 클래스 정보 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	TSubclassOf<AHomingProjectile> HomingClass;
	/** 발사할 폭탄 클래스 정보 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	TSubclassOf<ABombProjectile> BombClass;
	/** 발사할 미사일 클래스 정보 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	TSubclassOf<AMissileProjectile> MissileClass;
	/** 미사일 발사 후 설치할 Mine 정보 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	TSubclassOf<AMineItem> MineClass;
	/** 미사일 발사 후 폭파할 위치 정보 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Projectile")
	TArray<AActor*> ExplosionLocations;
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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat|Pattern")
	int32 SummonPatternInterval;
	/** 잡몹 스포너 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat|Pattern")
	ASpawnVolume* EnemySpawner;
	/** 소환할 잡몹 수 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat|Pattern")
	int32 SpawnEnemyCounter;
	/** 하늘로 미사일 발사하는 패턴 간격 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat|Pattern")
	int32 FlameExplosionPatternInterval;
	/** BusterStackTimer를 측정 시작하는 최소 거리 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat|Pattern")
	float BusterPatternMinDistance;
	/** 해당 시간만큼 보스 아래에 머물러 있을 경우 Buster 패턴 출력 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat|Pattern")
	int32 BusterMinTime;
	/** BusterTimer가 시작된 상태인지 체크 */
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Stat|Pattern")
	bool bIsBusterTimerTriggered;
	/** 일반 공격 패턴 간격 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat|Pattern")
	int32 NormalAttackPatternInterval;
	/** 일반 공격 패턴 간격 편차 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Stat|Pattern")
	int32 NormalAttackDeviation;

private:
	/** Move 상태의 시간을 측정할 타이머 */
	FTimerHandle MoveTimer;
	/** Idle 상태의 시간을 측정할 타이머 */
	FTimerHandle IdleTimer;
	/** Summon 패턴을 측정할 타이머 */
	FTimerHandle SummonPatternTimer;
	/** Flame 패턴을 측정할 타이머 */
	FTimerHandle FlamePatternTimer;
	/** Flame 패턴을 반복할 타이머 */
	FTimerHandle FlameRepeatTimer;
	/** LJavelinShot 패턴을 반복할 타이머 */
	FTimerHandle LJavelinRepeatTimer;
	/** RJavelinShot 패턴을 반복할 타이머 */
	FTimerHandle RJavelinRepeatTimer;
	/** Buster 패턴을 측정할 타이머 */
	FTimerHandle BusterPatternTimer;
	/** 일반 공격 패턴을 측정할 타이머 */
	FTimerHandle NormalPatternTimer;
	/** Possess 중인 AAIContoller */
	ABossController* BossController;
	/** 로켓을 발사할 소켓의 위치 정보 */
	TArray<FTransform> RocketSocketTransforms;
	/** Flame 반복 횟수 측정 */
	int32 FlameRepeatCount;
	/** LJavelinShot 반복 횟수 측정 */
	int32 LJavelinRepeatCount;
	/** RJavelinShot 반복 횟수 측정 */
	int32 RJavelinRepeatCount;
	/** Spawn 시킬 Mine의 위치 Index */
	int32 MineLocationIndex;
	/** 첫 번째 그로기 타입 */
	EBossGroggyType FirstGroggyType;
	/** 두 번째 그로기 타입 */
	EBossGroggyType SecondGroggyType;
	/** 부위별 공격 횟수 누적 */
	TMap<FName, int32> RegionAttackCount;

public:
	/** 부위별 데미지 적용 함수 */
	UFUNCTION(BlueprintImplementableEvent)
	void ApplyPartsDamaged(FName PartsName);
	/** 부위 파괴 함수 */
	UFUNCTION(BlueprintImplementableEvent)
	void DestroyRegion(FName PartsName);
	/** 잡몹 소환 패턴 시작을 알리는 함수 */
	void SummonPatternStart();
	/** 잡몹 소환 로직 함수 */
	UFUNCTION(BlueprintCallable)
	void SummonMinions();
	/** Flame 패턴 시작을 알리는 함수 */
	void FlamePatternStart();
	/** Flame 로직 함수 */
	UFUNCTION(BlueprintCallable)
	void SetFlameExplosionTimer();
	/** Flame 로직 함수 */
	void FlameExplosion();
	/** Buster 패턴 시작을 알리는 함수 */
	void BusterPatternStart();
	/** Buster 로직 함수 */
	UFUNCTION(BlueprintCallable)
	void Buster();
	/** 일반 공격 패턴 타이머 활성화 함수 */
	UFUNCTION(BlueprintCallable)
	void SetNormalAttackTimer();
	/** 왼쪽 SlugShot 로직 구현 */
	UFUNCTION(BlueprintCallable)
	void LSlugShot();
	/** 오른쪽 SlugShot 로직 구현 */
	UFUNCTION(BlueprintCallable)
	void RSlugShot();
	/** 왼쪽 JavelinShot 타이머 활성화 */
	UFUNCTION(BlueprintCallable)
	void LJavelinShotStart();
	/** 왼쪽 JavelinShot 로직 구현 */
	void LJavelinShot();
	/** 오른쪽 JavelinShot 타이머 활성화 */
	UFUNCTION(BlueprintCallable)
	void RJavelinShotStart();
	/** 오른쪽 JavelinShot 로직 구현 */
	void RJavelinShot();
	/** 보스 부위 데미지 판별 */
	void HandleDamageToPart(FName PartsName, float& Damage);
	/** Buster 인식 거리인지 측정 */
	void IsInBusterBound(float& Distance);
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
	/** Bone의 회전 보간 값을 반환하는 함수 */
	UFUNCTION(BlueprintCallable)
	FRotator GetBoneRotation(FName BoneName);
	/** 거리에 따라 Move 상태 전이 */
	void SetMoveState();
	/** 이동 상태로 전이 */
	UFUNCTION(BlueprintCallable)
	void InitMovementStateToMove();
	/** 상태 초기화 함수.
	* 스폰 이후 블루프린트에서 호출 */
	UFUNCTION(BlueprintCallable)
	void InitMovementStateToIdle();

protected:
	virtual void BeginPlay() override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	/** Blackboard의 이동 상태 관련 bool 변수 초기화 */
	void InitBlackboardMovementFlag(const EBossMovementState State);

	void IsFirstGroggyTriggered();
	void IsSecondGroggyTriggered();
	void IsCurrentHealthZero();
	

};
