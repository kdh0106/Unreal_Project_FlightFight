// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "Blueprint/UserWidget.h"
#include "FFHPBarWidget.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTFIGHT_API UFFHPBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateHPBarWidget(float CurrentHP, float MaxHP);

private:
	UPROPERTY()
	class UProgressBar* HPProgressBar;
};
