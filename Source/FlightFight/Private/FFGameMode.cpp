// Fill out your copyright notice in the Description page of Project Settings.



#include "FFGameMode.h"
#include "FFPawn.h"
#include "FlightFight.h"
#include "FFPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"
#include "FFTitleWidget.h"

AFFGameMode::AFFGameMode()
{
	DefaultPawnClass = AFFPawn::StaticClass();
	PlayerControllerClass = AFFPlayerController::StaticClass();
	//PlayerStateClass = AFFPlayerState::StaticClass();
	/*static ConstructorHelpers::FClassFinder<UFFHUDWidget>UI_HUD_C(TEXT("/Game/Book/UI/UI_HUD.UI_HUD_C"));
	if (UI_HUD_C.Succeeded())
	{
		HUDWidgetClass = UI_HUD_C.Class;
		ABLOG(Warning, TEXT("HUD Widget Class loaded success"));
	}
	else
	{
		ABLOG(Warning, TEXT("Failed to HUD WIdget"));
	}*/

	bIsListenServerHost = false;
	NextPlayerIndex = 0;
}

void AFFGameMode::BeginPlay()
{
	Super::BeginPlay();	
}

void AFFGameMode::InitializePlayerStarts()
{
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartsActors);

	for (AActor* Actor : PlayerStartsActors)
	{
		APlayerStart* PlayerStart = Cast<APlayerStart>(Actor);
		if (PlayerStart)
		{
			PlayerStarts.Add(PlayerStart);
		}
	}

	//�������� Spawn�Ǵ°� ����
	PlayerStarts.Sort([](const APlayerStart& A, const APlayerStart& B) {
		return A.GetName() < B.GetName();
		});
}

void AFFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ChoosePlayerStart(NewPlayer);
	LogPlayerStarts();
}

int32 AFFGameMode::GetPlayerIndex(AController* Player) const
{
	if (Player == nullptr)
		return -1;

	APlayerState* PlayerState = Player->GetPlayerState<APlayerState>();
	if (PlayerState == nullptr)
		return -1;

	return GameState->PlayerArray.IndexOfByPredicate([PlayerState](const APlayerState* PS) {
		return PS == PlayerState;
		});
}


AActor* AFFGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	Super::ChoosePlayerStart_Implementation(Player);
	if (PlayerStarts.Num() == 0)
	{
		InitializePlayerStarts();
	}

	if (PlayerStarts.Num() < 2)
	{
		UE_LOG(LogTemp, Warning, TEXT("Not enough PlayerStarts in the level!"));
		return nullptr;
	}

	if (APlayerStart** FoundStart = PlayerStartMap.Find(Player))
	{
		return *FoundStart;
	}
	// Player�� �ε����� Ȯ�� (0 �Ǵ� 1)
	int32 PlayerIndex = GetPlayerIndex(Player);
	ABLOG(Warning, TEXT("PlayerIndex is : %d"), PlayerIndex);

	// �÷��̾� �ε����� ���� ������ PlayerStart ��ȯ
	if (PlayerIndex == 0 || PlayerIndex == 1)
	{
		APlayerStart* Start = PlayerStarts[PlayerIndex];
		PlayerStartMap.Add(Player, Start);
		return Start;
	}

	// ����ġ ���� ���, ù ��° PlayerStart ��ȯ
	UE_LOG(LogTemp, Warning, TEXT("Unexpected player index. Using first PlayerStart."));
	return PlayerStarts[0];
}

void AFFGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
	if (!IsValid(NewPlayer))
	{
		return;
	}

	AActor* StartSpot = ChoosePlayerStart(NewPlayer);

	if (StartSpot == nullptr)
	{
		return;
	}

	// ���� Pawn ����
	if (NewPlayer->GetPawn() != nullptr)
	{
		NewPlayer->GetPawn()->Destroy();
	}

	FTransform SpawnTransform = StartSpot->GetActorTransform();
	// �� Pawn ����
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnTransform, SpawnParams);

	if (NewPawn != nullptr)
	{
		NewPawn->SetActorLocationAndRotation(SpawnTransform.GetLocation(), SpawnTransform.GetRotation());

		NewPlayer->Possess(NewPawn);
		if (APlayerController* PC = Cast<APlayerController>(NewPlayer))
		{
			PC->SetInitialLocationAndRotation(SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator());
			PC->SetControlRotation(SpawnTransform.GetRotation().Rotator()); 
		}
	}
}

bool AFFGameMode::SpawnPlayerPawnIfNeeded(AController* NewPlayer, AActor* StartSpot)
{
	if (NewPlayer->GetPawn() != nullptr)
	{
		// �̹� Pawn�� �ִ� ���, ��ġ�� ����
		NewPlayer->GetPawn()->SetActorLocation(StartSpot->GetActorLocation());
		NewPlayer->GetPawn()->SetActorRotation(StartSpot->GetActorRotation());
		ABLOG(Warning, TEXT("Existing pawn repositioned for player at %s"), *StartSpot->GetActorLocation().ToString());
		return true;
	}
	 
	if (SpawnedPlayers.Contains(NewPlayer))
	{
		ABLOG(Warning, TEXT("Player already spawned, but no pawn found. This shouldn't happen."));
		return false;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, StartSpot->GetActorLocation(), StartSpot->GetActorRotation(), SpawnParams);
	if (NewPawn)
	{
		NewPlayer->Possess(NewPawn);
		SpawnedPlayers.Add(NewPlayer);
		ABLOG(Warning, TEXT("%s Pawn spawned and possessed successfully at %s"), *NewPawn->GetName(), *NewPawn->GetActorLocation().ToString());
		return true;
	}
	else
	{
		ABLOG(Error, TEXT("Failed to spawn pawn at %s"), *StartSpot->GetActorLocation().ToString());
		return false;
	}
}

void AFFGameMode::LogPlayerStarts()
{
	ABLOG(Warning, TEXT("Logging all PlayerStarts!"));
	for (TActorIterator<APlayerStart>It(GetWorld()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;
	}
} 