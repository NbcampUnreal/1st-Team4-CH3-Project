#include "Boss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/AI/EnemyController/BossController.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"
#include "TheFourthDescendant/Player/PlayerCharacter.h"

#pragma region InitComponent
ABoss::ABoss()
{
	bCanAttack = false;
	bIsSpawned = false;
	bIsDead = false;
	bIsSummon = false;
	bIsFlame = false;
	bIsBuster = false;
	bIsBusterTimerTriggered = false;
	AttackPower = 0;
	MinRadius = 200;
	BackMovingAcceptance = 400;
	ApproachAcceptance = 600;
	MaxRadius = 800;
	SummonPatternInterval = 70;
	FlameExplosionPatternInterval = 90;
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
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	MovementState = EBossMovementState::Idle;
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

	// 소환 타이머 가동
	GetWorldTimerManager().SetTimer(
		SummonPatternTimer,
		this,
		&ABoss::SummonPatternStart,
		SummonPatternInterval,
		true,
		SummonPatternInterval);

	// 플레임 타이머 가동
	GetWorldTimerManager().SetTimer(
		FlamePatternTimer,
		this,
		&ABoss::FlamePatternStart,
		FlameExplosionPatternInterval,
		true,
		FlameExplosionPatternInterval);
}
#pragma endregion

#pragma region Damage, Death
float ABoss::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
                        class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	// @To-do 쉴드 50%, 0%, 체력 75% 때 그로기 패턴 추가
	if (Status.Health <= 0)
	{
		OnDeath();
	}

	return ActualDamage;
}



void ABoss::OnDeath()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Boss OnDeath");

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

	// 플레이어 방향으로 전방이동
	BossController->MoveToTargetActor(Player);
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

	AddMovementInput(DirectionVector);
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
	
	// 거리에 따라 보스 이동 모드 설정
	SetMoveState();
}
#pragma endregion

#pragma region Special Attack Pattern

void ABoss::SummonPatternStart()
{
	// AI 작동 정지
	if (BossController == nullptr) return;
	BossController->StopMovement();
	
	// Blackboard 값 초기화
	bIsSummon = true;
	Blackboard->SetValueAsBool(FName("IsSummon"), bIsSummon);
}

void ABoss::SummonMinions()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Emerald, "Summoning Minions");
}



void ABoss::FlamePatternStart()
{
	// AI 작동 정지
	if (BossController == nullptr) return;
	BossController->StopMovement();
	
	// Blackboard 값 초기화
	bIsFlame = true;
	Blackboard->SetValueAsBool(FName("IsFlame"), bIsFlame);
}

void ABoss::FlameExplosion()
{
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Emerald, "Flame Explosion");
}



void ABoss::BusterPatternStart()
{
	// AI 작동 정지
	if (BossController == nullptr) return;
	BossController->StopMovement();
	
	// Blackboard 값 초기화
	bIsBuster = true;
	Blackboard->SetValueAsBool(FName("IsBuster"), bIsBuster);

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

void ABoss::SlugShot()
{
	// 대포 발사하는 로직
}

#pragma endregion

#pragma region Util
float ABoss::GetDistanceToPlayer()
{
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
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Timer Init");
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
			GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Timer Start");
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
	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Red, "Normal Start");

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
			Blackboard->SetValueAsBool(FName("IsAttacking"), false);
		},
		PatternInterval,
		false
		);
}
#pragma endregion

#pragma region InitMovementState Functions
void ABoss::InitMovementStateToIdle()
{
	// Idle로 초기화
	MovementState = EBossMovementState::Idle;

	// AI 작동 정지
	if (BossController == nullptr) return;
	BossController->StopMovement();

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
	//GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Emerald, FString::Printf(TEXT("Distance %f"), Distance));
	
	switch (MovementState)
	{
		// (1) 플레이어에게 전진하는 상태
	case EBossMovementState::Approaching:
		if (Distance < ApproachAcceptance)
		{
			// AI 작동 정지
			if (BossController == nullptr) return;
			BossController->StopMovement();
			
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Boss Changed Move State To Surrounding !");
			MovementState = EBossMovementState::Surrounding;
		}
		break;

		// (2) 원을 그리며 이동하는 상태
	case EBossMovementState::Surrounding:
		if (Distance > MaxRadius)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Boss Changed Move State To Approaching !");
			MovementState = EBossMovementState::Approaching;
		}
		else if (Distance < MinRadius)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Boss Changed Move State To BackMoving !");
			MovementState = EBossMovementState::BackMoving;
		}
		break;

		// (3) 후진하는 상태
	case EBossMovementState::BackMoving:
		if (Distance > BackMovingAcceptance)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Boss Changed Move State To Surrounding !");
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
