// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Character.h"
#include "Templates/SubclassOf.h"
#include "../GameLogic/RacoonAdventureGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RA_BaseCharacter.generated.h"

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

UCLASS()
class RACOONADVENTURE_API ARA_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

public:
	// Sets default values for this character's properties
	ARA_BaseCharacter();

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	UFUNCTION(BlueprintCallable, Category = JumpMechanic)
		bool IsClimbing() { return bIsClimbing; }

	UFUNCTION(BlueprintCallable, Category = JumpMechanic)
		bool IsClimbingMoving() { return bIsClimbingMove; }

	/** Called to choose the correct animation to play based on the character's movement state */
	UFUNCTION()
		void UpdateAnimation();

	UFUNCTION()
		void UpdateCharacter();

	UFUNCTION(BlueprintCallable, Category = CharacterMovement)
		void AddHorisontalMovement(float Value);

	UFUNCTION(BlueprintCallable, Category = CharacterMovement)
		void AddVerticalMovement(float Value);

	UFUNCTION(BlueprintCallable, Category = Interaction)
		void BeginInteract(AActor* InteractableActor);

	UFUNCTION(BlueprintCallable, Category = Interaction)
		void EndInteract(AActor* InteractableActor);

	UFUNCTION(BlueprintCallable, Category = Interaction)
		void Interact();

	UFUNCTION(BlueprintCallable, Category = Action)
		void SwitchCrouching();

	UFUNCTION(BlueprintCallable, Category = Action)
		void PlayerJump();

	UFUNCTION(BlueprintCallable, Category = Action)
		void SimpleAttack();


	UFUNCTION(BlueprintCallable, Category = CharacterInfo)
		float GetHorisontalDirection() { return fHorisontalDirection; }

	UFUNCTION(BlueprintCallable, Category = CharacterInfo)
		float GetVerticalDirection() { return fVerticalDirection; }

	UFUNCTION(BlueprintCallable, Category = CharacterInfo)
		float GetCharacterMoveDirection() { return fCharacterMoveDirection; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		TMap<EPlayerState, class UPaperFlipbook*> AnimationSet;

	UPROPERTY(EditAnywhere, Category = Animations)
		TSubclassOf<class ARA_DamageActor> DamageActorBlueprintPtr;

	UFUNCTION()
		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	UFUNCTION()
		void OnAnimationFinished();

	/** Returns Sprite subobject **/
	FORCEINLINE class UPaperFlipbookComponent* GetSprite() const { return Sprite; }

private:
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UPaperFlipbookComponent* Sprite;

	UPROPERTY(VisibleAnywhere, Category = GameInstance)
		class URacoonAdventureGameInstance* pGameInstance;

	class ARA_LedderActor* pInteractable;

	EPlayerState PlayerState;

	float fHorisontalDirection = 0.f;

	float fVerticalDirection = 0.f;

	float fCharacterMoveDirection = 1.f;

	FTimerHandle    AnimationDelayTimer;
	FTimerHandle    ComboAttackTimer;

	bool bIsSimpleAttacking = false;
	bool bCanAttack = true;
	bool bIsJumpUp = false;
	bool bWantToJump = false;
	int32 iSimpleComboState = 0;

	bool bIsClimbing, bIsClimbingMove;
};
