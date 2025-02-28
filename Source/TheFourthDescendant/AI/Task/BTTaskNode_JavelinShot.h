#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_JavelinShot.generated.h"

class ABoss;

UCLASS()
class THEFOURTHDESCENDANT_API UBTTaskNode_JavelinShot : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTaskNode_JavelinShot();

private:
	ABoss* Boss;

protected:
	/** Task가 실행되기 시작할 때 호출되는 함수 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
