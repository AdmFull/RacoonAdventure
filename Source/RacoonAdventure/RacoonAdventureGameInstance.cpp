// Fill out your copyright notice in the Description page of Project Settings.


#include "RacoonAdventureGameInstance.h"

void URacoonAdventureGameInstance::Init()
{
	//Default values for main player
	uiLevel = 1;
	uiStrength = 5;
	uiEndurance = 5;
	uiCharisma = 5;
	uiIntelligence = 5;
	uiAgility = 5;

	fPlayerHP = 10; 
	fMaxPlayerHP = 20 + 10 * uiEndurance + 2 * uiStrength + 2 * uiLevel;

	fPlayerMana = 10;
	fMaxPlayerMana = 20 + 10 * uiIntelligence + 2 * uiLevel;

	fPlayerStamina = 10;
	fMaxPlayerStamina = 20 + 5 * uiStrength + 5 * uiEndurance + 2 * uiLevel;

	fHPRegenRate = 0.01f;
	fManaRegenRate = 0.01f;
	fStaminaRegenRate = 0.05f;

	fHPRegenSleepTime = 10.f;
	fManaRegenSleepTime = 5.f;
	fStaminaRegenSleepTime = 1.f;

	bCanRegenerate = bCanRegenHP = bCanRegenMana = bCanRegenStamina = true;
}

void URacoonAdventureGameInstance::SetPlayerStat(int32 uiNewStat, EPlayerStats eStat)
{
	switch (eStat)
	{
	case EPlayerStats::PLAYER_STRENGTH:		uiStrength = uiNewStat; break;
	case EPlayerStats::PLAYER_ENDURANCE:	uiEndurance = uiNewStat; break;
	case EPlayerStats::PLAYER_CHARISMA:		uiCharisma = uiNewStat; break;
	case EPlayerStats::PLAYER_INTELLIGENCE:	uiIntelligence = uiNewStat; break;
	case EPlayerStats::PLAYER_AGILITY:		uiAgility = uiNewStat; break;
	default:   break;
	}
}

int32 URacoonAdventureGameInstance::GetPlayerStat(EPlayerStats eStat)
{
	switch (eStat)
	{
	case EPlayerStats::PLAYER_STRENGTH:		return uiStrength; break;
	case EPlayerStats::PLAYER_ENDURANCE:	return uiEndurance; break;
	case EPlayerStats::PLAYER_CHARISMA:		return uiCharisma; break;
	case EPlayerStats::PLAYER_INTELLIGENCE:	return uiIntelligence; break;
	case EPlayerStats::PLAYER_AGILITY:		return uiAgility; break;
	default:								return 999999; break;
	}
}


/*****************************************************************************/
/****************************Health Points Options****************************/
/*****************************************************************************/
void URacoonAdventureGameInstance::AddPlayerHP(float value)
{
	fPlayerHP = (fPlayerHP += value) < 0 ? 0.f : fPlayerHP;
}

void URacoonAdventureGameInstance::AcceptPlayerDamage(float damage)
{ 
	bCanRegenHP = false;
	AddPlayerHP(damage * -1.f); 
	GetWorld()->GetTimerManager().SetTimer(HPRegenerationTimer, [this]() { bCanRegenHP = true; }, fHPRegenSleepTime, 1);
}

void URacoonAdventureGameInstance::RegenerateHP()
{
	{
		if (fPlayerHP < fMaxPlayerHP && bCanRegenHP)
		{
			fPlayerHP += fHPRegenRate;
		}
	}
}


/*****************************************************************************/
/********************************Mana Options*********************************/
/*****************************************************************************/
void URacoonAdventureGameInstance::AddPlayerMana(float value)
{
	fPlayerMana = (fPlayerMana += value) < 0 ? 0.f : fPlayerMana;
}

void URacoonAdventureGameInstance::RegenerateMana()
{
	if (fPlayerMana < fMaxPlayerMana && bCanRegenMana)
	{
		fPlayerMana += fManaRegenRate;
	}
}

void URacoonAdventureGameInstance::WastePlayerMana(float mana)
{
	bCanRegenMana = false;
	AddPlayerMana(mana * -1.f);
	GetWorld()->GetTimerManager().SetTimer(HPRegenerationTimer, [this]() { bCanRegenMana = true; }, fManaRegenSleepTime, 1);
}

bool URacoonAdventureGameInstance::WastePlayerManaIfPossible(float mana)
{
	if (IsManaEnought(mana))
	{
		WastePlayerMana(mana);
		return true;
	}
	return false;
}


/*****************************************************************************/
/******************************Stamina Options********************************/
/*****************************************************************************/
void URacoonAdventureGameInstance::AddPlayerStamina(float value)
{
	fPlayerStamina = (fPlayerStamina += value) < 0 ? 0.f : fPlayerStamina;
}

void URacoonAdventureGameInstance::RegenerateStamina()
{
	if (fPlayerStamina < fMaxPlayerStamina && bCanRegenStamina)
	{
		fPlayerStamina += fStaminaRegenRate;
	}
}

void URacoonAdventureGameInstance::WastePlayerStamina(float stamina)
{
	bCanRegenStamina = false;
	AddPlayerStamina(stamina * -1.f);
	GetWorld()->GetTimerManager().SetTimer(HPRegenerationTimer, [this]() { bCanRegenStamina = true; }, fStaminaRegenSleepTime, 1);
}

bool URacoonAdventureGameInstance::WastePlayerStaminaIfPossible(float stamina)
{
	if (IsStaminaEnought(stamina))
	{
		WastePlayerStamina(stamina);
		return true;
	}
	return false;
}


void URacoonAdventureGameInstance::RegenerationHandler()
{
	if (bCanRegenerate)
	{
		RegenerateHP();
		RegenerateMana();
		RegenerateStamina();
	}
}