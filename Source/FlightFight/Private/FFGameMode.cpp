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
	// Player의 인덱스를 확인 (0 또는 1)
	int32 PlayerIndex = GetPlayerIndex(Player);
	ABLOG(Warning, TEXT("PlayerIndex is : %d"), PlayerIndex);

	// 플레이어 인덱스에 따라 적절한 PlayerStart 반환
	if (PlayerIndex == 0 || PlayerIndex == 1)
	{
		APlayerStart* Start = PlayerStarts[PlayerIndex];
		PlayerStartMap.Add(Player, Start);
		return Start;
	}

	// 예상치 못한 경우, 첫 번째 PlayerStart 반환
	UE_LOG(LogTemp, Warning, TEXT("Unexpected player index. Using first PlayerStart."));
	return PlayerStarts[0];
}

//void AFFGameMode::AssignPlayerStart(AController* Player)
//{
//	if (PlayerStarts.Num() == 0) 
//	{
//		//월드의 모든 PlayerStart를 찾아 PlayerStarts 배열에 저장, APlayerStart 클래스 참조 -> "GameFramework/PlayerStart.h" 
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
//	//할당된 PlayerStart가 없으면, 새로 할당함
//	AssignPlayerStart(Player);
//	
//	if (AActor** FoundStart = PlayerToStartMap.Find(Player))
//	{
//		return *FoundStart;
//	}
//
//	//그래도 없으면 기본 구현 사용
//	return Super::FindPlayerStart_Implementation(Player, IncomingName);
//
//}

void AFFGameMode::RestartPlayer(AController* NewPlayer)
{
	if (!IsValid(NewPlayer))
	{
		return;
	}

	AActor* StartSpot = ChoosePlayerStart(NewPlayer);

	if (StartSpot == nullptr)
	{
		//ABLOG(Warning, TEXT("Failed to find PlayerStart!"));
		return;
	}

	//선택된 PlayerStart 정보
	//ABLOG(Warning, TEXT("RestartPlayer: Selected PlayerStart Location: %s, Rotation: %s"),
	//	*StartSpot->GetActorLocation().ToString(), *StartSpot->GetActorRotation().ToString());

	// 기존 Pawn 제거
	if (NewPlayer->GetPawn() != nullptr)
	{
		NewPlayer->GetPawn()->Destroy();
	}

	FTransform SpawnTransform = StartSpot->GetActorTransform();
	// 새 Pawn 스폰
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	APawn* NewPawn = GetWorld()->SpawnActor<APawn>(DefaultPawnClass, SpawnTransform, SpawnParams);

	if (NewPawn != nullptr)
	{
		NewPawn->SetActorLocationAndRotation(SpawnTransform.GetLocation(), SpawnTransform.GetRotation());

		// 로그 추가: 스폰된 Pawn 정보
		//ABLOG(Warning, TEXT("RestartPlayer: Spawned Pawn Location: %s, Rotation: %s"),
		//	*NewPawn->GetActorLocation().ToString(), *NewPawn->GetActorRotation().ToString());


		NewPlayer->Possess(NewPawn);
		if (APlayerController* PC = Cast<APlayerController>(NewPlayer))
		{
			PC->SetInitialLocationAndRotation(SpawnTransform.GetLocation(), SpawnTransform.GetRotation().Rotator());
			PC->SetControlRotation(SpawnTransform.GetRotation().Rotator());

			// 로그 추가: PlayerController 정보
			ABLOG(Warning, TEXT("RestartPlayer: PlayerController Location: %s, ControlRotation: %s"),
				*PC->GetPawn()->GetActorLocation().ToString(), *PC->GetControlRotation().ToString());
		}
	}
}

bool AFFGameMode::SpawnPlayerPawnIfNeeded(AController* NewPlayer, AActor* StartSpot)
{
	if (NewPlayer->GetPawn() != nullptr)
	{
		// 이미 Pawn이 있는 경우, 위치만 조정
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

void AFFGameMode::LogPlayerStarts()
{
	ABLOG(Warning, TEXT("Logging all PlayerStarts!"));
	for (TActorIterator<APlayerStart>It(GetWorld()); It; ++It)
	{
		APlayerStart* PlayerStart = *It;
		ABLOG(Warning, TEXT("PlayerStart: %s at location %s"),
			*PlayerStart->GetName(), *PlayerStart->GetActorLocation().ToString());
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
