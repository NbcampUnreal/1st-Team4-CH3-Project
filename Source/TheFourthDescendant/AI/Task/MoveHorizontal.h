#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MoveHorizontal.generated.h"

class ABoss;

UCLASS()
class THEFOURTHDESCENDANT_API UMoveHorizontal : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMoveHorizontal();

private:
	ABoss* Boss;

	/** 왼쪽, 오른쪽 중 움직일 방향.
	 * 0 : 왼쪽
	 * 1 : 오른쪽
	 */
	int32 DirectionIndex;

protected:
	/** Task가 실행되기 시작할 때 호출되는 함수 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	
};
