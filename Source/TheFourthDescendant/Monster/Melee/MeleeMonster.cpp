// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeMonster.h"

#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"


AMeleeMonster::AMeleeMonster()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// 공격 범위 컴포넌트 생성
	AttackRangeComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("AttackRangeComponent"));
	AttackRangeComponent->SetupAttachment(RootComponent);

	// 공격 범위 크기 및 위치 설정
	AttackRangeComponent->InitCapsuleSize(90, 90);
	AttackRangeComponent->SetRelativeLocation(FVector(120.0f, 0.0f, 0.0f));

	// 충돌만 허용
	AttackRangeComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	// 오버랩 채널로 설정
	AttackRangeComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackRangeComponent->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);

	// 오버랩 이벤트 바인딩
	AttackRangeComponent->OnComponentBeginOverlap.AddDynamic(this, &AMeleeMonster::OnAttackRangeOverlap);
}



void AMeleeMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead) return;

	if (bCanAttack && !bIsAttacked)
	{
		// 타겟과 나의 위치
		const FVector CurrentPos = GetActorLocation();
		const FVector TargetPos = Player->GetActorLocation();

		// 방향 벡터 계산
		const FVector Direction = (TargetPos - CurrentPos).GetSafeNormal();

		// 회전 벡터 계산
		const FRotator CurrentRotation = GetActorRotation();
		const FRotator TargetRotation = FRotationMatrix::MakeFromX(Direction).Rotator();

		// 회전 값 보간
		const FRotator NewRotation = FMath::RInterpTo(CurrentRotation, TargetRotation, DeltaTime, 5.0f);

		// 회전 적용
		SetActorRotation(NewRotation);
	}
}



void AMeleeMonster::Attack()
{
	// 공격 로직이 한 번 실행됐을 경우 return
	if (bIsAttacked) return;

	// 점프 중일 경우 return
	if (bIsJumpStarted || bIsJumping || bIsJumpEnded) return;
	
	bIsAttacked = true;

	// 게임 인스턴스 캐스팅
	UGameInstance* GameInstance = GetWorld()->GetGameInstance();
	UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GameInstance);

	// 회피한 횟수 1 증가
	MainGameInstance->AddEvasionAttackCount(1);

	// 공격 후 콜리전 비활성화
	AttackRangeComponent->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
}



void AMeleeMonster::Move()
{
	if (bIsJumpStarted || bIsJumping || bIsJumpEnded) return;
	
	if (EnemyController->bIsArrived)
	{
		bCanAttack = true;
		Blackboard->SetValueAsBool(FName("IsAttacking"), true);
		return;
	}
	EnemyController->MoveToTargetActor(Player);
}



void AMeleeMonster::AttackCompleted()
{
	bCanAttack = false;
	bIsAttacked = false;
	Blackboard->SetValueAsBool(FName("IsAttacking"), false);
	EnemyController->bIsArrived = false;

	// 공격 후 콜리전 비활성화
	AttackRangeComponent->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}



void AMeleeMonster::OnAttackRangeOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Enemy Attacked !")));

		// 게임 인스턴스 캐스팅
		UGameInstance* GameInstance = GetWorld()->GetGameInstance();
		UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GameInstance);
		
		// 게임 인스턴스에 플레이어에게 가한 데미지 최신화
		MainGameInstance->AddReceivedDamageByEnemy(AttackPower);

		// 회피한 횟수 1 감소
		MainGameInstance->SubtractEvasionAttackCount(1);
	
		// 플레이어에게 데미지 적용
		UGameplayStatics::ApplyDamage(Player, AttackPower, EnemyController, this, UDamageType::StaticClass());
	}
}