// Fill out your copyright notice in the Description page of Project Settings.


#include "RA_Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "../GameLogic/Actors/RA_DamageActor.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

FName ARA_Character::SpriteComponentName(TEXT("Sprite0"));

// Sets default values
ARA_Character::ARA_Character(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<URA_CharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
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

	Sprite = CreateOptionalDefaultSubobject<UPaperFlipbookComponent>(ARA_Character::SpriteComponentName);
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

void ARA_Character::PostInitializeComponents()
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

// Called when the game starts or when spawned
void ARA_Character::BeginPlay()
{
	Super::BeginPlay();

	CMComp = Cast<URA_CharacterMovementComponent>(GetCharacterMovement());

	CMComp->Initialize();

	Sprite->SetLooping(true);
}

void ARA_Character::UpdateAnimation()
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

		if (CMComp->IsClimbing())
			PlayerState = EPlayerState::PLAYER_CLIMB_IDLE;
		if (CMComp->IsClimbing() && CMComp->IsClimbingMoving())
			PlayerState = EPlayerState::PLAYER_CLIMB_MOVE;
	}
	else
	{
		PlayerState = EPlayerState::PLAYER_GROUND_ATTACK_0;
	}

	Sprite->SetFlipbook(AnimationSet[PlayerState]);
}

// Called every frame
void ARA_Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CMComp->SetDeltaTime(DeltaTime);
	UpdateCharacter();

	CMComp->cgiGameInstance->RegenerationHandler();

	UWorld* CurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	if (CurWorld)
	{
		UCapsuleComponent* uCapComp = GetCapsuleComponent();
		DrawDebugCapsule(CurWorld, uCapComp->GetCenterOfMass(), uCapComp->GetScaledCapsuleHalfHeight(), uCapComp->GetScaledCapsuleRadius(), uCapComp->GetComponentQuat(), FColor::Blue);
	}
}

// Called to bind functionality to input
void ARA_Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARA_Character::PlayerJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ARA_Character::SwitchCrouching);
	PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ARA_Character::JerkRight);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ARA_Character::JerkLeft);
	PlayerInputComponent->BindAction("MoveUp", IE_Pressed, this, &ARA_Character::JerkUp);
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ARA_Character::JerkDown);
	//PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &URA_CharacterMovementComponent::ThroughTheFlor);
	PlayerInputComponent->BindAxis("CharacterMoveLR", this, &ARA_Character::CharacterMoveLR);
	PlayerInputComponent->BindAxis("CharacterMoveUD", this, &ARA_Character::CharacterMoveUD);

	PlayerInputComponent->BindAction("SimpleAttack", IE_Pressed, this, &ARA_Character::SimpleAttack);
	PlayerInputComponent->BindAction("StrongAttack", IE_Pressed, this, &ARA_Character::SwitchCrouching);
}

float ARA_Character::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, class AActor* DamageCauser)
{
	const float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	CMComp->cgiGameInstance->AcceptPlayerDamage(DamageAmount);
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Damaged!");
	return 0;
}

void ARA_Character::OnAnimationFinished()
{
	float fuckMyBrain = 0.f;
}

void ARA_Character::CharacterMoveLR(float Value)
{
	/*UpdateChar();*/
	CMComp->SetHorisontalDirection(Value);

	if (CMComp->IsClimbing())
		CMComp->LedderMovingMode();
	// Apply the input to the character motion
	if (!bIsSimpleAttacking && !CMComp->IsClimbing())
		AddMovementInput(FVector(1.0f, 0.0f, 0.0f), Value);
	else
		CMComp->Velocity = FVector(1.f, 0.f, 0.f) * Value;
}

void ARA_Character::CharacterMoveUD(float Value)
{
	CMComp->SetVerticalDirection(Value);
	CMComp->LedderMovingMode();
}

void ARA_Character::SwitchCrouching()
{
	if (CMComp->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ARA_Character::PlayerJump()
{
	FVector vActorPosition = GetActorLocation();
	bool bTraceResult;
	FHitResult hrPlatrofm;

	if (!CMComp->IsClimbing() && CMComp->cgiGameInstance->IsStaminaEnought(1.f))
	{
		if (CMComp->IsCrouching())
			UnCrouch();

		if (!CMComp->TryWallJump())
		{
			FVector vTracePosition = FVector(0.f, 0.f, 30.f) + vActorPosition;
			CMComp->TraceRay(hrPlatrofm, bTraceResult, vActorPosition, vTracePosition);
			if (bTraceResult && hrPlatrofm.GetActor()->ActorHasTag("flor"))
			{
				FVector vMoveTo = FVector(0.f, 0.f, 100.f);
				CMComp->AddImpulse(vMoveTo);
			}
			else
			{
				if (JumpCurrentCount < JumpMaxCount)
				{
					Jump();
					CMComp->cgiGameInstance->WastePlayerStaminaIfPossible(1.f);
				}
				CMComp->LedderMovingMode();
			}
		}
	}
}

void ARA_Character::SimpleAttack()
{
	FVector vActorDirection;

	if (bCanAttack && CMComp->cgiGameInstance->WastePlayerStaminaIfPossible(20.f))
	{
		if (iSimpleComboState < 3)
		{
			UWorld* wCurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
			FVector vActorPosition = GetActorLocation();

			if (CMComp->GetHorisontalDirection() != 0.f || CMComp->GetVerticalDirection() != 0.f)
				vActorDirection = FVector(10.f * CMComp->GetHorisontalDirection(), 0.f, 10.f * CMComp->GetVerticalDirection());
			else
				vActorDirection = FVector(10.f * CMComp->GetMoveDirection(), 0.f, 10.f * CMComp->GetVerticalDirection());

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

				CMComp->GravityScale = 1.5f;
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

void ARA_Character::UpdateCharacter()
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
			CMComp->SetMoveDirection(-1.f);
		}
		else if (TravelDirection > 0.0f)
		{
			Controller->SetControlRotation(FRotator(0.0f, 0.0f, 0.0f));
			CMComp->SetMoveDirection(1.f);
		}
	}

	if (CMComp->IsFalling() && !CMComp->IsClimbing())
	{
		CMComp->LedderMovingMode();
	}
}

ARacoonAdventureGameMode* ARA_Character::GetCurrentGamemode()
{
	return (ARacoonAdventureGameMode*)GetWorld()->GetAuthGameMode();
}