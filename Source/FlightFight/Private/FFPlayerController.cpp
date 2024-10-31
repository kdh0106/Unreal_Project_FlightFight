// Fill out your copyright notice in the Description page of Project Settings.


#include "FFPlayerController.h"
#include "FFGameMode.h"
#include "Blueprint/UserWidget.h"
#include "FFHUDWidget.h"
#include "FFPawn.h"
#include "FFPlayerState.h"
#include "Kismet/GameplayStatics.h"

AFFPlayerController::AFFPlayerController()
{
	CurrentScore = 0;
}

void AFFPlayerController::BeginPlay()
{ 
	Super::BeginPlay();
	 
	FInputModeGameOnly InputMode;	
	SetInputMode(InputMode);

	PlayerCameraManager->ViewPitchMax = 179.99f;
	PlayerCameraManager->ViewPitchMin = -179.99f;  
	PlayerCameraManager->ViewRollMin = -179.99f;
	PlayerCameraManager->ViewRollMax = 179.99f;
	
	CurrentScore = 0;

	if (IsLocalPlayerController())
	{
		Client_CreateHUD();
	} 
}

void AFFPlayerController::Client_CreateHUD_Implementation()
{	
	if (IsLocalPlayerController())
	{
		if (!HUDWidget)
		{
			TSubclassOf<UFFHUDWidget> HUDWidgetClass = LoadClass<UFFHUDWidget>(nullptr, TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
			if (HUDWidgetClass)
			{
				HUDWidget = CreateWidget<UFFHUDWidget>(this, HUDWidgetClass);
				if (HUDWidget)
				{
					HUDWidget->AddToViewport();
					UpdateScoreDisplay();
				}
			}
		}
	}
}

void AFFPlayerController::UpdateScoreDisplay()
{
	if (IsLocalPlayerController() && HUDWidget)
	{
		HUDWidget->UpdateScore(CurrentScore);
	}
}

void AFFPlayerController::AddScore()
{
	CurrentScore++;
	UpdateScoreDisplay();
	//ABLOG(Warning, TEXT("%d is CurrentScore, %s is playercontroller"), CurrentScore, *this->GetName());
}

void AFFPlayerController::NotifyEnemyKilled()
{
	if (IsLocalPlayerController())
	{
		AddScore();
	}
	//ABLOG(Warning, TEXT("%d is CurrentScore, %s is playercontroller"), CurrentScore, *this->GetName());
}

void AFFPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}


void AFFPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFFPlayerController::Client_NotifyScoreAdd_Implementation()
{
	AddScore();
}

//void AFFPlayerController::CreateHUDWidget_Implementation()
//{
//	if (AFFGameMode* GameMode = Cast<AFFGameMode>(GetWorld()->GetAuthGameMode()))
//	{
//		if (GameMode->HUDWidgetClass)
//		{
//			HUDWidget = CreateWidget<UFFHUDWidget>(this, GameMode->HUDWidgetClass);
//			if (HUDWidget)
//			{
//				HUDWidget->AddToViewport();
//			}
//		}
//	}
//}