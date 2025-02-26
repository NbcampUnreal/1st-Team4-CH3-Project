#include "Boss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/AI/EnemyController/BossController.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"
#include "TheFourthDescendant/Player/PlayerCharacter.h"

ABoss::ABoss()
{
	AttackPower = 0;
	bCanAttack = false;
	bIsSpawned = false;
	bIsDead = false;
	Player = nullptr;
	MinRadius = 200;
	BackMovingAcceptance = 400;
	ApproachAcceptance = 600;
	MaxRadius = 800;
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}



void ABoss::BeginPlay()
{
	Super::BeginPlay();

	// 캐릭터 이동속도 초기화
	GetCharacterMovement()->MaxWalkSpeed = Status.WalkSpeed;

	// 체력 및 쉴드 초기화
	Status.Health = Status.MaxHealth;
	Status.Shield = Status.MaxShield;

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
}



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


void ABoss::MoveToTarget()
{
	BossController->MoveToTargetActor(Player);
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



void ABoss::SetMoveState()
{
	// 보스, 플레이어 사이의 거리 계산
	float Distance = GetDistanceToPlayer();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Distance: %f"), Distance));
	
	switch (MovementState)
	{
		// (1) 플레이어에게 전진하는 상태
		case EBossMovementState::Approaching:
			if (Distance < ApproachAcceptance)
			{
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
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Boss Changed Move State To Backmoving !");
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
		case EBossMovementState::Idle:
			break;
	}

	// blackboard 값 초기화
	InitBlackboardMovementFlag(MovementState);
}

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
