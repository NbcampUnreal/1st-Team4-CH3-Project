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
	AttackPower = 0;
	bCanAttack = false;
	bIsSpawned = false;
	bIsDead = false;
	MinRadius = 200;
	BackMovingAcceptance = 400;
	ApproachAcceptance = 600;
	MaxRadius = 800;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
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

	// 캐릭터 이동속도 초기화
	GetCharacterMovement()->MaxWalkSpeed = Status.WalkSpeed;

	// 체력 및 쉴드 초기화
	Status.Health = Status.MaxHealth;
	Status.Shield = Status.MaxShield;
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
		DirectionVector = -GetActorRightVector();
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
			break;
	}

}



void ABoss::SetMoveState()
{
	// 보스, 플레이어 사이의 거리 계산
	float Distance = GetDistanceToPlayer();
	
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
