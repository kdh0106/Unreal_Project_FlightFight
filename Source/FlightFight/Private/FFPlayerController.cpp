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

void AFFPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

//AFFPlayerController::AFFPlayerController()
//{
//	bInitialRotationSet = false;
//}

void AFFPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*if (!bInitialRotationSet && GetPawn())
	{
		SetControlRotation(GetPawn()->GetActorRotation()); 
		bInitialRotationSet = true;
		ABLOG(Warning, TEXT("Initial ControlRotation set to : %s"), *GetControlRotation().ToString());
	}

	if (GetPawn())
	{
		ABLOG(Warning, TEXT("Tick - ControlRotation: %s, PawnRotation: %s"),
			*GetControlRotation().ToString(), *GetPawn()->GetActorRotation().ToString());
	}*/
}