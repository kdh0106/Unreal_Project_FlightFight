// Fill out your copyright notice in the Description page of Project Settings.


#include "FFPlayerController.h"
#include "FFGameMode.h"
#include "Blueprint/UserWidget.h"
#include "FFHUDWidget.h"
#include "FFEndWidget.h"
#include "FFPawn.h"
#include "FFTitleWidget.h"
#include "FFGameInstance.h"
#include "Kismet/GameplayStatics.h"

AFFPlayerController::AFFPlayerController()
{
	CurrentScore = 0;
	bIsGameOver = false;
}

void AFFPlayerController::BeginPlay()
{ 
	Super::BeginPlay();
	FInputModeGameOnly InputMode;
	SetInputMode(InputMode);

	PlayerCameraManager->ViewPitchMax = 179.99f;
	PlayerCameraManager->ViewPitchMin = -179.99f;  
	PlayerCameraManager->ViewRollMin = -179.99f;
	PlayerCameraManager->ViewRollMax = 179.99f;
	
	CurrentScore = 0;

	UFFGameInstance* MyGameInstance = Cast<UFFGameInstance>(GetGameInstance());

	if (IsLocalPlayerController()) //클라이언트들의 PlayerController 복사본이 아닌, 자신의 것만 / 독립적인 점수 관리
	{
		if (MyGameInstance)
		{
			if (!MyGameInstance->bIsTitleWidgetDisplayed)
			{
				bShowMouseCursor = true;
				SetInputMode(FInputModeUIOnly());
				Client_CreateTitle();
				MyGameInstance->bIsTitleWidgetDisplayed = true;
				if (GEngine)
				{
					GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Beginplay in Controller active!!!!!")));
				}
			}
			else
			{
				Client_CreateHUD();
			}
		}
		
	}

	//if (IsLocalPlayerController() && MyGameInstance && !MyGameInstance->bIsTitleWidgetDisplayed) //클라이언트들의 PlayerController 복사본이 아닌, 자신의 것만 / 독립적인 점수 관리
	//{
	//	bShowMouseCursor = true;
	//	SetInputMode(FInputModeUIOnly());
	//	Client_CreateTitle();
	//	MyGameInstance->bIsTitleWidgetDisplayed = true;
	//	//bIsTitleDisplayed = true;
	//	if (GEngine)
	//	{
	//		GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Red, FString(TEXT("Beginplay in Controller active!!!!!")));
	//	}
	//	//Client_CreateHUD();
	//} 
}

void AFFPlayerController::Client_CreateTitle_Implementation()
{
	if (IsLocalPlayerController())
	{
		if (!TitleWidget)
		{
			TSubclassOf<UFFTitleWidget> TitleWidgetClass = LoadClass<UFFTitleWidget>(nullptr, TEXT("/Game/Book/UI/UI_Title.UI_Title_C"));
			if (TitleWidgetClass)
			{
				TitleWidget = CreateWidget<UFFTitleWidget>(this, TitleWidgetClass);
				if (TitleWidget)
				{
					TitleWidget->AddToViewport();
				}
			}
		}
	}
}

void AFFPlayerController::Client_CreateHUD_Implementation()	
{
	if (IsLocalPlayerController())
	{
		if (!HUDWidget)
		{
			TSubclassOf<UFFHUDWidget> HUDWidgetClass = LoadClass<UFFHUDWidget>(nullptr, TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
			if (HUDWidgetClass)
			{
				HUDWidget = CreateWidget<UFFHUDWidget>(this, HUDWidgetClass);
				if (HUDWidget)
				{
					HUDWidget->AddToViewport();
					UpdateScoreDisplay();
				}
			}
		}
	}
}

void AFFPlayerController::UpdateScoreDisplay()
{
	if (IsLocalPlayerController() && HUDWidget)
	{
		HUDWidget->UpdateScore(CurrentScore);
	}
}

void AFFPlayerController::AddScore()
{
	CurrentScore++;
	UpdateScoreDisplay();
	if (CurrentScore >= 5)
	{
		bIsGameOver = true;
		Client_OnGameEnd(true);
		Client_DisableInput();
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			AFFPlayerController* OtherPlayer = Cast<AFFPlayerController>(*It);
			if (OtherPlayer && OtherPlayer != this)
			{
				OtherPlayer->Client_OnGameEnd(false);
				OtherPlayer->bIsGameOver = true;
			}
		}
	}
}

void AFFPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (bIsGameOver)
	{
		//바로 호출했을 땐 Client_DisableInput이 호출되지 않음.
		//지연 호출하여 GetPawn()이 완전히 초기화된 이후 입력을 비활성화할 수 있게끔 해줬음.

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &AFFPlayerController::Client_DisableInput, 0.1f, false);
		
		ABLOG(Warning, TEXT("onpossess called on %s, bisgameover : %s"), *GetName(), bIsGameOver ? TEXT("True") : TEXT("false"));
	}
}


void AFFPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
} 

void AFFPlayerController::Client_NotifyScoreAdd_Implementation()
{
	AddScore();
}

void AFFPlayerController::Client_OnGameEnd_Implementation(bool bIsWinner)
{
	if (!EndWidget)
	{
		TSubclassOf<UFFEndWidget> EndWidgetClass = LoadClass<UFFEndWidget>(nullptr, TEXT("/Game/Book/UI/UI_End.UI_End_C"));
		if (EndWidgetClass)
		{
			EndWidget = CreateWidget<UFFEndWidget>(this, EndWidgetClass);
			if (EndWidget)
			{
				EndWidget->AddToViewport();
				EndWidget->SetResultText(bIsWinner, CurrentScore);
				bShowMouseCursor = true;
				SetInputMode(FInputModeUIOnly());
			}	
		}
	}
}

void AFFPlayerController::Client_DisableInput_Implementation()
{
	if (GetPawn())
	{
		GetPawn()->DisableInput(this);
	}
}

