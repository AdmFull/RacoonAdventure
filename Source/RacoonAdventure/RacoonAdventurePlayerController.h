// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RacoonAdventurePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class RACOONADVENTURE_API ARacoonAdventurePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
};
