// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../GameLogic/RacoonAdventureGameInstance.h"
#include "RA_CharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class RACOONADVENTURE_API URA_CharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_UCLASS_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = GameInstance)
		void Initialize();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GameInstance)
		URacoonAdventureGameInstance* cgiGameInstance;

	UFUNCTION(BlueprintCallable, Category = RayTrace)
		void TraceRay(FHitResult& hitResult, bool& traceResult, FVector positionFrom, FVector positionTo);

	UFUNCTION(BlueprintCallable, Category = JerkMechanic)
		void MakeJerk(float jerkForce, float HorisontalDirection, float VerticalDirection);

	UFUNCTION(BlueprintCallable, Category = JumpMechanic)
		void LedderMovingMode();

	UFUNCTION(BlueprintCallable, Category = JumpMechanic)
		bool TryWallJump();

	UFUNCTION(BlueprintCallable, Category = JumpMechanic)
		void ThroughTheFlor();

	UFUNCTION(BlueprintCallable, Category = JumpMechanic)
		bool IsClimbing() { return bIsClimbing; }

	UFUNCTION(BlueprintCallable, Category = JumpMechanic)
		bool IsClimbingMoving() { return bIsClimbingMove; }

	UFUNCTION(BlueprintCallable, Category = Setters)
		void SetHorisontalDirection(float value) { fHorisontalDirection = value; }

	UFUNCTION(BlueprintCallable, Category = Setters)
		void SetVerticalDirection(float value) { fVerticalDirection = value; }

	UFUNCTION(BlueprintCallable, Category = Setters)
		void SetMoveDirection(float value) { fCharacterMoveDirection = value;  }

	UFUNCTION(BlueprintCallable, Category = Setters)
		void SetDeltaTime(float value) { fDeltaTime = value; }

	UFUNCTION(BlueprintCallable, Category = Getters)
		float GetHorisontalDirection() { return fHorisontalDirection; }

	UFUNCTION(BlueprintCallable, Category = Getters)
		float GetVerticalDirection() { return fVerticalDirection; }

	UFUNCTION(BlueprintCallable, Category = Getters)
		float GetMoveDirection() { return fCharacterMoveDirection; }

	UFUNCTION(BlueprintCallable, Category = Getters)
		float GetDeltaTime() { return fDeltaTime; }

	class ARA_Character* CharacterPtr;

private:
	//wall jump mechanic
	bool bIsWallJumping = false;
	bool bOnGround = true;
	bool bPostWallJump = false;
	FVector vInWJLocation;

	/*Jerk realisation*/
	FTimerHandle    JerkTimeoutTimer;
	FTimerHandle    CanJerkTimer;
	int iJerkState = -1;
	bool bCanJerk = true;

	FTimerHandle ThroughTheFlorTimer;
	int iThroughFlor = -1;

	float fHorisontalDirection = 0.f;
	float fVerticalDirection = 0.f;
	float fCharacterMoveDirection = 1.f;

	bool bIsClimbing, bIsClimbingMove;
	float fClibmAx;
	float fDeltaTime;
};
