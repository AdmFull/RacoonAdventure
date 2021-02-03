// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "RacoonAdventureGameInstance.h"
#include "Templates/SubclassOf.h"
#include "RacoonAdventureCharacter.generated.h"

class UTextRenderComponent;

UENUM(BlueprintType)
enum class EPlayerState : uint8 {
	PLAYER_DEFAULT,
	PLAYER_IDLE,
	PLAYER_RUNNUNG,
	PLAYER_JUMP,
	PLAYER_FALLING,
	PLAYER_CROUCH_IDLE,
	PLAYER_CROUCH_MOVE,
	PLAYER_CLIMB_IDLE,
	PLAYER_CLIMB_MOVE,
	PLAYER_GROUND_ATTACK_0,
	PLAYER_GROUND_ATTACK_1,
	PLAYER_GROUND_ATTACK_FIN,
	PLAYER_AIR_ATTACK_0,
	PLAYER_AIR_ATTACK_1,
	PLAYER_AIR_ATTACK_FIN,
	PLAYER_DEAD
};

/**
 * This class is the default character for RacoonAdventure, and it is responsible for all
 * physical interaction between the player and the world.
 *
 * The capsule component (inherited from ACharacter) handles collision with the world
 * The CharacterMovementComponent (inherited from ACharacter) handles movement of the collision capsule
 * The Sprite component (inherited from APaperCharacter) handles the visuals
 */
UCLASS(config=Game)
class ARacoonAdventureCharacter : public APaperCharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess="true"))
	class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	virtual void Tick(float DeltaSeconds) override;
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		TMap<EPlayerState, class UPaperFlipbook*> AnimationSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = GameInstance)
		URacoonAdventureGameInstance* cgiGameInstance;

	UPROPERTY(EditAnywhere, Category = Animations)
		TSubclassOf<class ARA_DamageActor> DamageActorBlueprintPtr;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	void UpdateCharacter();

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Game")
		ARacoonAdventureGameMode* GetCurrentGamemode();

public:
	ARacoonAdventureCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	FTimerHandle    AnimationDelayTimer;
	FTimerHandle    ComboAttackTimer;
	bool bIsSimpleAttacking;
	bool bCanAttack = true;
	bool bIsJumpUp;
	bool bWantToJump = false;
	float fHorisontalDirection;
	float fVerticalDirection;
	int32 iSimpleComboState;

	void SwitchCrouching();
	void PlayerJump();
	void SimpleAttack();
	void LedderMovingMode();
	void FlipbookAnimationFinished();

	/** Called for side to side input */
	void CharacterMoveLR(float Value);


	/*Jerk realisation*/
	FTimerHandle    JerkTimeoutTimer;
	FTimerHandle    CanJerkTimer;
	int iJerkState = -1;
	bool bCanJerk = true;

	void MakeJerk(float jerkForce);

	void JerkRight() { MakeJerk(500.f); }
	void JerkLeft()  { MakeJerk(500.f); }

	void TraceRay(FHitResult &hitResult, bool &traceResult, FVector positionFrom, FVector positionTo);

	void ThroughTheFlor();
	FTimerHandle ThroughTheFlorTimer;
	int iThroughFlor = -1;

	//wall jump mechanic
	bool TryWallJump();
	bool bIsWallJumping = false;
	bool bOnGround = true;
	bool bPostWallJump = false;
	float fCharacterMoveDirection = 0.f;
	FVector vInWJLocation;

	EPlayerState PlayerState;

private:
	//Climbing mechanic
	void CharacterMoveUD(float Value);

	bool bIsClimbing, bIsClimbingMove;
	float fClibmAx;
	float fDeltaTime;

};
