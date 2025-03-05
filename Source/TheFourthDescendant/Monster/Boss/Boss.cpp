#include "Boss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "TheFourthDescendant/AI/EnemyController/BossController.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"
#include "TheFourthDescendant/GameManager/MainGameStateBase.h"
#include "TheFourthDescendant/Gimmic/SpawnVolume.h"
#include "TheFourthDescendant/Item/MineItem/MineItem.h"
#include "TheFourthDescendant/Monster/Projectile/BombProjectile.h"
#include "TheFourthDescendant/Monster/Projectile/HomingProjectile.h"
#include "TheFourthDescendant/Monster/Projectile/MissileProjectile.h"
#include "TheFourthDescendant/Monster/Projectile/ProjectileBase.h"
#include "TheFourthDescendant/Player/PlayerCharacter.h"

#pragma region InitComponent
ABoss::ABoss()
{
	BerserkType = EBossBerserkType::Default;
	FirstGroggyType = EBossGroggyType::Default;
	SecondGroggyType = EBossGroggyType::Default;
	bCanAttack = false;
	bIsSpawned = false;
	bIsDead = false;
	bIsSummon = false;
	bIsFlame = false;
	bIsBuster = false;
	bIsLSlugShot = false;
	bIsRSlugShot = false;
	bIsBusterTimerTriggered = false;
	KneeItemDestroyedCount = 0;
	LJavelinRepeatCount = 0;
	RJavelinRepeatCount = 0;
	MineLocationIndex = 0;
	FlameRepeatCount = 0;
	AttackPower = 0;
	MinRadius = 200;
	BackMovingAcceptance = 400;
	ApproachAcceptance = 600;
	MaxRadius = 800;
	SummonPatternInterval = 70;
	FlameExplosionPatternInterval = 90;
	NormalAttackPatternInterval = 6;
	NormalAttackDeviation = 2;
	RotationSpeed = 40;
	MoveTime = 180;
	IdleTime = 5;
	BusterPatternMinDistance = 1050;
	BusterMinTime = 3;
	Player = nullptr;
	BossController = nullptr;
	EnemyController = nullptr;
	Blackboard = nullptr;
	Mesh = nullptr;
	EnemySpawner = nullptr;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	MovementState = EBossMovementState::Idle;
	RocketSocketTransforms.Empty();
	ExplosionLocations.Empty();
	
}


void ABoss::BeginPlay()
{
	Super::BeginPlay();

	// 플레이어 캐스팅
	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(),0));
	if (Player == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Boss BeginPlay : Character Casting Failed !");
	}

	// AIController 캐스팅
	BossController = Cast<ABossController>(GetController());
	if (BossController == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Boss BeginPlay : AIController Casting Failed !");
	}

	// 블랙보드 할당
	Blackboard = BossController->GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Boss BeginPlay : BlackBoard Casting Failed !");
	}
	Blackboard->SetValueAsObject(FName("TargetActor"), Player);

	// 스켈레탈 메시 할당
	Mesh = GetMesh();

	// 캐릭터 이동속도 초기화
	GetCharacterMovement()->MaxWalkSpeed = Status.WalkSpeed;

	// 체력 및 쉴드 초기화
	Status.Health = Status.MaxHealth;
	Status.Shield = Status.MaxShield;

	// 보스 패턴 타이머 세팅
	SetPatternTimer(1);
}
#pragma endregion

