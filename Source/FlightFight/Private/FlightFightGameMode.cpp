// Copyright Epic Games, Inc. All Rights Reserved.

#include "FlightFightGameMode.h"
#include "FlightFightCharacter.h"
#include "UObject/ConstructorHelpers.h"

AFlightFightGameMode::AFlightFightGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
