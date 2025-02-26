// Fill out your copyright notice in the Description page of Project Settings.


#include "CrossHairWidget.h"

#include "Components/Image.h"

void UCrossHairWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	CrossHair_L = Cast<UImage>(GetWidgetFromName(TEXT("CrossHair_L")));
	ensureMsgf(IsValid(CrossHair_L), TEXT("크로스헤어 왼쪽을 찾을 수 없음."));
	CrossHair_L->SetRenderTransformAngle(0.f);

	CrossHair_R = Cast<UImage>(GetWidgetFromName(TEXT("CrossHair_R")));
	ensureMsgf(IsValid(CrossHair_R), TEXT("크로스헤어 오른쪽을 찾을 수 없음."));	
	CrossHair_R->SetRenderTransformAngle(180.f);

	if(GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(UpdateTimer, this, &UCrossHairWidget::MoveCrossHair, 0.01f, true);
	}
}

void UCrossHairWidget::Shoot()
{
	IncreaseMovingTime();
}

void UCrossHairWidget::IncreaseMovingTime()
{
	RemainMovingTime += ShotPower;
	if (RemainMovingTime > MaximumRemainTime)
	{
		RemainMovingTime = MaximumRemainTime;
	}
}

void UCrossHairWidget::MoveCrossHair()
{
	if(RemainMovingTime <= 0)
	{
		return;
	}

	checkf(0 != MaximumRemainTime, TEXT("MaximumRemainTime은 0이 될 수 없습니다."));
	
	float MoveAlpha = RemainMovingTime / MaximumRemainTime;
	CurrentMoveDistance = FMath::Lerp(0.f, MaximumMoveDistance, MoveAlpha);

	CrossHair_L->SetRenderTranslation(FVector2D(-CurrentMoveDistance, 0.f));
	CrossHair_R->SetRenderTranslation(FVector2D(CurrentMoveDistance, 0.f));
	
	RemainMovingTime -= MoveToOriginTime;
}