#pragma region Damage, Death
float ABoss::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                        class AController* EventInstigator, AActor* DamageCauser)
{
	// 스폰 중이거나 사망했을 경우 return
	if (!bIsSpawned || bIsDead) return 0;

	if (DamageEvent.IsOfType(FPointDamageEvent::ClassID))
	{
		const FPointDamageEvent* PointDamage = static_cast<const FPointDamageEvent*>(&DamageEvent);
		
		FHitResult HitResult = PointDamage->HitInfo;
		FName RegionName = *HitResult.PhysicsObjectOwner->GetName();
		HandleDamageToPart(RegionName, DamageAmount);
		HandleDamageToKneeItem(RegionName);

		// 충돌 결과 전파
		OnHitInfo.Broadcast(HitResult, DamageAmount);		
	}
	
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	
	IsCurrentHealthZero();
	IsFirstGroggyTriggered();
	IsSecondGroggyTriggered();
	IsBerserkTriggered();

	// 게임 인스턴스 캐스팅
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GameInstance);

	// 가한 피해량 증가
	MainGameInstance->AddDealtDamageToEnemy(ActualDamage);

	return ActualDamage;
}



void ABoss::OnDeath()
{
	// 사망했을 경우 return
	if (bIsDead) return;
	
	// 체력 0으로 초기화 및 콜리전 비활성화
	Status.Health = 0;
	SetActorEnableCollision(false);

	// 사망 플래그 값으로 애니메이션 재생
	bIsDead = true;
	Blackboard->SetValueAsBool(FName("IsDead"), true);

	// 죽인 적 수 반영
	UGameInstance* GameInstance = GetGameInstance();
	UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GameInstance);
	MainGameInstance->AddKilledEnemyCount(1);
}
#pragma endregion

#pragma region Move Functions
void ABoss::MoveToTarget()
{
	if (Player == nullptr) return;
	
	FVector BossLocation = GetActorLocation();
	FVector PlayerLocation = Player->GetActorLocation();

	FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();

	AddMovementInput(Direction, 1.0f);
}

void ABoss::MoveHorizontal(int32& Direction)
{
	// 움직일 방향벡터 선언
	FVector DirectionVector = FVector::ZeroVector;
	
	// 방향벡터 왼쪽으로 초기화
	if (Direction == 0)
	{
		DirectionVector = -GetActorRightVector();
	}
	// 방향벡터 오른쪽으로 초기화
	else if (Direction == 1)
	{
		DirectionVector = GetActorRightVector();
	}

	AddMovementInput(DirectionVector, 1.0f);
}


void ABoss::MoveBack()
{
	// 보스의 후방 벡터 반환
	FVector BackDirection = -GetActorForwardVector();
	
	// 뒤로 이동
	AddMovementInput(BackDirection, 1.0f);
}


void ABoss::RotationToTarget(float DeltaSeconds)
{
	if (!Player) return;
	
	// 버서커 상태로 전이가 가능한지 확인
	IsBerserkTransitionPossible();

	RotationToTargetWithAllBones(DeltaSeconds);
	
	// 거리에 따라 보스 이동 모드 설정
	SetMoveState();
}
#pragma endregion

#pragma region Special Attack Pattern

void ABoss::SummonPatternStart()
{
	// Blackboard 값 초기화
	bIsSummon = true;
}

void ABoss::SummonMinions()
{
	for (int i=0; i<SpawnEnemyCounter; ++i)
	{
		EnemySpawner->SpawnEnemy();	
	}
}



void ABoss::FlamePatternStart()
{
	// 소켓 배열 초기화
	RocketSocketTransforms.Empty();
	
	// Blackboard 값 초기화
	bIsFlame = true;
}

void ABoss::SetFlameExplosionTimer()
{
	GetWorldTimerManager().SetTimer(
		FlameRepeatTimer,
		this,
		&ABoss::FlameExplosion,
		0.075f,
		true);
}

