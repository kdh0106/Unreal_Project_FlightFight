// Fill out your copyright notice in the Description page of Project Settings.



#include "FFGameMode.h"
#include "FFPawn.h"
#include "FFPlayerController.h"
#include "UObject/ConstructorHelpers.h"

AFFGameMode::AFFGameMode()
{
	DefaultPawnClass = AFFPawn::StaticClass();
	PlayerControllerClass = AFFPlayerController::StaticClass();
}

void AFFGameMode::RestartPlayer(AController* NewPlayer)
{
	if (NewPlayer == nullptr)
	{
		return;
	}

	AActor* StartSpot = FindPlayerStart(NewPlayer);
	if (StartSpot != nullptr)
	{
		FVector Location = StartSpot->GetActorLocation();
		FRotator Rotation = StartSpot->GetActorRotation();

		FActorSpawnParameters SpawnParams;
	}
}
