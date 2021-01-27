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

	iSimpleComboState = -1;

	PlayerState = EPlayerState::PLAYER_IDLE;
}

void ARacoonAdventureCharacter::BeginPlay()
{
	Super::BeginPlay();

	UWorld* CurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (CurWorld)
	{
		if (CurWorld->GetGameInstance() != nullptr)
		{
			//Contains all game parameters
			cgiGameInstance = Cast<URacoonAdventureGameInstance>(CurWorld->GetGameInstance());
		}
	}
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
		else
		{
			if (PlayerVelocity.Z > 0.0f)
				PlayerState = EPlayerState::PLAYER_JUMP;
			if (PlayerVelocity.Z < 0.0f && !bIsJumpUp)
				PlayerState = EPlayerState::PLAYER_FALLING;
		}

		if (bIsClimbing)
			PlayerState = EPlayerState::PLAYER_CLIMB_IDLE;
		if (bIsClimbing && bIsClimbingMove)
			PlayerState = EPlayerState::PLAYER_CLIMB_MOVE;
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

	case EPlayerState::PLAYER_CLIMB_IDLE:
		DesiredAnimation = ClimbIdleAnimation;
		break;

	case EPlayerState::PLAYER_CLIMB_MOVE:
		DesiredAnimation = ClimbMoveAnimation;
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
	fDeltaTime = DeltaSeconds;
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
	PlayerInputComponent->BindAxis("CharacterMoveLR", this, &ARacoonAdventureCharacter::CharacterMoveLR);
	PlayerInputComponent->BindAxis("CharacterMoveUD", this, &ARacoonAdventureCharacter::CharacterMoveUD);

	PlayerInputComponent->BindAction("SimpleAttack", IE_Pressed, this, &ARacoonAdventureCharacter::SimpleAttack);
	PlayerInputComponent->BindAction("StrongAttack", IE_Pressed, this, &ARacoonAdventureCharacter::SwitchCrouching);

	PlayerInputComponent->BindTouch(IE_Pressed, this, &ARacoonAdventureCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &ARacoonAdventureCharacter::TouchStopped);
}

void ARacoonAdventureCharacter::CharacterMoveLR(float Value)
{
	/*UpdateChar();*/
	fHorisontalDirection = Value;
	if (bIsClimbing)
		LedderMovingMode();
	// Apply the input to the character motion
	if(!bIsSimpleAttacking && !bIsClimbing)
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
	else
		GetCharacterMovement()->Velocity = FVector(1.f, 0.f, 0.f) * Value;
}

void ARacoonAdventureCharacter::CharacterMoveUD(float Value)
{
	fVerticalDirection = Value;
	LedderMovingMode();
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
	
	if (!bIsClimbing)
	{
		Jump();
		LedderMovingMode();
	}
}

void ARacoonAdventureCharacter::SimpleAttack()
{
	float fAttackPower = 1.0;
	//if (bIsSimpleAttacking)
	//{
		
		if(iSimpleComboState < 0) GetWorld()->GetTimerManager().SetTimer(ComboAttackTimer, [this]() { iSimpleComboState = -1; GetCharacterMovement()->GravityScale = 2.f; }, 1.0f, 1);
		if (iSimpleComboState < 3)
		{
			GetCharacterMovement()->GravityScale = 1.5f;
			iSimpleComboState++;
		}
		else
		{
			if (GetCharacterMovement()->IsFalling())
			{
				GetCharacterMovement()->GravityScale = 2.f;
				GetCharacterMovement()->AddImpulse(FVector(0.f, 0.f, -50000.f), true);
			}
		}
		GetWorld()->GetTimerManager().SetTimer(AnimationDelayTimer, [this]() { bIsSimpleAttacking = false; }, 0.25f + 0.05f* iSimpleComboState, 1);
		
		bIsSimpleAttacking = true;
	//}
}

//Ladder mode worker
void ARacoonAdventureCharacter::LedderMovingMode()
{
	FHitResult hrStairs, hrGround;
	UWorld* wCurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	FVector vActorPosition = GetActorLocation();
	FVector vStairsOutPosition = vActorPosition + FVector(0.f, -50.f, 0.f);
	bool bTraceResult;

	if (wCurWorld)
	{
		bTraceResult = wCurWorld->LineTraceSingleByChannel(hrStairs, vActorPosition, vStairsOutPosition, ECollisionChannel::ECC_Visibility);

		if (bIsClimbing)
		{
			if (fVerticalDirection == 0)
			{
				bIsClimbingMove = false;
			}

			if (bTraceResult && hrStairs.GetActor()->ActorHasTag("stairs"))
			{
				GetCharacterMovement()->Velocity = FVector(0.f, 0.f, 0.f);
				GetCharacterMovement()->GravityScale = 10.f / (cgiGameInstance->GetPlayerStat(EPlayerStats::PLAYER_STRENGTH) + cgiGameInstance->GetPlayerStat(EPlayerStats::PLAYER_ENDURANCE));
				if (fVerticalDirection != 0)
				{
					FVector vNewActorLocation = vActorPosition + (FVector(5.f, 0.f, 0.f) * fHorisontalDirection);
					vNewActorLocation.Z = vNewActorLocation.Z + (fVerticalDirection * GetCharacterMovement()->MaxWalkSpeed * fDeltaTime * 0.2f) / 2.5f;
					SetActorLocation(vNewActorLocation, true);
					bIsClimbingMove = true;

					//If want to jump, check direction to jump
					if (fHorisontalDirection != 0.f)
					{
						FVector vNewImpulse = FVector(100.f, 0.f, 0.f) * fHorisontalDirection;
						GetCharacterMovement()->AddImpulse(vNewImpulse, true);
					}
				}
			}
			else
			{
				GetCharacterMovement()->GravityScale = 2.f;
				FVector vNewImpulse = (FVector(0.f, 0.f, 300.f) + fVerticalDirection) + (FVector(50.f, 0.f, 0.f) * fHorisontalDirection);
				GetCharacterMovement()->AddImpulse(vNewImpulse, true);
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
					if (hrStairs.GetActor()->ActorHasTag("stairs"))
					{
						bIsClimbing = true;
						FVector vHitActorLocation = hrStairs.GetActor()->GetActorLocation() + (FVector(5.f, 0.f, 0.f) * fHorisontalDirection);
						vHitActorLocation.Y = 0.f;
						SetActorLocation(vHitActorLocation);
						FVector vNewImpulse = FVector(0.f, 0.f, 50.f);
						GetCharacterMovement()->AddImpulse(vNewImpulse, true);
					}
				}
			}
		}
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

	if (GetCharacterMovement()->IsFalling() && !bIsClimbing)
	{
		LedderMovingMode();
	}
}

ARacoonAdventureGameMode* ARacoonAdventureCharacter::GetCurrentGamemode()
{
	return (ARacoonAdventureGameMode*)GetWorld()->GetAuthGameMode();
}