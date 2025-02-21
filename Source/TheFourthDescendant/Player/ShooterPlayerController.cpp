// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/UserWidget.h"

AShooterPlayerController::AShooterPlayerController()
{
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// 호출 순서
	// APlayerCharacter::SetupPlayerInputComponent
	// AShooterPlayerController::BeginPlay
	// APlayerCharacter::BeginPlay
	// 이 시점에는 이미 Pawn이 존재하므로 연결이 가능하다.
	UE_LOG(LogTemp, Warning, TEXT("AShooterPlayerController::BeginPlay"));
	
	if (ULocalPlayer* LocalPlayer = GetLocalPlayer();
		UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
	{
		if (InputMappingContext)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}

	if (HUDWidgetClass)
	{
		HUDWidget = CreateWidget<UUserWidget>(this, HUDWidgetClass);
		if (HUDWidget)
		{
			HUDWidget->AddToViewport();
		}
	}
}
