// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FFHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTFIGHT_API UFFHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	//UFUNCTION(BlueprintCallable, Category = "UI")
	//void UpdateHUDWidget(int32 CurrentGameScore, int32 HighGameScore);

	void UpdateScore(int32 NewScore);

private:
	UPROPERTY()
	class UTextBlock* CurrentScore;

	UPROPERTY()
	class UTextBlock* HighScore;

protected:
	virtual void NativeConstruct() override;
};
