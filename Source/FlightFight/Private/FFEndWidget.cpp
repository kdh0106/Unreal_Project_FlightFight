// Fill out your copyright notice in the Description page of Project Settings.


#include "FFEndWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "FFPlayerController.h"
#include "FFPawn.h"

void UFFEndWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FinishScore = Cast<UTextBlock>(GetWidgetFromName(TEXT("textFinishScore")));
	Result = Cast<UTextBlock>(GetWidgetFromName(TEXT("textResult")));
	QuitButton = Cast<UButton>(GetWidgetFromName(TEXT("btnLeaveSession")));
	if (QuitButton != nullptr)
	{
		QuitButton->OnClicked.AddDynamic(this, &UFFEndWidget::QuitButtonClicked);
	}
}

void UFFEndWidget::SetResultText(bool bIsWinner, int32 NewScore)
{
	if (Result)
	{
		FSlateColor TextColor = bIsWinner ? FSlateColor(FLinearColor::Blue) : FSlateColor(FLinearColor::Red);
		Result->SetText(FText::FromString(bIsWinner ? TEXT("WIN!!") : TEXT("LOSE..")));
		Result->SetColorAndOpacity(TextColor);
	}
	if (FinishScore)
	{
		FinishScore->SetText(FText::FromString(FString::FromInt(NewScore)));
	}
}

void UFFEndWidget::QuitButtonClicked()
{
	AFFPlayerController* PlayerController = Cast<AFFPlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
		APawn* PlayerPawn = PlayerController->GetPawn();
		if (PlayerPawn)
		{
			AFFPawn* MyPawn = Cast<AFFPawn>(PlayerPawn);
			if (MyPawn)
			{
				MyPawn->LeaveGameSession();
			}
		}
	}
}
       

