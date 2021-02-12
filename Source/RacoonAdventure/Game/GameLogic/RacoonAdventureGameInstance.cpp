// Fill out your copyright notice in the Description page of Project Settings.


#include "RacoonAdventureGameInstance.h"
#include "../Character/RA_BaseCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameSave/RA_SaveGame.h"

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

void URacoonAdventureGameInstance::SaveSync(FString SlotNameString, int32 UserIndexInt32)
{
	if (URA_SaveGame* SaveGameInstance = Cast<URA_SaveGame>(UGameplayStatics::CreateSaveGameObject(URA_SaveGame::StaticClass())))
	{
		// Set data on the savegame object.
		SaveGameInstance->PlayerName = TEXT("PlayerOne");

		SaveGameInstance->fPlayerHP = fPlayerHP;
		SaveGameInstance->fMaxPlayerHP = fMaxPlayerHP;
		SaveGameInstance->fPlayerMana = fPlayerMana;
		SaveGameInstance->fMaxPlayerMana = fMaxPlayerMana;
		SaveGameInstance->fPlayerStamina = fPlayerStamina;
		SaveGameInstance->fMaxPlayerStamina = fMaxPlayerStamina;

		SaveGameInstance->uiLevel = uiLevel;
		SaveGameInstance->uiExperience = uiExperience;
		SaveGameInstance->uiStrength = uiStrength;
		SaveGameInstance->uiEndurance = uiEndurance;
		SaveGameInstance->uiCharisma = uiCharisma;
		SaveGameInstance->uiIntelligence = uiIntelligence;
		SaveGameInstance->uiAgility = uiAgility;

		ARA_BaseCharacter* PlayerControllerPtr = Cast<ARA_BaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		SaveGameInstance->tPlayerLocation = PlayerControllerPtr->GetActorTransform();

		UWorld* CurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
		if (IsValid(CurWorld))
		{
			SaveGameInstance->LevelName = CurWorld->GetCurrentLevel()->GetFName();
		}

		// Save the data immediately.
		if (UGameplayStatics::SaveGameToSlot(SaveGameInstance, SlotNameString, UserIndexInt32))
		{
			//UE_LOG(LogTemp, Warning, TEXT("Game saved to slot: %s:%d"), SlotNameString, UserIndexInt32);
		}
	}
}

void URacoonAdventureGameInstance::LoadSync(FString SlotNameString, int32 UserIndexInt32)
{
	if (URA_SaveGame* LoadedGame = Cast<URA_SaveGame>(UGameplayStatics::LoadGameFromSlot(SlotNameString, UserIndexInt32)))
	{
		fPlayerHP = LoadedGame->fPlayerHP;
		fMaxPlayerHP = LoadedGame->fMaxPlayerHP;
		fPlayerMana = LoadedGame->fPlayerMana;
		fMaxPlayerMana = LoadedGame->fMaxPlayerMana;
		fPlayerStamina = LoadedGame->fPlayerStamina;
		fMaxPlayerStamina = LoadedGame->fMaxPlayerStamina;

		uiLevel = LoadedGame->uiLevel;
		uiExperience = LoadedGame->uiExperience;
		uiStrength = LoadedGame->uiStrength;
		uiEndurance = LoadedGame->uiEndurance;
		uiCharisma = LoadedGame->uiCharisma;
		uiIntelligence = LoadedGame->uiIntelligence;
		uiAgility = LoadedGame->uiAgility;

		UWorld* CurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
		if (IsValid(CurWorld))
		{
			FLatentActionInfo LatentInfo;
			UGameplayStatics::UnloadStreamLevel(this, CurWorld->GetCurrentLevel()->GetFName(), LatentInfo, true);
			UGameplayStatics::LoadStreamLevel(this, LoadedGame->LevelName, true, true, LatentInfo);
		}

		ARA_BaseCharacter* PlayerControllerPtr = Cast<ARA_BaseCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
		PlayerControllerPtr->SetActorTransform(LoadedGame->tPlayerLocation);

		// The operation was successful, so LoadedGame now contains the data we saved earlier.
		//UE_LOG(LogTemp, Warning, TEXT("Game loaded from slot: %s"), *LoadedGame->PlayerName);
	}
}