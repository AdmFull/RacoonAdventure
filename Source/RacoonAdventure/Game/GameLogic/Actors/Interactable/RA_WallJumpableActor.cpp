// Fill out your copyright notice in the Description page of Project Settings.


#include "RA_WallJumpableActor.h"
#include "../../../Character/RA_BaseCharacter.h"

ARA_WallJumpableActor::ARA_WallJumpableActor(const FObjectInitializer& ObjectInitializer)
{
	eInteractionType = EInteractionType::EWallJumpInteract;
}

// Called when the game starts or when spawned
void ARA_WallJumpableActor::BeginPlay()
{
	Super::BeginPlay();

}

void ARA_WallJumpableActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ARA_WallJumpableActor::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
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

void ARA_WallJumpableActor::OnInteract(AActor* Interructor)
{
	if (Interructor)
	{
		ARA_BaseCharacter* CharacterPtr = Cast<ARA_BaseCharacter>(Interructor);
		if (CharacterPtr)
		{
			FVector vInWJDirection;
			FVector vActorPosition = Interructor->GetActorLocation();

			if (CharacterPtr->GetCharacterMovement()->IsFalling())
			{
				if (pGameInstance->WastePlayerStaminaIfPossible(5.f))
				{
					if (vActorPosition.X > this->GetActorLocation().X)
						vInWJDirection = FVector(500.f, 0.f, 500.f);
					else
						vInWJDirection = FVector(-500.f, 0.f, 500.f);

					CharacterPtr->LaunchCharacter(vInWJDirection, true, true);
				}
			}
		}
	}
}