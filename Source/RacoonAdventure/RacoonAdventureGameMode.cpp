// Copyright Epic Games, Inc. All Rights Reserved.

#include "RacoonAdventureGameMode.h"
#include "RacoonAdventureCharacter.h"

ARacoonAdventureGameMode::ARacoonAdventureGameMode()
{
	// Set default pawn class to our character
	DefaultPawnClass = ARacoonAdventureCharacter::StaticClass();	
}

void ARacoonAdventureGameMode::BeginPlay()
{
	Super::BeginPlay();
	ChangeMenuWidget(StartingWidgetClass);
}

void ARacoonAdventureGameMode::ChangeMenuWidget(TSubclassOf<UUserWidget> NewWidgetClass)
{
    if (CurrentWidget != nullptr)
    {
        CurrentWidget->RemoveFromViewport();
        CurrentWidget = nullptr;
    }
    if (NewWidgetClass != nullptr)
    {
        CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), NewWidgetClass);
        if (CurrentWidget != nullptr)
        {
            CurrentWidget->AddToViewport();
        }
    }
}