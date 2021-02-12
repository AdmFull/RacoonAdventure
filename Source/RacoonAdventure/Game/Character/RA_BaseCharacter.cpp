// Fill out your copyright notice in the Description page of Project Settings.


#include "RA_BaseCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "../GameLogic/Actors/Interactable/RA_DamageActor.h"
#include "../GameLogic/Actors/Interactable/RA_InteractableActor.h"
#include "../GameLogic/Actors/Interactable/RA_InteractableSaveActor.h"
#include "../GameLogic/Actors/Interactable/RA_WallJumpableActor.h"
#include "../GameLogic/Actors/Interactable/RA_LedderActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ARA_BaseCharacter::ARA_BaseCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Use only Yaw from the controller and ignore the rest of the rotation.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Set the size of our collision capsule.
	GetCapsuleComponent()->SetCapsuleHalfHeight(96.0f);
	GetCapsuleComponent()->SetCapsuleRadius(40.0f);

	// Create a camera boom attached to the root (capsule)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 500.0f;
	CameraBoom->SocketOffset = FVector(0.0f, 0.0f, 75.0f);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->SetRelativeRotation(FRotator(0.0f, -90.0f, 0.0f));


	// Create an orthographic camera (no perspective) and attach it to the boom
	SideViewCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SideViewCamera"));
	SideViewCameraComponent->ProjectionMode = ECameraProjectionMode::Orthographic;
	SideViewCameraComponent->OrthoWidth = 2048.0f;
	SideViewCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);

	// Prevent all automatic rotation behavior on the camera, character, and camera component
	CameraBoom->SetUsingAbsoluteRotation(true);
	SideViewCameraComponent->bUsePawnControlRotation = false;
	SideViewCameraComponent->bAutoActivate = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	// Configure character movement
	GetCharacterMovement()->GravityScale = 2.0f;
	GetCharacterMovement()->AirControl = 0.30f;
	GetCharacterMovement()->JumpZVelocity = 400.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 300.0f;
	GetCharacterMovement()->MaxFlySpeed = 300.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

	Sprite = CreateOptionalDefaultSubobject<UPaperFlipbookComponent>(TEXT("PaperFlipbook"));
	if (Sprite)
	{
		Sprite->AlwaysLoadOnClient = true;
		Sprite->AlwaysLoadOnServer = true;
		Sprite->bOwnerNoSee = false;
		Sprite->bAffectDynamicIndirectLighting = true;
		Sprite->PrimaryComponentTick.TickGroup = TG_PrePhysics;
		Sprite->SetupAttachment(GetCapsuleComponent());
		static FName CollisionProfileName(TEXT("CharacterMesh"));
		Sprite->SetCollisionProfileName(CollisionProfileName);
		Sprite->SetGenerateOverlapEvents(false);
		// Enable replication on the Sprite component so animations show up when networked
		Sprite->SetIsReplicated(true);
	}
}

