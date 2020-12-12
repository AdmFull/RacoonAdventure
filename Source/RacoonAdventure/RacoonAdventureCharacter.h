// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "RacoonAdventureCharacter.generated.h"

class UTextRenderComponent;

UENUM(BlueprintType)
enum class EPlayerState : uint8 {
	PLAYER_IDLE,
	PLAYER_RUNNUNG,
	PLAYER_JUMP,
	PLAYER_FALLING,
	PLAYER_CROUCH_IDLE,
	PLAYER_CROUCH_MOVE,
	PLAYER_SIMPLE_ATTACK,
	PLAYER_STRONG_ATTACK
};

UENUM(BlueprintType)
enum class EPlayerStats : uint8 {
	PLAYER_STRENGTH,
	PLAYER_ENDURANCE,
	PLAYER_CHARISMA,
	PLAYER_INTELLIGENCE,
	PLAYER_AGILITY
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

	UTextRenderComponent* TextComponent;
	virtual void Tick(float DeltaSeconds) override;
protected:
	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* IdleAnimation;

	// The animation to play while running around
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Animations)
	class UPaperFlipbook* RunningAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* JumpAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* FallAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* CrouchIdleAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* CrouchWalkAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* SimpleAtackAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* SimpleAtackAnimation2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* SimpleAtackAnimation3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* SimpleAtackAnimationAir;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* SimpleAtackAnimationAir2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* SimpleAtackAnimationAir3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* StrongAtackAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* SpellAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* BlockAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* ParryAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* PickUpAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* ClimbAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* UseAnimation;

	// The animation to play while idle (standing still)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
	class UPaperFlipbook* RollAnimation;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	/** Called for side to side input */
	void MoveRight(float Value);

	void UpdateCharacter();

	/** Handle touch inputs. */
	void TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location);

	/** Handle touch stop event. */
	void TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	ARacoonAdventureCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

private:
	//Character bars
	float fPlayerHP;
	float fPlayerMana;
	float fPlayerStamina;

	int32 uiStrength;
	int32 uiEndurance;
	int32 uiCharisma;
	int32 uiIntelligence;
	int32 uiAgility;
public:
	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
	float GetPlayerHP()			{ return fPlayerHP; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
	float GetPlayerMana()		{ return fPlayerMana; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
	float GetPlayerStamina()	{ return fPlayerStamina; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
	void SetPlayerHP(float newHP)			{ fPlayerHP = newHP; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
	void SetPlayerMana(float newMana)		{ fPlayerMana = newMana; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
	void SetPlayerStamina(float newStamina) { fPlayerStamina = newStamina; }

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
	int32 GetPlayerStat(EPlayerStats eStat);

	UFUNCTION(BlueprintCallable, Category = PlayerLifeState)
	void SetPlayerStat(int32 uiNewStat, EPlayerStats eStat);


private:
	FTimerHandle    AnimationDelayTimer;
	FTimerHandle    ComboAttackTimer;
	bool bIsSimpleAttacking;
	bool bIsJumpUp;
	int32 iSimpleComboState;

	void SwitchCrouching();
	void PlayerJump();
	void SimpleAttack();
	void FlipbookAnimationFinished();
	EPlayerState PlayerState;
};
