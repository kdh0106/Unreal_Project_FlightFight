// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "FlightFight.h"
#include "Engine/GameInstance.h"
#include "FFGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class FLIGHTFIGHT_API UFFGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	bool bIsTitleWidgetDisplayed = false;
};
