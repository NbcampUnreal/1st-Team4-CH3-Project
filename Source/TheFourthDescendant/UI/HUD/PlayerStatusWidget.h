
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusWidget.generated.h"

UCLASS()
class THEFOURTHDESCENDANT_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void NativeOnInitialized() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	int32 BulletInMag;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bullet")
	int32 TotalMagsBullet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge")
	float HPPercent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gauge")
	float SPPercent;

public:
	UFUNCTION(BlueprintCallable, Category = "Setting Bullet Infos | InMagazine")
	void IncreaseBulletInMag();
	UFUNCTION(BlueprintCallable, Category = "Setting Bullet Infos | InMagazine")
	void DecreaseBulletInMag();
	// 원하는 숫자만큼 현재 탄알집에 총알을 설정해준다.
	UFUNCTION(BlueprintCallable, Category = "Setting Bullet Infos | InMagazine")
	void SettingBulletInMag(int32 NewBulletCountInMag);

	// 원하는 숫자만큼 현재 탄알집에 있는 총알을 제외하고 총알을 위젯에 설정해준다.
	UFUNCTION(BlueprintCallable, Category = "Setting Bullet Infos | OtherTotal")
	void SettingTotalMagsBullet(int32 NewTotalMagsBulletCount);

	// 편의성 함수, 전체 총알 수 중에서 원하는 만큼을 차감해서 탄알집에 넣어주는 효과
	UFUNCTION(BlueprintCallable, Category = "Setting Bullet Infos | Actions")
	void Reload(int32 DesireMagBulletCount);

	// Health Point ProgressBar의 퍼센트 설정, CurrentHP와 MaxHP를 입력해주면 내부에서 처리하여 변수에 대입해준다.
	UFUNCTION(BlueprintCallable, Category = "Setting ProgressBar")
	void SetHPPercentTwoParams(float CurrentHP, float MaxHP);
	// Health Point ProgressBar의 퍼센트 설정, Percent를 단독으로 인자로 사용한다.
	UFUNCTION(BlueprintCallable, Category = "Setting ProgressBar")
	void SetHPPercentOneParams(float Percent);
	UFUNCTION(BlueprintCallable, Category = "Setting ProgressBar")
	float GetHPPercent() const;

	// Shield Point ProgressBar의 퍼센트 설정, CurrentSP와 MaxSP를 입력해주면 내부에서 처리하여 변수에 대입해준다.
	UFUNCTION(BlueprintCallable, Category = "Setting ProgressBar")
	void SetSPPercentTwoParams(float CurrentSP, float MaxSP);
	// Shield Point ProgressBar의 퍼센트 설정, Percent를 단독으로 인자로 사용한다.
	UFUNCTION(BlueprintCallable, Category = "Setting ProgressBar")
	void SetSPPercentOneParams(float Percent);
	UFUNCTION(BlueprintCallable, Category = "Setting ProgressBar")
	float GetSPPercent() const;	
};
