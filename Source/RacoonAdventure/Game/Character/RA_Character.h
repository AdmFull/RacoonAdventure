// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "GameFramework/Character.h"
#include "Templates/SubclassOf.h"
#include "../GameLogic/RacoonAdventureGameMode.h"
#include "RA_CharacterMovementComponent.h"
#include "RA_Character.generated.h"

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

UCLASS(config = Game)
class RACOONADVENTURE_API ARA_Character : public ACharacter
{
	GENERATED_BODY()

	// Name of the Sprite component
	static FName SpriteComponentName;

	/** Side view camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* SideViewCameraComponent;

	/** Camera boom positioning the camera beside the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

public:
	// Sets default values for this character's properties
	ARA_Character(const FObjectInitializer& ObjectInitializer);

	/** Returns SideViewCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetSideViewCameraComponent() const { return SideViewCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void PostInitializeComponents() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Animations)
		TMap<EPlayerState, class UPaperFlipbook*> AnimationSet;

	UPROPERTY(EditAnywhere, Category = Animations)
		TSubclassOf<class ARA_DamageActor> DamageActorBlueprintPtr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Components)
		URA_CharacterMovementComponent* CMComp;

	/** Called to choose the correct animation to play based on the character's movement state */
	void UpdateAnimation();

	void UpdateCharacter();

	UFUNCTION()
		virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser) override;

	UFUNCTION()
		void OnAnimationFinished();

	/** Returns Sprite subobject **/
	FORCEINLINE class UPaperFlipbookComponent* GetSprite() const { return Sprite; }

	UFUNCTION(BlueprintCallable, Category = "Game")
		ARacoonAdventureGameMode* GetCurrentGamemode();

private:
	/** The main skeletal mesh associated with this Character (optional sub-object). */
	UPROPERTY(Category = Character, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UPaperFlipbookComponent* Sprite;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	EPlayerState PlayerState;

private:

	FTimerHandle    AnimationDelayTimer;
	FTimerHandle    ComboAttackTimer;
	bool bIsSimpleAttacking = false;
	bool bCanAttack = true;
	bool bIsJumpUp = false;
	bool bWantToJump = false;
	int32 iSimpleComboState = 0;

	void SwitchCrouching();
	void PlayerJump();
	void SimpleAttack();

	/** Called for side to side input */
	void CharacterMoveLR(float Value);

	void JerkRight() { CMComp->MakeJerk(500.f, 1.f, 0.f); }
	void JerkLeft() { CMComp->MakeJerk(500.f, -1.f, 0.f); }
	void JerkUp() { CMComp->MakeJerk(500.f, 0.f, 1.f); }
	void JerkDown() { CMComp->MakeJerk(1000.f, 0.f, -1.f); }

private:
	//Climbing mechanic
	void CharacterMoveUD(float Value);

};
