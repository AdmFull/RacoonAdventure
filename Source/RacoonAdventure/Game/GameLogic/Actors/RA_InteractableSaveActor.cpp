// Fill out your copyright notice in the Description page of Project Settings.


#include "RA_InteractableSaveActor.h"
#include "../../Character/RA_Character.h"

ARA_InteractableSaveActor::ARA_InteractableSaveActor(const FObjectInitializer& ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
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
		ARA_Character* CharacterPtr = Cast<ARA_Character>(OtherActor);
		if (CharacterPtr)
		{
			int i = 0;
		}
	}
}

void ARA_InteractableSaveActor::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		ARA_Character* CharacterPtr = Cast<ARA_Character>(OtherActor);
		if (CharacterPtr)
		{
			int i = 0;
		}
	}
}

void ARA_InteractableSaveActor::OnInteract(AActor* Interructor)
{

}