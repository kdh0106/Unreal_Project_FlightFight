// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "FFHUDWidget.h"
#include "FFEndWidget.h"
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
	AFFPlayerController();

	UPROPERTY()
	int32 CurrentScore;

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
	class UFFHUDWidget* HUDWidget;

	UPROPERTY()
	class UFFEndWidget* EndWidget;

	UFUNCTION(Client, Reliable)
	void Client_CreateHUD();

	void UpdateScoreDisplay();

	void AddScore();

	void NotifyEnemyKilled();

	UFUNCTION(Client, Reliable)
	void Client_NotifyScoreAdd();

	UFUNCTION(Client, Reliable)
	void Client_OnGameEnd(bool bIsWinner);

	UFUNCTION(Client, Reliable)
	void Client_DisableInput();
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	bool bIsGameOver;
};
