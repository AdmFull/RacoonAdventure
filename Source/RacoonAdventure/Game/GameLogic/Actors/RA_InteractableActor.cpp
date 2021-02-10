// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/BoxComponent.h"
#include "RA_InteractableActor.h"

// Sets default values
ARA_InteractableActor::ARA_InteractableActor()
{
	eInteractionType = EInteractionType::EDefaultInteract;

	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBox"), true);
	BoxCollisionComponent->SetCollisionProfileName(FName("Trigger"));
	BoxCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARA_InteractableActor::OnBoxBeginOverlap);
	BoxCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ARA_InteractableActor::OnBoxEndOverlap);
	RootComponent = BoxCollisionComponent;
}