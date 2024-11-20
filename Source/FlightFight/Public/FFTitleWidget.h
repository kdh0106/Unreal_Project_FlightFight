// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "Blueprint/UserWidget.h"
#include "FFTitleWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTFIGHT_API UFFTitleWidget : public UUserWidget
{
	GENERATED_BODY()

private:
	UPROPERTY()
	class UButton* StartButton;

	UPROPERTY()
	class UButton* ExitButton;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void StartButtonClicked();

	UFUNCTION()
	void ExitButtonClicked();

	UPROPERTY(EditDefaultsOnly)
	FString LobbyMapPath = "/Game/Book/Maps/NewMap1.NewMap1";
	
};
