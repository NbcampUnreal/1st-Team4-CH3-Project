#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CrossHairWidget.generated.h"

class UImage;

UCLASS()
class THEFOURTHDESCENDANT_API UCrossHairWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Control")
	float ShotPower;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Control")
	float MaximumRemainTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Control")
	float RemainMovingTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Control")
	float MoveToOriginTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Control")
	float MaximumMoveDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move Control")
	float CurrentMoveDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component | Image")
	TObjectPtr<UImage> CrossHair_L;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component | Image")
	TObjectPtr<UImage> CrossHair_R;

public:
	UFUNCTION(BlueprintCallable, Category = "Move Control")
	void Shoot();

private:
	UFUNCTION()
	void IncreaseMovingTime();
	UFUNCTION()
	void MoveCrossHair();	
};
