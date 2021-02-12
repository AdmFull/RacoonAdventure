// Fill out your copyright notice in the Description page of Project Settings.


#include "RA_InteractableSaveActor.h"
#include "Factories/Factory.h"
#include "../../../Character/RA_BaseCharacter.h"

ARA_InteractableSaveActor::ARA_InteractableSaveActor(const FObjectInitializer& ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	eInteractionType = EInteractionType::ESaveGameInteract;
}

// Called when the game starts or when spawned
void ARA_InteractableSaveActor::BeginPlay()
{
	Super::BeginPlay();

}


// Called every frame
void ARA_InteractableSaveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ARA_InteractableSaveActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		//Getting our character instance 
		ARA_BaseCharacter* CharacterPtr = Cast<ARA_BaseCharacter>(OtherActor);
		if (CharacterPtr)
		{
			CharacterPtr->BeginInteract(this);
		}
	}
}

void ARA_InteractableSaveActor::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		ARA_BaseCharacter* CharacterPtr = Cast<ARA_BaseCharacter>(OtherActor);
		if (CharacterPtr)
		{
			CharacterPtr->EndInteract(this);
		}
	}
}

void ARA_InteractableSaveActor::OnInteract(AActor* Interructor)
{
	if (Interructor)
	{
		if (GEngine)
		{
			UWorld* CurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
			if (CurWorld)
			{
				pGameInstance->SaveSync("GameSave", 0);
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Someone saving game!"));
			}
		}
	}
}