void ABoss::FlameExplosion()
{
	if (MissileClass == nullptr) return;

	// 소켓 이름 리스트 생성
	for (int i = 1; i <= 6; i++)
	{
		FString SocketName = FString::Printf(TEXT("RocketSocket%d"), i);
		FTransform SocketTransform = Mesh->GetSocketTransform(FName(*SocketName), ERelativeTransformSpace::RTS_World);

		// 배열에 추가
		RocketSocketTransforms.Add(SocketTransform);
	}

	// 미사일을 발사할 소켓의 위치를 랜덤으로 할당
	int32 RandSocketIndex = FMath::RandRange(0,5);
	FVector SpawnLocation = RocketSocketTransforms[RandSocketIndex].GetLocation();
	FRotator SpawnRotation = RocketSocketTransforms[RandSocketIndex].Rotator();

	// 발사할 미사일 캐스팅
	AMissileProjectile* SpawnedRocket = GetWorld()->SpawnActor<AMissileProjectile>(MissileClass, SpawnLocation, SpawnRotation);

	// 랜덤 각도 할당
	float RandSocketRoll = FMath::RandRange(-0.2f, 0.2f);
	float RandSocketPitch = FMath::RandRange(-0.2f, 0.2f);

	// 미사일 발사
	if (SpawnedRocket)
	{
		FVector LaunchDirection = FVector(RandSocketRoll, RandSocketPitch, 1); 
		SpawnedRocket->FireMissileIntoTheSky(LaunchDirection);
	}

	// 미사일 발사 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(this, FlameSound, SpawnLocation, SpawnRotation);

	// 스폰할 지뢰 좌표 할당
	FVector MineSpawnLocation = ExplosionLocations[MineLocationIndex]->GetActorLocation();

	// 스폰할 Mine 캐스팅
	AMineItem* Mine = GetWorld()->SpawnActor<AMineItem>(MineClass, MineSpawnLocation, FRotator::ZeroRotator);
	

	++FlameRepeatCount;
	++MineLocationIndex;

	if (FlameRepeatCount >= 16)
	{
		GetWorldTimerManager().ClearTimer(FlameRepeatTimer);
		FlameRepeatCount = 0;
	}

	if (MineLocationIndex >= ExplosionLocations.Num())
	{
		MineLocationIndex = 0;
	}
}



void ABoss::BusterPatternStart()
{
	// Blackboard 값 초기화
	bIsBuster = true;

	// Buster Timer 초기화
	GetWorldTimerManager().ClearTimer(BusterPatternTimer);
	bIsBusterTimerTriggered = false;
}

void ABoss::Buster()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Buster Explosion");
}

#pragma endregion

#pragma region Normal Attack Pattern
void ABoss::LSlugShot()
{
	if (Player == nullptr) return;

	// 플레이어 위치 반환
	FVector PlayerLocation = Player->GetActorLocation();
	
	// 왼쪽 팔이 사격 상태라면 왼쪽 총구에서 발사
	if (bIsLSlugShot)
	{
		FVector LeftMuzzleLocation = Mesh->GetSocketLocation(FName("L_MuzzleSocket"));
		FVector LTargetDirection = (PlayerLocation - LeftMuzzleLocation).GetSafeNormal();
		ABombProjectile* Projectile = GetWorld()->SpawnActor<ABombProjectile>(BombClass, LeftMuzzleLocation, FRotator::ZeroRotator);
		if (Projectile != nullptr)
		{
			Projectile->Fire(LTargetDirection);
		}
	}
}

void ABoss::RSlugShot()
{
	if (Player == nullptr) return;

	// 플레이어 위치 반환
	FVector PlayerLocation = Player->GetActorLocation();

	// 오른쪽 팔이 사격 상태라면 오른쪽 총구에서 발사
	if (bIsRSlugShot)
	{
		FVector RightMuzzleLocation = Mesh->GetSocketLocation(FName("R_MuzzleSocket"));
		FVector RTargetDirection = (PlayerLocation - RightMuzzleLocation).GetSafeNormal();
		ABombProjectile* Projectile = GetWorld()->SpawnActor<ABombProjectile>(BombClass, RightMuzzleLocation, FRotator::ZeroRotator);
		if (Projectile != nullptr)
		{
			Projectile->Fire(RTargetDirection);
		}
	}
}

void ABoss::LJavelinShotStart()
{
	GetWorldTimerManager().SetTimer(
		LJavelinRepeatTimer,
		this,
		&ABoss::LJavelinShot,
		0.4f,
		true);
}


