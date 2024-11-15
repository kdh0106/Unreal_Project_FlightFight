// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/PlayerState.h"
#include "GameFramework/GameState.h"
#include "Blueprint/UserWidget.h"
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

	void HostServer();
	void JoinServer(const FString& IPAddress);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<class UFFHUDWidget> HUDWidgetClass;

	UPROPERTY()
	class UFFHUDWidget* HUDWidget;

	virtual void BeginPlay() override;

private:
	TArray<AActor*> UsedPlayerStarts;
	TSet<AController*> SpawnedPlayers;
	bool bIsListenServerHost;

	UPROPERTY()
	TMap<AController*, APlayerStart*> PlayerStartMap;

	UPROPERTY()
	TArray<APlayerStart*> PlayerStarts;
	TArray<AActor*> PlayerStartsActors;

	bool SpawnPlayerPawnIfNeeded(AController* NewPlayer, AActor* StartSpot);

	void LogPlayerStarts();
	void InitializePlayerStarts();
	int32 GetPlayerIndex(AController* Player) const;

	int32 NextPlayerIndex;
};
