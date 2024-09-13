// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "GameFramework/GameModeBase.h"
#include "FFGameMode.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTFIGHT_API AFFGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AFFGameMode();

	void RestartPlayer(AController* NewPlayer);
};