void ABoss::LJavelinShot()
{
	// 왼쪽 어깨 미사일 Socket 반환
	FString SocketName = FString::Printf(TEXT("MissileSocket_L%d"), LJavelinRepeatCount+1);
	FTransform SocketTransform = Mesh->GetSocketTransform(FName(*SocketName), ERelativeTransformSpace::RTS_World);

	// Homing Projectile 생성
	AHomingProjectile* HomingProjectile = GetWorld()->SpawnActor<AHomingProjectile>(HomingClass,SocketTransform.GetLocation(),FRotator::ZeroRotator);

	// 방향벡터 설정
	FVector LaunchDirection = FVector(0, -1, 1); 

	// 발사
	HomingProjectile->Fire(LaunchDirection);
	
	// 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(this, JavelinSound, GetActorLocation());
	
	// 발사한 탄환 수 1개 증가
	++LJavelinRepeatCount;

	// 모두 발사한 경우 변수 및 타이머 초기화
	if (LJavelinRepeatCount >=  5)
	{
		LJavelinRepeatCount = 0;
		GetWorldTimerManager().ClearTimer(LJavelinRepeatTimer);
		SetNormalAttackTimer();
		bIsLJavelinShot = false;
	}
	
}

void ABoss::RJavelinShotStart()
{
	GetWorldTimerManager().SetTimer(
		RJavelinRepeatTimer,
		this,
		&ABoss::RJavelinShot,
		0.4f,
		true);
}

void ABoss::RJavelinShot()
{
	// 오른쪽 어깨 미사일 Socket 위치 반환
	FString SocketName = FString::Printf(TEXT("MissileSocket_R%d"), RJavelinRepeatCount+1);
	FTransform SocketTransform = Mesh->GetSocketTransform(FName(*SocketName), ERelativeTransformSpace::RTS_World);

	// Homing Projectile 생성
	AHomingProjectile* HomingProjectile = GetWorld()->SpawnActor<AHomingProjectile>(HomingClass, SocketTransform.GetLocation(),FRotator::ZeroRotator);

	// 방향벡터 설정
	FVector LaunchDirection = FVector(0, -1, 1); 

	// 발사
	HomingProjectile->Fire(LaunchDirection);

	// 사운드 재생
	UGameplayStatics::PlaySoundAtLocation(this, JavelinSound, GetActorLocation());

	// 발사한 탄환 수 1개 증가
	++RJavelinRepeatCount;

	// 모두 발사한 경우 변수 및 타이머 초기화
	if (RJavelinRepeatCount >=  5)
	{
		RJavelinRepeatCount = 0;
		GetWorldTimerManager().ClearTimer(RJavelinRepeatTimer);
		SetNormalAttackTimer();
		bIsRJavelinShot = false;
	}
}


#pragma endregion

#pragma region Util
void ABoss::HandleDamageToPart(FName PartsName, float& Damage)
{
	if (!RegionNames.Contains(PartsName))
	{
		return;
	}
	
	// 해당 부위의 머티리얼 색상 변경
	ApplyPartsDamaged(PartsName);

	// 해당 부위의 Attack Count 증가
	if (RegionAttackCount.FindOrAdd(PartsName))
	{
		if (RegionAttackCount[PartsName] >= RegionDestroyCount)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "Destroyed");
			Damage += RegionDestroyDamage;
			DestroyRegion(PartsName);
		}
		else
		{
			++RegionAttackCount[PartsName];	
		}
	}
	else
	{
		RegionAttackCount[PartsName] = 1;
	}
	
}

