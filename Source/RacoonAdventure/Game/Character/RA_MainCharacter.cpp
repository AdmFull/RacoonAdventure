// Fill out your copyright notice in the Description page of Project Settings.


#include "RA_MainCharacter.h"

ARA_MainCharacter::ARA_MainCharacter()
{

}

void ARA_MainCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void ARA_MainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARA_MainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Note: the 'Jump' action and the 'MoveRight' axis are bound to actual keys/buttons/sticks in DefaultInput.ini (editable from Project Settings..Input)
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ARA_BaseCharacter::PlayerJump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ARA_BaseCharacter::SwitchCrouching);
	/*PlayerInputComponent->BindAction("MoveRight", IE_Pressed, this, &ARA_Character::JerkRight);
	PlayerInputComponent->BindAction("MoveLeft", IE_Pressed, this, &ARA_Character::JerkLeft);
	PlayerInputComponent->BindAction("MoveUp", IE_Pressed, this, &ARA_Character::JerkUp);
	PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &ARA_Character::JerkDown);
	PlayerInputComponent->BindAction("Interact", IE_Pressed, this, &ARA_Character::Interact);*/
	//PlayerInputComponent->BindAction("MoveDown", IE_Pressed, this, &URA_CharacterMovementComponent::ThroughTheFlor);
	PlayerInputComponent->BindAxis("CharacterMoveLR", this, &ARA_BaseCharacter::AddHorisontalMovement);
	PlayerInputComponent->BindAxis("CharacterMoveUD", this, &ARA_BaseCharacter::AddVerticalMovement);

	PlayerInputComponent->BindAction("SimpleAttack", IE_Pressed, this, &ARA_BaseCharacter::SimpleAttack);
	PlayerInputComponent->BindAction("StrongAttack", IE_Pressed, this, &ARA_BaseCharacter::SwitchCrouching);

}