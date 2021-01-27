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

	fPlayerMana = 20;
	fMaxPlayerMana = 20 + 10 * uiIntelligence + 2 * uiLevel;

	fPlayerStamina = 20;
	fMaxPlayerStamina = 20 + 5 * uiStrength + 5 * uiEndurance + 2 * uiLevel;
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