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


	float fHPRegenRate, fHPRegenSleepTime;
	float fManaRegenRate, fManaRegenSleepTime;
	float fStaminaRegenRate, fStaminaRegenSleepTime;

	bool bCanRegenerate, bCanRegenHP, bCanRegenMana, bCanRegenStamina;

	FTimerHandle	HPRegenerationTimer;
	FTimerHandle	ManaRegenerationTimer;
	FTimerHandle	StaminaRegenerationTimer;

public:
	virtual void Init() override;

public:

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void RegenerationHandler();

	/*****************************************************************************/
	/****************************Health Points Options****************************/
	/*****************************************************************************/

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void SetPlayerHP(float newHP) { fPlayerHP = newHP; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void AddPlayerHP(float value);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void AcceptPlayerDamage(float damage);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void RegenerateHP();

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerHP() { return fPlayerHP; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerHPMax() { return fMaxPlayerHP; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerHPPerc() { return fPlayerHP / fMaxPlayerHP; }


	/*****************************************************************************/
	/********************************Mana Options*********************************/
	/*****************************************************************************/
	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void SetPlayerMana(float newMana) { fPlayerMana = newMana; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void AddPlayerMana(float value);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void WastePlayerMana(float mana);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		bool IsManaEnought(float mana) { return fPlayerMana > mana; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		bool WastePlayerManaIfPossible(float mana);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void RegenerateMana();

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerMana() { return fPlayerMana; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerManaMax() { return fMaxPlayerMana; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerManaPerc() { return fPlayerMana / fMaxPlayerMana; }


	/*****************************************************************************/
	/******************************Stamina Options********************************/
	/*****************************************************************************/
	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void SetPlayerStamina(float newStamina) { fPlayerStamina = newStamina; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void AddPlayerStamina(float value);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void WastePlayerStamina(float stamina);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		bool IsStaminaEnought(float stamina) { return fPlayerStamina > stamina; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		bool WastePlayerStaminaIfPossible(float stamina);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void RegenerateStamina();

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerStamina() { return fPlayerStamina; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerStaminaMax() { return fMaxPlayerStamina; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		float GetPlayerStaminaPerc() { return fPlayerStamina / fMaxPlayerStamina; }



	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		int32 GetPlayerStat(EPlayerStats eStat);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
		void SetPlayerStat(int32 uiNewStat, EPlayerStats eStat);
	
};
