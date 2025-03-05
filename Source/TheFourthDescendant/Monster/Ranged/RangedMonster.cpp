// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedMonster.h"

#include "EngineUtils.h"
#include "TheFourthDescendant/GameManager/MainGameInstance.h"
#include "TheFourthDescendant/Monster/Projectile/EnemyProjectile.h"

ARangedMonster::ARangedMonster()
{
	PrimaryActorTick.bCanEverTick = true;
	
	RangedAttackCount = 0;
	CurrentRangedAttackCount = 0;
	bIsReloading = false;
}

void ARangedMonster::BeginPlay()
{
	Super::BeginPlay();

	// 탄환 수 초기화
	CurrentRangedAttackCount = RangedAttackCount;
	// 소켓 추적용 메시 컴포넌트 초기화
	SkeletalMesh = this->GetMesh();

	// 부모 클래스에 해당하는 액터는 모두 무시
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor->IsA(AMonster::StaticClass()))
		{
			Params.AddIgnoredActor(Actor);
		}
	}
}

void ARangedMonster::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsDead) return; // 사망 상태일시 return

	if (!Player) return; // 플레이어가 없는 경우 예외 처리

	const FVector CurrentLocation = GetActorLocation();
	const FVector TargetLocation = Player->GetActorLocation();

	// 타겟을 향한 방향 벡터
	FVector Direction = (TargetLocation - CurrentLocation);
	Direction.Z = 0; // Z축 변경 방지 (수평 회전만 수행)
	const FRotator LookAtRotation = Direction.Rotation(); // 타겟을 바라보는 회전값

	// 부드러운 회전
	const float RotationSpeed = 5.0f; // 회전 속도 조절
	const FRotator InterpRotation = FMath::RInterpTo(GetActorRotation(), LookAtRotation, DeltaTime, RotationSpeed);

	// 회전 적용
	SetActorRotation(InterpRotation);
}

void ARangedMonster::OnDeath()
{
	DestroyWidget();
	Super::OnDeath();
}




void ARangedMonster::Attack()
{
	// 잔탄이 0발 이하인 경우 return
	if (CurrentRangedAttackCount <= 0)
	{
		bCanAttack = false;
		bIsReloading = true;
		Blackboard->SetValueAsBool(FName("IsReloading"), true);
		return;
	}
	
	// 총구 위치
	const FVector MuzzleLocation = SkeletalMesh->GetSocketLocation(FName("MuzzleSocket"));
	// 플레이어 위치
	const FVector PlayerLocation = Player->GetActorLocation();

	FHitResult HitResult;

	// 총구와 플레이어 사이에 물체가 있는지 확인
	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, MuzzleLocation, PlayerLocation, ECC_Visibility, Params);

	// LineTracing 추적 디버그용 
	//DrawDebugLine(GetWorld(), MuzzleLocation, PlayerLocation, FColor::Red, false, 1, 0, 1.0f);
	
	if (bHit)
	{
		// 플레이어가 적중한 경우 데미지 로직 작성
		if (HitResult.GetActor() == Player)
		{
			// 공격 가능한 상태로 전환
			bCanAttack = true;
		}
	}

	// 공격한 상태라면 탄환 수 감소
	if (bCanAttack)
	{
		--CurrentRangedAttackCount;
		const FVector FireDirection = PlayerLocation - MuzzleLocation;
		AEnemyProjectile* Projectile = GetWorld()->SpawnActor<AEnemyProjectile>(ProjectileClass, MuzzleLocation, FRotator::ZeroRotator);
		Projectile->FireInDirection(FireDirection.GetSafeNormal());

		// 회피한 공격 수 증가
		UGameInstance* GameInstance = GetWorld()->GetGameInstance();
		UMainGameInstance* MainGameInstance = Cast<UMainGameInstance>(GameInstance);
		MainGameInstance->AddEvasionAttackCount(1);
	}
}

void ARangedMonster::Move()
{
	EnemyController->MoveToTargetActor(Player);
}

void ARangedMonster::ReloadCompleted()
{
	// 탄환 수 초기화
	CurrentRangedAttackCount = RangedAttackCount;
	// 장전 상태 플래그 비활성화
	bIsReloading = false;
	// AI의 장전 상태 플래그 비활성화
	Blackboard->SetValueAsBool(FName("IsReloading"), false);
}

