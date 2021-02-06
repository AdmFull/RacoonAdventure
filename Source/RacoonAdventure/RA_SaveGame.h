// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "RA_SaveGame.generated.h"

/**
 * 
 */
UCLASS()
class RACOONADVENTURE_API URA_SaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:

    UPROPERTY(VisibleAnywhere, Category = Basic)
        FString PlayerName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
        FString SaveSlotName;

    UPROPERTY(VisibleAnywhere, Category = Basic)
        uint32 UserIndex;

	UPROPERTY(VisibleAnywhere, Category = GameStats)
		float fPlayerHP;
	UPROPERTY(VisibleAnywhere, Category = GameStats)
		float fMaxPlayerHP;
	UPROPERTY(VisibleAnywhere, Category = GameStats)
		float fPlayerMana;
	UPROPERTY(VisibleAnywhere, Category = GameStats)
		float fMaxPlayerMana;
	UPROPERTY(VisibleAnywhere, Category = GameStats)
		float fPlayerStamina;
	UPROPERTY(VisibleAnywhere, Category = GameStats)
		float fMaxPlayerStamina;

	UPROPERTY(VisibleAnywhere, Category = PlayerStats)
		int32 uiLevel;
	UPROPERTY(VisibleAnywhere, Category = PlayerStats)
		int64 uiExperience;
	UPROPERTY(VisibleAnywhere, Category = PlayerStats)
		int32 uiStrength;
	UPROPERTY(VisibleAnywhere, Category = PlayerStats)
		int32 uiEndurance;
	UPROPERTY(VisibleAnywhere, Category = PlayerStats)
		int32 uiCharisma;
	UPROPERTY(VisibleAnywhere, Category = PlayerStats)
		int32 uiIntelligence;
	UPROPERTY(VisibleAnywhere, Category = PlayerStats)
		int32 uiAgility;

	UPROPERTY(VisibleAnywhere, Category = PlayerLocation)
		FTransform tPlayerLocation;
	UPROPERTY(VisibleAnywhere, Category = PlayerLocation)
		int32 uiMapNumber;

    URA_SaveGame();
};
