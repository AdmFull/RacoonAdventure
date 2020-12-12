// Fill out your copyright notice in the Description page of Project Settings.


#include "RacoonAdventurePlayerController.h"
#include "RacoonAdventureGameMode.h"

void ARacoonAdventurePlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameAndUI());
}