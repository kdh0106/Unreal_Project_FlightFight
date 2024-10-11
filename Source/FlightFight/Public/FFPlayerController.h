// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "GameFramework/PlayerController.h"
#include "FFPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTFIGHT_API AFFPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	//AFFPlayerController();

	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	//bool bInitialRotationSet;
};
