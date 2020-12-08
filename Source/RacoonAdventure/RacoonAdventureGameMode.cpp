// Copyright Epic Games, Inc. All Rights Reserved.

#include "RacoonAdventureGameMode.h"
#include "RacoonAdventureCharacter.h"

ARacoonAdventureGameMode::ARacoonAdventureGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = ARacoonAdventureCharacter::StaticClass();	
}