void ABoss::HandleDamageToKneeItem(FName PartsName)
{
	if (!KneeItemNames.Contains(PartsName))
	{
		return;
	}
	
	// 해당 부위의 머티리얼 색상 변경
	ApplyPartsDamaged(PartsName);

	// 해당 부위의 Attack Count 증가
	if (RegionAttackCount.FindOrAdd(PartsName))
	{
		if (RegionAttackCount[PartsName] >= KneeitemDestroyCount)
		{
			DestroyRegion(PartsName);
			++KneeItemDestroyedCount;
		}
		else
		{
			++RegionAttackCount[PartsName];	
		}
	}
	else
	{
		RegionAttackCount[PartsName] = 1;
	}
	
	if (KneeItemDestroyedCount >= 6)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, "KneeItemDestroyedCount");
		BerserkType = EBossBerserkType::Transition;
		SetPatternTimer(1);
	}
}


float ABoss::GetDistanceToPlayer()
{
	if (Player == nullptr) return 0;
	
	// 보스, 플레이어의 거리를 반환
	FVector BossLocation = GetActorLocation();
	FVector PlayerLocation = Player->GetActorLocation();

	// 거리 계산
	float Dist = FVector::Dist(PlayerLocation, BossLocation);

	// 거리 값 반환
	return Dist;
}

FRotator ABoss::GetBoneRotation(FName BoneName)
{
	if (Mesh == nullptr || Player == nullptr) return FRotator();

	// 뼈의 위치 반환
	FVector BoneLocation = Mesh->GetBoneLocation(BoneName);

	// 플레이어 위치 반환
	FVector PlayerLocation = Player->GetActorLocation();

	// 회전 목표 방향벡터 반환
	FRotator BoneTargetRotation = (PlayerLocation - BoneLocation).GetSafeNormal().Rotation();
	
	return BoneTargetRotation;
}

void ABoss::IsInBusterBound(float& Distance)
{
	// 패턴 인식 거리보다 멀다면 return
	if (Distance > BusterPatternMinDistance)
	{
		// 시간 측정 중이었다면 Timer 초기화
		if (bIsBusterTimerTriggered)
		{
			GetWorldTimerManager().ClearTimer(BusterPatternTimer);
			bIsBusterTimerTriggered = false;
		}
		
	}
	else
	{
		// 패턴 인식 거리인 경우
		// 시간 측정 중이 아니었다면 Timer 가동
		if (!bIsBusterTimerTriggered)
		{
			GetWorldTimerManager().SetTimer(
				BusterPatternTimer,
				this,
				&ABoss::BusterPatternStart,
				BusterMinTime,
				false
				);

			bIsBusterTimerTriggered = true;
		}
	}
}

void ABoss::SetNormalAttackTimer()
{
	bIsAttacking = true;
	// 중복 활성화 방지
	GetWorldTimerManager().ClearTimer(NormalPatternTimer);
	
	// 일반 공격 패턴 간격에 NormalAttackDeviation 만큼 편차를 둔 난수 생성
	int PatternInterval = FMath::RandRange(
		NormalAttackPatternInterval - NormalAttackDeviation,
		NormalAttackPatternInterval + NormalAttackDeviation
		);
	
	// 공격 상태 해제
	GetWorldTimerManager().SetTimer(
		NormalPatternTimer,
		[this]()
		{
			bIsAttacking = false;
			Blackboard->SetValueAsBool(FName("IsAttacking"), bIsAttacking);
		},
		PatternInterval,
		false
		);
}

void ABoss::IsFirstGroggyTriggered()
{
	// 첫 번째 그로기가 출력된 경우 return
	if (FirstGroggyType == EBossGroggyType::Actioned) return;
	
	// 첫 번째 그로기가 트리거 됐고, 보스가 그로기가 가능한 상태인 경우
	if (FirstGroggyType == EBossGroggyType::Triggered && bIsAttacking)
	{
		bIsGroggy = true;
		FirstGroggyType = EBossGroggyType::Actioned;
	}

	// 현재 쉴드량이 최대 쉴드량의 절반 이하인지 확인
	if (Status.Shield <= Status.MaxShield / 2)
	{
		if (FirstGroggyType == EBossGroggyType::Default)
		{
			FirstGroggyType = EBossGroggyType::Triggered;
		}
	}
}

