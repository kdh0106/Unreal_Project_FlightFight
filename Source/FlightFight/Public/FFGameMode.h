// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
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

	virtual void PostLogin(APlayerController* NewPlayer) override;
	//virtual AActor* FindPlayerStart_Implementation(AController* Player, const FString& IncomingName) override;
	virtual void RestartPlayer(AController* NewPlayer) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

private:
	TArray<AActor*> UsedPlayerStarts;
	TSet<AController*> SpawnedPlayers;
	bool bIsListenServerHost;

	bool SpawnPlayerPawnIfNeeded(AController* NewPlayer, AActor* StartSpot);

	void LogPlayerStarts();

	//TMap<AController*, AActor*> PlayerToStartMap;
	int32 NextPlayerIndex;

	//void AssignPlayerStart(AController* Player);*/
	//void SpawnAndPossessPawn(AController* NewPlayer);
};
