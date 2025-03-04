#include "BTTaskNode_JavelinShot.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "TheFourthDescendant/Monster/Boss/Boss.h"

UBTTaskNode_JavelinShot::UBTTaskNode_JavelinShot()
{
	Boss = nullptr;
	NodeName = "JavelinShot";
	bNotifyTick = true;
}

EBTNodeResult::Type UBTTaskNode_JavelinShot::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	Boss = Cast<ABoss>(OwnerComp.GetAIOwner()->GetCharacter());
	if (Boss == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Javelin : Boss Casting Failed !");
		return EBTNodeResult::Failed;
	}

	// 보스의 SlugShot 패턴 활성화
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "JavelinShot !!");
	Boss->bIsLJavelinShot = true;
	Boss->bIsRJavelinShot = true;
	
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Javelin : Blackboard Casting Failed !");
		return EBTNodeResult::Failed;
	}

	if (Boss->bIsLJavelinShot)
	{
		Boss->LJavelinShotStart();
	}

	if (Boss->bIsRJavelinShot)
	{
		Boss->RJavelinShotStart();
	}

	// 보스가 공격 중임을 알림
	Blackboard->SetValueAsBool(FName("IsAttacking"), true);
	
	return EBTNodeResult::Succeeded;
}