void ARA_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* CurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (CurWorld)
	{
		if (CurWorld->GetGameInstance() != nullptr)
		{
			//Contains all game parameters
			pGameInstance = Cast<URacoonAdventureGameInstance>(CurWorld->GetGameInstance());
		}
	}

	Sprite->SetLooping(true);
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsPendingKill())
	{
		if (Sprite)
		{
			// force animation tick after movement component updates
			if (Sprite->PrimaryComponentTick.bCanEverTick && GetCharacterMovement())
			{
				Sprite->PrimaryComponentTick.AddPrerequisite(GetCharacterMovement(), GetCharacterMovement()->PrimaryComponentTick);
			}
		}
	}
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	if (!bIsSimpleAttacking)
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			if (!GetCharacterMovement()->IsCrouching())
			{
				if (PlayerSpeedSqr > 0.0f)
					PlayerState = EPlayerState::PLAYER_RUNNUNG;
				else
					PlayerState = EPlayerState::PLAYER_IDLE;
			}
			else
			{
				if (PlayerSpeedSqr > 0.0f)
					PlayerState = EPlayerState::PLAYER_CROUCH_MOVE;
				else
					PlayerState = EPlayerState::PLAYER_CROUCH_IDLE;
			}
		}
		else
		{
			if (PlayerVelocity.Z > 0.0f)
				PlayerState = EPlayerState::PLAYER_JUMP;
			if (PlayerVelocity.Z < 0.0f && !bIsJumpUp)
				PlayerState = EPlayerState::PLAYER_FALLING;
		}

		if (IsClimbing())
			PlayerState = EPlayerState::PLAYER_CLIMB_IDLE;
		if (IsClimbing() && IsClimbingMoving())
			PlayerState = EPlayerState::PLAYER_CLIMB_MOVE;
	}
	else
	{
		PlayerState = EPlayerState::PLAYER_GROUND_ATTACK_0;
	}

	Sprite->SetFlipbook(AnimationSet[PlayerState]);
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
float ARA_BaseCharacter::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	pGameInstance->AcceptPlayerDamage(DamageAmount);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Damaged!");
	return 0;
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::OnAnimationFinished()
{
	float fuckMyBrain = 0.f;
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::AddHorisontalMovement(float Value)
{
	fHorisontalDirection = Value;
	AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::AddVerticalMovement(float Value)
{
	fVerticalDirection = Value;
	AddMovementInput(FVector(0.0f, 0.0f, 1.0f), Value);
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::SwitchCrouching()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::PlayerJump()
{
	if (!IsClimbing() && pGameInstance->IsStaminaEnought(1.f))
	{
		if (GetCharacterMovement()->IsCrouching())
			UnCrouch();

		//Interact();
		if (JumpCurrentCount < JumpMaxCount)
		{
			Jump();
			pGameInstance->WastePlayerStaminaIfPossible(1.f);
		}
		//CMComp->LedderMovingMode();
	}
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::SimpleAttack()
{
	FVector vActorDirection;

	if (bCanAttack && pGameInstance->WastePlayerStaminaIfPossible(20.f))
	{
		if (iSimpleComboState < 3)
		{
			UWorld* wCurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
			FVector vActorPosition = GetActorLocation();

			/*if (CMComp->GetHorisontalDirection() != 0.f || CMComp->GetVerticalDirection() != 0.f)
				vActorDirection = FVector(10.f * CMComp->GetHorisontalDirection(), 0.f, 10.f * CMComp->GetVerticalDirection());
			else
				vActorDirection = FVector(10.f * CMComp->GetMoveDirection(), 0.f, 10.f * CMComp->GetVerticalDirection());*/

			FVector vNewActorPosition(vActorPosition + vActorDirection);

			DrawDebugLine(wCurWorld, vActorPosition, vNewActorPosition, FColor::Red, false, 1.f);

			if (DamageActorBlueprintPtr)
			{
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.Owner = this;
				ARA_DamageActor* aSpawningObject = wCurWorld->SpawnActor<ARA_DamageActor>(DamageActorBlueprintPtr, vNewActorPosition, FRotator(0.f, 0.f, 0.f), SpawnInfo);
				if (aSpawningObject)
				{
					aSpawningObject->InitializeDamage(vActorDirection, 10.f, 5.f, 2.01);
				}

				GetCharacterMovement()->GravityScale = 1.5f;
				iSimpleComboState++;
				GetWorld()->GetTimerManager().SetTimer(ComboAttackTimer, [this]()
				{
					iSimpleComboState = -1;
					GetCharacterMovement()->GravityScale = 2.f;
					bIsSimpleAttacking = false;
				}, 0.4f, 1);
			}
		}
		GetWorld()->GetTimerManager().SetTimer(AnimationDelayTimer, [this]() { bIsSimpleAttacking = false; bCanAttack = true; }, 0.25f + 0.05f * iSimpleComboState, 1);

		bIsSimpleAttacking = true;
	}
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::UpdateCharacter()
{
	// Update animation to match the motion
	UpdateAnimation();

	// Now setup the rotation of the controller based on the direction we are travelling
	const FVector PlayerVelocity = GetVelocity();
	float TravelDirection = PlayerVelocity.X;
	// Set the rotation so that the character faces his direction of travel.
	if (Controller != nullptr)
	{
		if (TravelDirection < 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0, 180.0f, 0.0f));
			fCharacterMoveDirection = -1.f;
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
			fCharacterMoveDirection = 1.f;
		}
	}

	if (GetCharacterMovement()->IsFalling() && !IsClimbing())
	{
		//CMComp->LedderMovingMode();
	}
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::BeginInteract(AActor* InteractableActor)
{
	pInteractable = Cast<ARA_LedderActor>(InteractableActor);
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::EndInteract(AActor* InteractableActor)
{
	pInteractable = nullptr;
}

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
void ARA_BaseCharacter::Interact()
{
	if (pInteractable)
	{
		pInteractable->OnInteract(this);
	}
}