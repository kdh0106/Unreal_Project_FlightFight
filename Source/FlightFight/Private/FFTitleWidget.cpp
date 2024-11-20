// Fill out your copyright notice in the Description page of Project Settings.


#include "FFTitleWidget.h"
#include "Components/Button.h"
#include "FFPlayerController.h"
#include "Kismet/GameplayStatics.h"

void UFFTitleWidget::NativeConstruct()
{
	Super::NativeConstruct();

	StartButton = Cast<UButton>(GetWidgetFromName(TEXT("StartBtn")));
	if (StartButton != nullptr)
	{
		StartButton->OnClicked.AddDynamic(this, &UFFTitleWidget::StartButtonClicked);
	}
	ExitButton = Cast<UButton>(GetWidgetFromName(TEXT("ExitBtn")));
	if (ExitButton != nullptr)
	{
		ExitButton->OnClicked.AddDynamic(this, &UFFTitleWidget::ExitButtonClicked);
	}

}

void UFFTitleWidget::StartButtonClicked()
{
	AFFPlayerController* PlayerController = Cast<AFFPlayerController>(GetOwningPlayer());
	if (PlayerController)
	{
		this->RemoveFromViewport(); 
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetInputMode(FInputModeGameOnly());
		PlayerController->Client_CreateHUD();
		//PlayerController->ClientTravel(LobbyMapPath, ETravelType::TRAVEL_Absolute);
	}
 }

void UFFTitleWidget::ExitButtonClicked()
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		UKismetSystemLibrary::QuitGame(GetWorld(), PlayerController, EQuitPreference::Quit, false);
	}
}