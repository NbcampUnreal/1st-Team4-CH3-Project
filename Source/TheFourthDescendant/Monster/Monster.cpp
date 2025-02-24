// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameState.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"
#include "TheFourthDescendant/GameManager/MainGameStateBase.h"

AMonster::AMonster()
{
	AttackPower = 0;
	bCanAttack = false;
	bIsSpawned = false;
	bIsDead = false;

	Player = nullptr;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AMonster::BeginPlay()
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
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Monster BeginPlay : Character Casting Failed !");
	}

	// AIController 캐스팅
	EnemyController = Cast<AEnemyController>(GetController());
	if (EnemyController == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Monster BeginPlay : AIController Casting Failed !");
	}

	// 블랙보드 할당
	Blackboard = EnemyController->GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Monster BeginPlay : BlackBoard Casting Failed !");
	}

	Blackboard->SetValueAsObject(FName("TargetActor"), Player);
}

float AMonster::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (Status.Health <= 0)
	{
		OnDeath();
	}

	return ActualDamage;
}


void AMonster::Attack()
{
}

void AMonster::Move()
{
	
}

void AMonster::OnDeath()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, "Monster OnDeath");

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

	// GameState에서 Enemy 처치시 호출하는 콜백 함수
	AGameStateBase* StateBase = UGameplayStatics::GetGameState(GetWorld());
	AMainGameStateBase* MainState = Cast<AMainGameStateBase>(StateBase);
	MainState->OnEnemyKilled();
}
