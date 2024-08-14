// Fill out your copyright notice in the Description page of Project Settings.


#include "FFPlayerController.h"

void AFFPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	PlayerCameraManager->ViewPitchMax = 179.99f;
	PlayerCameraManager->ViewPitchMin = -179.99f;
	PlayerCameraManager->ViewRollMin = -179.99f;
	PlayerCameraManager->ViewRollMax = 179.99f;
}