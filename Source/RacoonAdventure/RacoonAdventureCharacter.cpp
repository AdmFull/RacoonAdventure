// Copyright Epic Games, Inc. All Rights Reserved.

#include "RacoonAdventureCharacter.h"
#include "PaperFlipbookComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"

DEFINE_LOG_CATEGORY_STATIC(SideScrollerCharacter, Log, All);

//////////////////////////////////////////////////////////////////////////
// ARacoonAdventureCharacter

ARacoonAdventureCharacter::ARacoonAdventureCharacter()
{
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
	GetCharacterMovement()->AirControl = 0.80f;
	GetCharacterMovement()->JumpZVelocity = 1000.f;
	GetCharacterMovement()->GroundFriction = 3.0f;
	GetCharacterMovement()->MaxWalkSpeed = 600.0f;
	GetCharacterMovement()->MaxFlySpeed = 600.0f;

	// Lock character motion onto the XZ plane, so the character can't move in or out of the screen
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->SetPlaneConstraintNormal(FVector(0.0f, -1.0f, 0.0f));

	// Behave like a traditional 2D platformer character, with a flat bottom instead of a curved capsule bottom
	// Note: This can cause a little floating when going up inclines; you can choose the tradeoff between better
	// behavior on the edge of a ledge versus inclines by setting this to true or false
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;

    // 	TextComponent = CreateDefaultSubobject<UTextRenderComponent>(TEXT("IncarGear"));
    // 	TextComponent->SetRelativeScale3D(FVector(3.0f, 3.0f, 3.0f));
    // 	TextComponent->SetRelativeLocation(FVector(35.0f, 5.0f, 20.0f));
    // 	TextComponent->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
    // 	TextComponent->SetupAttachment(RootComponent);

	// Enable replication on the Sprite component so animations show up when networked
	GetSprite()->SetIsReplicated(true);
	bReplicates = true;
	bIsJumpUp = false;
	bIsSimpleAttacking = false;

	fPlayerHP = 20.f;
	fPlayerMana = 20.f;
	fPlayerStamina = 20.f;

	iSimpleComboState = -1;

	PlayerState = EPlayerState::PLAYER_IDLE;
}

//////////////////////////////////////////////////////////////////////////
// Animation

void ARacoonAdventureCharacter::UpdateAnimation()
{
	const FVector PlayerVelocity = GetVelocity();
	const float PlayerSpeedSqr = PlayerVelocity.SizeSquared();

	UPaperFlipbook* DesiredAnimation = IdleAnimation;

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
		else if (GetCharacterMovement()->IsFalling())
		{
			if (PlayerVelocity.Z > 0.0f)
				PlayerState = EPlayerState::PLAYER_JUMP;
			if (PlayerVelocity.Z < 0.0f && !bIsJumpUp)
				PlayerState = EPlayerState::PLAYER_FALLING;
		}
	}
	else
	{
		PlayerState = EPlayerState::PLAYER_SIMPLE_ATTACK;
	}

	switch (PlayerState)
	{
	case EPlayerState::PLAYER_IDLE:
		DesiredAnimation = IdleAnimation;
		break;

	case EPlayerState::PLAYER_RUNNUNG:
		DesiredAnimation = RunningAnimation;
		break;
	case EPlayerState::PLAYER_JUMP:
		DesiredAnimation = JumpAnimation;
		break;
	case EPlayerState::PLAYER_FALLING:
		DesiredAnimation = FallAnimation;
		break;
	case EPlayerState::PLAYER_CROUCH_IDLE:
		DesiredAnimation = CrouchIdleAnimation;
		break;
	case EPlayerState::PLAYER_CROUCH_MOVE:
		DesiredAnimation = CrouchWalkAnimation;
		break;
	case EPlayerState::PLAYER_SIMPLE_ATTACK:
		if (!GetCharacterMovement()->IsFalling())
		{
			switch (iSimpleComboState)
			{
			case 0: { DesiredAnimation = SimpleAtackAnimation; } break;
			case 1: { DesiredAnimation = SimpleAtackAnimation2; } break;
			case 2: { DesiredAnimation = SimpleAtackAnimation3; } break;
			}
		}
		else
		{
			switch (iSimpleComboState)
			{
			case 0: { DesiredAnimation = SimpleAtackAnimationAir; } break;
			case 1: { DesiredAnimation = SimpleAtackAnimationAir2; } break;
			case 2: { DesiredAnimation = SimpleAtackAnimationAir3; } break;
			}
		}
		break;

	default:
		break;
	}

	if( GetSprite()->GetFlipbook() != DesiredAnimation 	)
	{
		GetSprite()->SetFlipbook(DesiredAnimation);
	}
}

void ARacoonAdventureCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateCharacter();	
}


//////////////////////////////////////////////////////////////////////////
// Input

void ARacoonAdventureCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARacoonAdventureCharacter::PlayerJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ARacoonAdventureCharacter::SwitchCrouching);
	PlayerInputComponent->BindAxis("MoveRight", this, &ARacoonAdventureCharacter::MoveRight);

	PlayerInputComponent->BindAction("SimpleAttack", IE_Pressed, this, &ARacoonAdventureCharacter::SimpleAttack);
	PlayerInputComponent->BindAction("StrongAttack", IE_Pressed, this, &ARacoonAdventureCharacter::SwitchCrouching);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &ARacoonAdventureCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ARacoonAdventureCharacter::TouchStopped);
}

void ARacoonAdventureCharacter::MoveRight(float Value)
{
	/*UpdateChar();*/

	// Apply the input to the character motion
	if(!bIsSimpleAttacking)
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
}

void ARacoonAdventureCharacter::TouchStarted(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Jump on any touch
	Jump();
}

void ARacoonAdventureCharacter::TouchStopped(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	// Cease jumping once touch stopped
	StopJumping();
}

void ARacoonAdventureCharacter::SwitchCrouching() 
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

void ARacoonAdventureCharacter::PlayerJump()
{
	if (GetCharacterMovement()->IsCrouching())
		UnCrouch();
	Jump();
	bIsJumpUp = true;
	GetWorld()->GetTimerManager().SetTimer(AnimationDelayTimer, [this]() { bIsJumpUp = false; }, 0.3f, 1);
}

void ARacoonAdventureCharacter::SimpleAttack()
{
	float fAttackPower = 1.0;
	if (!bIsSimpleAttacking && iSimpleComboState < 3)
	{
		if(iSimpleComboState < 0) GetWorld()->GetTimerManager().SetTimer(ComboAttackTimer, [this]() { iSimpleComboState = -1; }, 1.0f, 1);
		if (iSimpleComboState < 3) iSimpleComboState++;
		GetWorld()->GetTimerManager().SetTimer(AnimationDelayTimer, [this]() { bIsSimpleAttacking = false; }, 0.25f + 0.05f* iSimpleComboState, 1);
		
		bIsSimpleAttacking = true;
	}
}

void ARacoonAdventureCharacter::SetPlayerStat(int32 uiNewStat, EPlayerStats eStat)
{
	switch (eStat)
	{
	case EPlayerStats::PLAYER_STRENGTH:		uiStrength = uiNewStat; break;
	case EPlayerStats::PLAYER_ENDURANCE:		uiEndurance = uiNewStat; break;
	case EPlayerStats::PLAYER_CHARISMA:		uiCharisma = uiNewStat; break;
	case EPlayerStats::PLAYER_INTELLIGENCE:	uiIntelligence = uiNewStat; break;
	case EPlayerStats::PLAYER_AGILITY:		uiAgility = uiNewStat; break;
	default:   break;
	}
}

int32 ARacoonAdventureCharacter::GetPlayerStat(EPlayerStats eStat)
{
	switch (eStat)
	{
	case EPlayerStats::PLAYER_STRENGTH:		return uiStrength; break;
	case EPlayerStats::PLAYER_ENDURANCE:	return uiEndurance; break;
	case EPlayerStats::PLAYER_CHARISMA:		return uiCharisma; break;
	case EPlayerStats::PLAYER_INTELLIGENCE:	return uiIntelligence; break;
	case EPlayerStats::PLAYER_AGILITY:		return uiAgility; break;
	default:								return 999999; break;
	}
}

void ARacoonAdventureCharacter::UpdateCharacter()
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
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
		}
	}
}
