// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "Blueprint/UserWidget.h"
#include "FFEndWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTFIGHT_API UFFEndWidget : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UPROPERTY()
	class UTextBlock* FinishScore;

	UPROPERTY()
	class UTextBlock* Result;

	UPROPERTY()
	class UButton* QuitButton;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void QuitButtonClicked();
public:
	void SetResultText(bool bIsWinner, int32 NewScore);
};
