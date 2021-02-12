// Fill out your copyright notice in the Description page of Project Settings.


#include "RA_LedderActor.h"
#include "../../../Character/RA_BaseCharacter.h"

ARA_LedderActor::ARA_LedderActor(const FObjectInitializer& ObjectInitializer)
{
	eInteractionType = EInteractionType::ELedderInteract;
}

// Called when the game starts or when spawned
void ARA_LedderActor::BeginPlay()
{
	Super::BeginPlay();

}

void ARA_LedderActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
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

void ARA_LedderActor::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		ARA_BaseCharacter* CharacterPtr = Cast<ARA_BaseCharacter>(OtherActor);
		if (CharacterPtr)
		{
			CharacterPtr->GetCharacterMovement()->GravityScale = 2.f;
			CharacterPtr->EndInteract(this);
		}
	}
}

void ARA_LedderActor::OnInteract(AActor* Interructor)
{
	if (Interructor)
	{
		ARA_BaseCharacter* CharacterPtr = Cast<ARA_BaseCharacter>(Interructor);
		if (CharacterPtr)
		{
			FVector vActorPosition = CharacterPtr->GetActorLocation();
			if (CharacterPtr->GetVerticalDirection() != 0)
			{
				if (!CharacterPtr->GetCharacterMovement()->IsFalling())
				{
					CharacterPtr->LaunchCharacter(FVector(0.f, 0.f, 20.f), true, true);
				}
				else
				{
					CharacterPtr->GetCharacterMovement()->Velocity = FVector(0.f, 0.f, 0.f);
					CharacterPtr->GetCharacterMovement()->GravityScale = 10.f / (pGameInstance->GetPlayerStat(EPlayerStats::PLAYER_STRENGTH) + pGameInstance->GetPlayerStat(EPlayerStats::PLAYER_ENDURANCE));
					FVector vNewActorLocation = vActorPosition + (FVector(5.f, 0.f, 0.f) * CharacterPtr->GetHorisontalDirection());
					vNewActorLocation.Z = vNewActorLocation.Z + (CharacterPtr->GetVerticalDirection() * CharacterPtr->GetCharacterMovement()->MaxWalkSpeed * fDeltaTime * 0.2f) / 2.5f;
					CharacterPtr->SetActorLocation(vNewActorLocation, true);
					//bIsClimbingMove = true;

					//If want to jump, check direction to jump
					if (CharacterPtr->GetHorisontalDirection() != 0.f)
					{
						FVector vNewImpulse = FVector(100.f, 0.f, 0.f) * CharacterPtr->GetHorisontalDirection();
						CharacterPtr->LaunchCharacter(vNewImpulse, true, true);
					}
				}
			}
			else
			{
				CharacterPtr->GetCharacterMovement()->GravityScale = 2.f;
			}
		}
	}
}