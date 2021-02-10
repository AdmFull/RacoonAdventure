// Fill out your copyright notice in the Description page of Project Settings.


#include "RA_CharacterMovementComponent.h"
#include "RA_Character.h"
#include "Kismet/GameplayStatics.h"

//TODO: Remove this class

URA_CharacterMovementComponent::URA_CharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{

}

void URA_CharacterMovementComponent::Initialize()
{
	UWorld* CurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (CurWorld)
	{
		if (CurWorld->GetGameInstance() != nullptr)
		{
			//Contains all game parameters
			cgiGameInstance = Cast<URacoonAdventureGameInstance>(CurWorld->GetGameInstance());

			CharacterPtr = Cast<ARA_Character>(UGameplayStatics::GetPlayerCharacter(CurWorld, 0));
		}
	}
}

void URA_CharacterMovementComponent::TraceRay(FHitResult& hitResult, bool& traceResult, FVector positionFrom, FVector positionTo)
{

	UWorld* wCurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (wCurWorld)
	{
		//DrawDebugLine(wCurWorld, positionFrom, positionTo, FColor::Cyan, false, 1.f);
		traceResult = wCurWorld->LineTraceSingleByChannel(hitResult, positionFrom, positionTo, ECollisionChannel::ECC_Visibility);
	}
}

void URA_CharacterMovementComponent::MakeJerk(float jerkForce, float HorisontalDirection, float VerticalDirection)
{
	if (bCanJerk && cgiGameInstance->IsStaminaEnought(5.f))
	{
		if (iJerkState < 0) GetWorld()->GetTimerManager().SetTimer(JerkTimeoutTimer, [this]() { iJerkState = -1; }, 0.2f, 1);
		if (++iJerkState > 0)
		{
			if (cgiGameInstance->WastePlayerStaminaIfPossible(5.f))
			{
				CharacterPtr->LaunchCharacter(FVector(jerkForce * HorisontalDirection, 0.f, jerkForce * VerticalDirection), false, true);
				iJerkState = -1;
				bCanJerk = false;
				GetWorld()->GetTimerManager().SetTimer(CanJerkTimer, [this]() { bCanJerk = true; }, 0.6f, 1);
			}
		}
	}
}

void URA_CharacterMovementComponent::LedderMovingMode()
{
	FHitResult hrLadder, hrGround;
	FVector vActorPosition = CharacterPtr->GetActorLocation();
	FVector vLadderOutPosition = vActorPosition + FVector(0.f, -50.f, 0.f);
	bool bTraceResult;

	TraceRay(hrLadder, bTraceResult, vActorPosition, vLadderOutPosition);

	if (bIsClimbing)
	{
		if (fVerticalDirection == 0)
		{
			bIsClimbingMove = false;
		}

		if (bTraceResult && hrLadder.GetActor()->ActorHasTag("ladder"))
		{
			this->Velocity = FVector(0.f, 0.f, 0.f);
			this->GravityScale = 10.f / (cgiGameInstance->GetPlayerStat(EPlayerStats::PLAYER_STRENGTH) + cgiGameInstance->GetPlayerStat(EPlayerStats::PLAYER_ENDURANCE));
			if (fVerticalDirection != 0)
			{
				FVector vNewActorLocation = vActorPosition + (FVector(5.f, 0.f, 0.f) * fHorisontalDirection);
				vNewActorLocation.Z = vNewActorLocation.Z + (fVerticalDirection * this->MaxWalkSpeed * fDeltaTime * 0.2f) / 2.5f;
				CharacterPtr->SetActorLocation(vNewActorLocation, true);
				bIsClimbingMove = true;

				//If want to jump, check direction to jump
				if (fHorisontalDirection != 0.f)
				{
					FVector vNewImpulse = FVector(100.f, 0.f, 0.f) * fHorisontalDirection;
					this->AddImpulse(vNewImpulse, true);
				}
			}
		}
		else
		{
			this->GravityScale = 2.f;
			FVector vNewImpulse = (FVector(0.f, 0.f, 300.f) + fVerticalDirection) + (FVector(50.f, 0.f, 0.f) * fHorisontalDirection);
			this->AddImpulse(vNewImpulse, true);
			bIsClimbingMove = false;
			bIsClimbing = false;
		}
	}
	else
	{
		if (fVerticalDirection != 0)
		{
			if (bTraceResult)
			{
				if (hrLadder.GetActor()->ActorHasTag("ladder"))
				{
					bIsClimbing = true;
					FVector vHitActorLocation = hrLadder.GetActor()->GetActorLocation() + (FVector(5.f, 0.f, 0.f) * fHorisontalDirection);
					vHitActorLocation.Y = 0.f;
					CharacterPtr->SetActorLocation(vHitActorLocation);
					FVector vNewImpulse = FVector(0.f, 0.f, 50.f);
					this->AddImpulse(vNewImpulse, true);
				}
			}
		}
	}
}

bool URA_CharacterMovementComponent::TryWallJump()
{
	bool bTraceResult;
	FHitResult hrWall;
	FVector vInWJDirection;
	FVector vActorPosition = GetActorLocation();

	if (!bIsWallJumping && !bPostWallJump)
	{
		FVector vTracePosition = FVector(20.f * fCharacterMoveDirection, 0.f, 0.f) + vActorPosition;
		TraceRay(hrWall, bTraceResult, vActorPosition, vTracePosition);

		if (bTraceResult && hrWall.GetActor()->ActorHasTag("wall_jumpable"))
		{
			if (this->IsFalling())
			{
				if (cgiGameInstance->WastePlayerStaminaIfPossible(5.f))
				{
					bIsWallJumping = bPostWallJump = true;
					vInWJLocation = vActorPosition;
					if (vActorPosition.X > hrWall.GetActor()->GetActorLocation().X)
						vInWJDirection = FVector(500.f, 0.f, 500.f);
					else
						vInWJDirection = FVector(-500.f, 0.f, 500.f);

					CharacterPtr->LaunchCharacter(vInWJDirection, true, true);
					bIsWallJumping = bPostWallJump = false;
					return true;
				}
			}
		}
	}
	return false;
}

void URA_CharacterMovementComponent::ThroughTheFlor()
{
	FHitResult hrFlor;
	bool bTraceResult;
	FVector vActorPosition = GetActorLocation();
	FVector vFlorOutPosition = vActorPosition + FVector(0.f, 0.f, -30.f);

	TraceRay(hrFlor, bTraceResult, vActorPosition, vFlorOutPosition);

	if (bTraceResult && hrFlor.GetActor()->ActorHasTag("flor"))
	{
		if (iThroughFlor < 0) GetWorld()->GetTimerManager().SetTimer(ThroughTheFlorTimer, [this]() { iThroughFlor = -1; }, 0.6f, 1);
		if (++iThroughFlor > 0)
		{
			FVector vNewActorLocation = vActorPosition + FVector(0.f, 0.f, -60.f);
			CharacterPtr->SetActorLocation(vNewActorLocation);
		}
	}
}