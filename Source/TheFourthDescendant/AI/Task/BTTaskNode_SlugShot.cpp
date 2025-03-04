#include "BTTaskNode_SlugShot.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TheFourthDescendant/Monster/Boss/Boss.h"

UBTTaskNode_SlugShot::UBTTaskNode_SlugShot()
{
	Boss = nullptr;
	NodeName = "SlugShot";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_SlugShot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Boss = Cast<ABoss>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Boss == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "SlugShot : Boss Casting Failed !");
		return EBTNodeResult::Failed;
	}

	// 보스의 SlugShot 패턴 활성화
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "SlugShot !!");
	Boss->bIsLSlugShot = true;
	Boss->bIsRSlugShot = true;
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "SlugShot : Blackboard Casting Failed !");
		return EBTNodeResult::Failed;
	}

	// 보스가 공격 중임을 알림
	Blackboard->SetValueAsBool(FName("IsAttacking"), true);
	
	return EBTNodeResult::Succeeded;
}


