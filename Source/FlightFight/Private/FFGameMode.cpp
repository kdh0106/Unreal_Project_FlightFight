// Fill out your copyright notice in the Description page of Project Settings.



#include "FFGameMode.h"
#include "FFPawn.h"
#include "FlightFight.h"
#include "FFPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AFFGameMode::AFFGameMode()
{
	DefaultPawnClass = AFFPawn::StaticClass();
	PlayerControllerClass = AFFPlayerController::StaticClass();

	bIsListenServerHost = false;
	NextPlayerIndex = 0;
}

void AFFGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	//AssignPlayerStart(NewPlayer);

	/*if (GetNetMode() == NM_ListenServer && !bIsListenServerHost)
	{
		bIsListenServerHost = true;
		RestartPlayer(NewPlayer);
	}*/
	/*else
	{
		AActor* StartSpot = ChoosePlayerStart_Implementation(NewPlayer);
		if (StartSpot)
		{
			SpawnPlayerPawn(NewPlayer, StartSpot);
		}
	}*/
	if (!SpawnedPlayers.Contains(NewPlayer))
	{
		RestartPlayer(NewPlayer);
	}

}

AActor* AFFGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	TArray<AActor*> AvailablePlayerStarts;
	if (AvailablePlayerStarts.Num() == 0)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), AvailablePlayerStarts);
	}

	if (AvailablePlayerStarts.Num() > 0)
	{
		int32 StartIndex = NextPlayerIndex % AvailablePlayerStarts.Num();
		AActor* ChosenPlayerStart = AvailablePlayerStarts[StartIndex];
		UsedPlayerStarts.Add(ChosenPlayerStart);
		NextPlayerIndex++;
		return ChosenPlayerStart;
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

//void AFFGameMode::AssignPlayerStart(AController* Player)
//{
//	if (PlayerStarts.Num() == 0) 
//	{
//		//������ ��� PlayerStart�� ã�� PlayerStarts �迭�� ����, APlayerStart Ŭ���� ���� -> "GameFramework/PlayerStart.h" 
//		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
//	} 
//
//	//ABLOG(Warning, TEXT("The PS's Num is : %d"), PlayerStarts.Num());
//
//	if (PlayerStarts.Num() > 0)
//	{
//		int32 StartIndex = NextPlayerIndex % PlayerStarts.Num();
//		PlayerToStartMap.Add(Player, PlayerStarts[StartIndex]);
//
//		FRotator SpawnRotation = PlayerStarts[StartIndex]->GetActorRotation();
//		FVector SpawnLocation = PlayerStarts[StartIndex]->GetActorLocation();
//
//		FString PlayerName = Player->GetName();
//
//		ABLOG(Warning, TEXT("Player : %s // SPR, SPL : %s, %s"), *PlayerName, *SpawnRotation.ToString(), *SpawnLocation.ToString());
//
//		if (APawn* Pawn = Player->GetPawn())
//		{
//			AFFPawn* FFPawn = Cast<AFFPawn>(Pawn);
//			if (FFPawn)
//			{
//				FFPawn->SetSpawnLocationAndRotation(SpawnLocation, SpawnRotation);
//			}
//		}
//		NextPlayerIndex++;
//
//		//FString PlayerName = Player->GetName();
//		//ABLOG(Warning, TEXT("Assign!!!! %s"), *PlayerName);
//	}
//}

//AActor* AFFGameMode::FindPlayerStart_Implementation(AController* Player, const FString& IncomingName)
//{
//	if (AActor** FoundStart = PlayerToStartMap.Find(Player))
//	{
//		return *FoundStart;
//	}
//
//	//�Ҵ�� PlayerStart�� ������, ���� �Ҵ���
//	AssignPlayerStart(Player);
//	
//	if (AActor** FoundStart = PlayerToStartMap.Find(Player))
//	{
//		return *FoundStart;
//	}
//
//	//�׷��� ������ �⺻ ���� ���
//	return Super::FindPlayerStart_Implementation(Player, IncomingName);
//
//}

void AFFGameMode::RestartPlayer(AController* NewPlayer)
{
	if (!IsValid(NewPlayer))
	{
		ABLOG(Error, TEXT("RestartPlayer: Invalid player controller"));
		return;
	}

	AActor* StartSpot = ChoosePlayerStart_Implementation(NewPlayer);
	if (StartSpot)
	{
		SpawnPlayerPawnIfNeeded(NewPlayer, StartSpot);
	}
	else
	{
		ABLOG(Warning, TEXT("Failed to find PlayerStarst for player restart"));
	}
}

bool AFFGameMode::SpawnPlayerPawnIfNeeded(AController* NewPlayer, AActor* StartSpot)
{
	if (NewPlayer->GetPawn() != nullptr)
	{
		// �̹� Pawn�� �ִ� ���, ��ġ�� ����
		NewPlayer->GetPawn()->SetActorLocation(StartSpot->GetActorLocation());
		NewPlayer->GetPawn()->SetActorRotation(StartSpot->GetActorRotation());
		UE_LOG(LogTemp, Log, TEXT("Existing pawn repositioned for player at %s"), *StartSpot->GetActorLocation().ToString());
		return true;
	}

	if (SpawnedPlayers.Contains(NewPlayer))
	{
		UE_LOG(LogTemp, Warning, TEXT("Player already spawned, but no pawn found. This shouldn't happen."));
		return false;
	}

	ABLOG(Warning, TEXT("Spawning Pawn %s for player at location : %s"), *NewPlayer->GetName(), *StartSpot->GetActorLocation().ToString());

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

//void AFFGameMode::SpawnAndPossessPawn(AController* NewPlayer)
//{
//	if (!NewPlayer)
//	{
//		return;
//	}
//	AActor* StartSpot = FindPlayerStart(NewPlayer);
//	if (StartSpot != nullptr)
//	{
//		FVector Location = StartSpot->GetActorLocation(); 
//		FRotator Rotation = StartSpot->GetActorRotation();
//
//		FActorSpawnParameters SpawnParams;
//		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;  //??
//
//		APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, Location, Rotation, SpawnParams);
//		//APawn* NewPawn = SpawnDefaultPawnFor(NewPlayer, StartSpot);
//		ABLOG(Warning, TEXT("Spawn in Restart!!"));
//		if (NewPawn != nullptr)
//		{
//			NewPawn->SetOwner(NewPlayer);
//			NewPlayer->Possess(NewPawn);
//			ABLOG(Warning, TEXT("Possess on REstart!!"));
//
//			if (GetNetMode() == NM_ListenServer || GetNetMode() == NM_DedicatedServer)
//			{
//				NewPawn->SetAutonomousProxy(true);
//			}
//
//			/*AFFPawn* FFPawn = Cast<AFFPawn>(NewPawn);
//			if (FFPawn)
//			{
//				FFPawn->SetInitialSpawnLocation(Location);
//			}*/
//		}
//	}
//	else
//	{
//		ABLOG(Error, TEXT("Could not find a PlayerStart for the player."));
//	}
//}