void ABoss::IsSecondGroggyTriggered()
{
	// 첫 번째 그로기가 출력된 적이 없거나, 두 번째 그로기가 출력된 적이 있는 경우 return
	if (FirstGroggyType != EBossGroggyType::Actioned ||
		SecondGroggyType == EBossGroggyType::Actioned) return;
	
	// 두 번째 그로기가 트리거 됐고, 보스가 그로기가 가능한 상태인 경우
	if (SecondGroggyType == EBossGroggyType::Triggered && bIsAttacking)
	{
		bIsGroggy = true;
		SecondGroggyType = EBossGroggyType::Actioned;
	}

	// 현재 쉴드량이 0 이하인지 확인
	if (Status.Shield <= 0)
	{
		if (SecondGroggyType == EBossGroggyType::Default)
		{
			SecondGroggyType = EBossGroggyType::Triggered;
		}
	}
}

void ABoss::IsCurrentHealthZero()
{
	if (Status.Health <= 0)
	{
		OnDeath();
		AGameStateBase* GameState = UGameplayStatics::GetGameState(GetWorld());
		AMainGameStateBase* MainGameState = Cast<AMainGameStateBase>(GameState);
		MainGameState->OnBossLevelEnded();
	}
}

void ABoss::IsBerserkTriggered()
{
	// 버서커 패턴이 발동된 적이 있다면 return
	if (BerserkType != EBossBerserkType::Default) return;
	
	// 현재 체력이 버서커 패턴이 발동되는 체력보다 낮은 지 확인
	if (Status.Health <= BerserkMinHealth)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Berserk");
		// 버서커 패턴 발동 가능 상태로 전이
		BerserkType = EBossBerserkType::Triggered;

		GetWorldTimerManager().ClearTimer(SummonPatternTimer);
		GetWorldTimerManager().ClearTimer(FlamePatternTimer);
		
		// 소환 타이머 가동
		GetWorldTimerManager().SetTimer(
			SummonPatternTimer,
			this,
			&ABoss::SummonPatternStart,
			SummonPatternInterval/2,
			true,
			SummonPatternInterval/2);

		// 플레임 타이머 가동
		GetWorldTimerManager().SetTimer(
			FlamePatternTimer,
			this,
			&ABoss::FlamePatternStart,
			FlameExplosionPatternInterval,
			true,
			FlameExplosionPatternInterval/2);
	}
}

void ABoss::RotationToTargetWithAllBones(float DeltaSeconds)
{
	// 보스와 플레이어의 현재 위치를 가져옴
	FVector BossLocation = GetActorLocation();
	FVector PlayerLocation = Player->GetActorLocation();

	// 정규화된 방향 벡터
	FVector Direction = (PlayerLocation - BossLocation).GetSafeNormal();

	// 목표 회전 각도 계산 (Yaw만 변경)
	FRotator TargetRotation = Direction.Rotation();
	TargetRotation.Pitch = 0;

	// 현재 회전 값 가져오기
	FRotator CurrentRotation = GetActorRotation();

	// 일정 속도로 회전
	FRotator NewRotation = FMath::RInterpConstantTo(CurrentRotation, TargetRotation, DeltaSeconds, RotationSpeed);

	// 보스 회전 적용
	SetActorRotation(NewRotation);
}


void ABoss::IsBerserkTransitionPossible()
{
	// 그로기 상태이거나 공격 중이라면 return
	if (bIsGroggy || !bIsAttacking) return;

	// 버서커 패턴 발동 가능 상태인지 확인
	if (BerserkType == EBossBerserkType::Triggered)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, "Berserk Transition");

		// 버서커 애니메이션 재생 상태로 전이
		BerserkType = EBossBerserkType::Transition;
		SetPatternTimer(PatternAdjustmentValue);
	}
}

