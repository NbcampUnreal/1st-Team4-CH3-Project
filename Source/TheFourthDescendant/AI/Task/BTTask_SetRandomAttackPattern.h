#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_SetRandomAttackPattern.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API UBTTask_SetRandomAttackPattern : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_SetRandomAttackPattern();

protected:
	/** Task가 실행되기 시작할 때 호출되는 함수 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
