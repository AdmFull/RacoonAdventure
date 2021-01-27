// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "RacoonAdventureGameInstance.generated.h"

UENUM(BlueprintType)
enum class EPlayerStats : uint8 {
	PLAYER_STRENGTH,
	PLAYER_ENDURANCE,
	PLAYER_CHARISMA,
	PLAYER_INTELLIGENCE,
	PLAYER_AGILITY
};

/**
 * 
 */
UCLASS()
class RACOONADVENTURE_API URacoonAdventureGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	float fPlayerHP, fMaxPlayerHP;
	float fPlayerMana, fMaxPlayerMana;
	float fPlayerStamina, fMaxPlayerStamina;

	int32 uiLevel;
	int32 uiStrength;
	int32 uiEndurance;
	int32 uiCharisma;
	int32 uiIntelligence;
	int32 uiAgility;

public:
	virtual void Init() override;

public:
	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
	float GetPlayerHP() { return fPlayerHP; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
	float GetPlayerMaxHP() { return fMaxPlayerHP; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerHPPerc() { return fPlayerHP / fMaxPlayerHP; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerMana() { return fPlayerMana; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerManaPerc() { return fPlayerMana / fMaxPlayerMana; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerStamina() { return fPlayerStamina; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerStaminaPerc() { return fPlayerStamina / fMaxPlayerStamina; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void SetPlayerHP(float newHP) { fPlayerHP = newHP; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void SetPlayerMana(float newMana) { fPlayerMana = newMana; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void SetPlayerStamina(float newStamina) { fPlayerStamina = newStamina; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		int32 GetPlayerStat(EPlayerStats eStat);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void SetPlayerStat(int32 uiNewStat, EPlayerStats eStat);
	
};
