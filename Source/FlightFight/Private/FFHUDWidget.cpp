// Fill out your copyright notice in the Description page of Project Settings.


#include "FFHUDWidget.h"
#include "Components/TextBlock.h"
#include "FFPlayerState.h"

void UFFHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CurrentScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("textCurrentScore")));
	HighScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("textHighScore")));
}
//void UFFHUDWidget::UpdateHUDWidget(int32 CurrentGameScore, int32 HighGameScore)	
//{
//	if (CurrentScore)
//	{
//		CurrentScore->SetText(FText::FromString(FString::FromInt(CurrentGameScore)));
//	}
//	if (HighScore)
//	{
//		HighScore->SetText(FText::FromString(FString::FromInt(HighGameScore))); 
//	}
//}

void UFFHUDWidget::UpdateScore(int32 NewScore)
{
	if (CurrentScore)
	{
		CurrentScore->SetText(FText::FromString(FString::FromInt(NewScore)));
	}
}