void ABoss::SetPatternTimer(int32 AdjustmentValue)
{

	GetWorldTimerManager().ClearTimer(SummonPatternTimer);
	GetWorldTimerManager().ClearTimer(FlamePatternTimer);

	// 소환 타이머 가동
	GetWorldTimerManager().SetTimer(
		SummonPatternTimer,
		this,
		&ABoss::SummonPatternStart,
		SummonPatternInterval / AdjustmentValue,
		true,
		SummonPatternInterval / AdjustmentValue);

	// 플레임 타이머 가동
	GetWorldTimerManager().SetTimer(
		FlamePatternTimer,
		this,
		&ABoss::FlamePatternStart,
		FlameExplosionPatternInterval / AdjustmentValue,
		true,
		FlameExplosionPatternInterval/ AdjustmentValue);
}



#pragma endregion

#pragma region InitMovementState Functions
void ABoss::InitMovementStateToIdle()
{
	// Idle로 초기화
	MovementState = EBossMovementState::Idle;

	// Idle Timer 초기화
	GetWorldTimerManager().SetTimer(
		IdleTimer,
		this,
		&ABoss::InitMovementStateToMove,
		IdleTime,
		false
		);
}

void ABoss::InitMovementStateToMove()
{
	// 보스, 플레이어 사이의 거리 계산
	float Distance = GetDistanceToPlayer();

	// 현재 거리에 따라 상태 초기화
	if (Distance < MinRadius)
	{
		MovementState = EBossMovementState::BackMoving;
	}
	else if (MinRadius <= Distance && Distance <= MaxRadius)
	{
		MovementState = EBossMovementState::Surrounding;
	}
	else
	{
		MovementState = EBossMovementState::Approaching;
	}
	
	// Move Timer 초기화
	GetWorldTimerManager().SetTimer(
		MoveTimer,
		this,
		&ABoss::InitMovementStateToIdle,
		MoveTime,
		false
		);
}

void ABoss::InitBlackboardMovementFlag(const EBossMovementState State)
{
	// blackboard의 이동 상태 관련 변수 모두 false로 초기화
	Blackboard->SetValueAsBool(FName("IsApproaching"), false);
	Blackboard->SetValueAsBool(FName("IsBackMoving"), false);
	Blackboard->SetValueAsBool(FName("IsSurrounding"), false);
	Blackboard->SetValueAsBool(FName("IsMoving"), true);
	
	// 현재 이동 상태에 대한 변수만 true로 초기화
	switch (State)
	{
		case EBossMovementState::Approaching:
			Blackboard->SetValueAsBool(FName("IsApproaching"), true);
			break;
		case EBossMovementState::Surrounding:
			Blackboard->SetValueAsBool(FName("IsSurrounding"), true);
			break;
		case EBossMovementState::BackMoving:
			Blackboard->SetValueAsBool(FName("IsBackMoving"), true);
			break;
		case EBossMovementState::Idle:
			Blackboard->SetValueAsBool(FName("IsMoving"), false);
			break;
	}

}


void ABoss::SetMoveState()
{
	// 보스, 플레이어 사이의 거리 계산
	float Distance = GetDistanceToPlayer();
	IsInBusterBound(Distance);

	switch (MovementState)
	{
		// (1) 플레이어에게 전진하는 상태
	case EBossMovementState::Approaching:
		if (Distance < ApproachAcceptance)
		{
			MovementState = EBossMovementState::Surrounding;
		}
		break;

		// (2) 원을 그리며 이동하는 상태
	case EBossMovementState::Surrounding:
		if (Distance > MaxRadius)
		{
			MovementState = EBossMovementState::Approaching;
		}
		else if (Distance < MinRadius)
		{
			MovementState = EBossMovementState::BackMoving;
		}
		break;

		// (3) 후진하는 상태
	case EBossMovementState::BackMoving:
		if (Distance > BackMovingAcceptance)
		{
			MovementState = EBossMovementState::Surrounding;
		}
		break;

		// (4) Idle 상태
	case EBossMovementState::Idle:
		break;
	}
	
	// blackboard 값 초기화
	InitBlackboardMovementFlag(MovementState);
}
#pragma endregion
