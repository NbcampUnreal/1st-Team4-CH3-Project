#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MoveToTarget.generated.h"


class ABoss;

UCLASS()
class THEFOURTHDESCENDANT_API UMoveToTarget : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMoveToTarget();

private:
	ABoss* Boss;

protected:
	/** Task가 실행되기 시작할 때 호출되는 함수 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
