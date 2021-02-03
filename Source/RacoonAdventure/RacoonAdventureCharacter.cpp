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
#include "RA_DamageActor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

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
		PlayerState = EPlayerState::PLAYER_GROUND_ATTACK_0;
	}

	GetSprite()->SetFlipbook(AnimationSet[PlayerState]);
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
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ARacoonAdventureCharacter::JerkRight);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ARacoonAdventureCharacter::JerkLeft);
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ARacoonAdventureCharacter::ThroughTheFlor);
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
	FVector vActorPosition = GetActorLocation();
	bool bTraceResult;
	FHitResult hrPlatrofm;

	if (!bIsClimbing)
	{
		if (GetCharacterMovement()->IsCrouching())
			UnCrouch();

		if (!TryWallJump())
		{
			FVector vTracePosition = FVector(0.f, 0.f, 30.f) + vActorPosition;
			TraceRay(hrPlatrofm, bTraceResult, vActorPosition, vTracePosition);
			if (bTraceResult && hrPlatrofm.GetActor()->ActorHasTag("flor"))
			{
				FVector vMoveTo = FVector(0.f, 0.f, 100.f);
				GetCharacterMovement()->AddImpulse(vMoveTo);
			}
			else
			{
				Jump();
				LedderMovingMode();
			}
		}
		else
		{

		}
	}
}

void ARacoonAdventureCharacter::SimpleAttack()
{
	float fAttackPower = 1.0;

	if (bCanAttack)
	{
		if (iSimpleComboState < 3)
		{
			UWorld* wCurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
			FVector vActorPosition = GetActorLocation();
			FVector vActorDirection = FVector(50.f * fCharacterMoveDirection, 0.f, 50.f * fVerticalDirection);
			FVector vNewActorPosition(vActorPosition + vActorDirection);


			if (DamageActorBlueprintPtr)
			{
				FActorSpawnParameters SpawnInfo;
				SpawnInfo.Owner = this;
				SpawnInfo.Instigator = UGameplayStatics::GetPlayerPawn(this, 0);
				ARA_DamageActor* aSpawningObject = wCurWorld->SpawnActor<ARA_DamageActor>(DamageActorBlueprintPtr, vNewActorPosition, FRotator(0.f, 0.f, 0.f), SpawnInfo);
				if (aSpawningObject)
				{
					aSpawningObject->InitializeDamage(vActorDirection, 10.f, 10.f, 0.5);
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

//Ladder mode worker
void ARacoonAdventureCharacter::LedderMovingMode()
{
	FHitResult hrLadder, hrGround;
	FVector vActorPosition = GetActorLocation();
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
				if (hrLadder.GetActor()->ActorHasTag("ladder"))
				{
					bIsClimbing = true;
					FVector vHitActorLocation = hrLadder.GetActor()->GetActorLocation() + (FVector(5.f, 0.f, 0.f) * fHorisontalDirection);
					vHitActorLocation.Y = 0.f;
					SetActorLocation(vHitActorLocation);
					FVector vNewImpulse = FVector(0.f, 0.f, 50.f);
					GetCharacterMovement()->AddImpulse(vNewImpulse, true);
				}
			}
		}
	}
}

void ARacoonAdventureCharacter::MakeJerk(float jerkForce)
{
	if (bCanJerk)
	{
		if (iJerkState < 0) GetWorld()->GetTimerManager().SetTimer(JerkTimeoutTimer, [this]() { iJerkState = -1; }, 0.2f, 1);
		if (++iJerkState > 0)
		{
			LaunchCharacter(FVector(jerkForce * fCharacterMoveDirection, 0.f, 0.f), false, true);
			iJerkState = -1;
			bCanJerk = false;
			GetWorld()->GetTimerManager().SetTimer(CanJerkTimer, [this]() { bCanJerk = true; }, 0.6f, 1);
		}
	}
}

void ARacoonAdventureCharacter::TraceRay(FHitResult &hitResult, bool &traceResult, FVector positionFrom, FVector positionTo)
{

	UWorld* wCurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (wCurWorld)
	{
		DrawDebugLine(wCurWorld, positionFrom, positionTo, FColor::Cyan, false, 1.f);
		traceResult = wCurWorld->LineTraceSingleByChannel(hitResult, positionFrom, positionTo, ECollisionChannel::ECC_Visibility);
	}
}

void ARacoonAdventureCharacter::ThroughTheFlor()
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
			SetActorLocation(vNewActorLocation);
		}
	}
}

bool ARacoonAdventureCharacter::TryWallJump()
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
			if (GetCharacterMovement()->IsFalling())
			{
				bIsWallJumping = bPostWallJump = true;
				vInWJLocation = vActorPosition;
				if (vActorPosition.X > hrWall.GetActor()->GetActorLocation().X)
					vInWJDirection = FVector(500.f, 0.f, 500.f);
				else
					vInWJDirection = FVector(-500.f, 0.f, 500.f);

				LaunchCharacter(vInWJDirection, true, true);
				bIsWallJumping = bPostWallJump = false;
				return true;
			}
		}
	}
	return false;
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
			fCharacterMoveDirection = -1.f;
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
			fCharacterMoveDirection = 1.f;
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