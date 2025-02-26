#include "Boss.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/AI/EnemyController/EnemyController.h"
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
	BackmovingAcceptance = 400;
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
	EnemyController = Cast<AEnemyController>(GetController());
	if (EnemyController == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Boss BeginPlay : AIController Casting Failed !");
	}

	// 블랙보드 할당
	Blackboard = EnemyController->GetBlackboardComponent();
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


void ABoss::Move()
{
	// (1) 전진하는 상태

	// (2) 원을 그리며 이동하는 상태

	// (3) 후진하는 상태

	SetMoveState();
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
	float Distance = FVector::Dist(PlayerLocation, BossLocation);

	// 거리 값 반환
	return Distance;
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
				MovementState = EBossMovementState::Backmoving;
			}
			break;

		// (3) 후진하는 상태
		case EBossMovementState::Backmoving:
			if (Distance > BackmovingAcceptance)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Boss Changed Move State To Surrounding !");
				MovementState = EBossMovementState::Surrounding;
			}
			break;
	}
}
