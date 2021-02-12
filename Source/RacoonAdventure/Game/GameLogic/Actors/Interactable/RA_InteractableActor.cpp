// Fill out your copyright notice in the Description page of Project Settings.

#include "RA_InteractableActor.h"
#include "Components/BoxComponent.h"

// Sets default values
ARA_InteractableActor::ARA_InteractableActor()
{
	PrimaryActorTick.bCanEverTick = true;
	eInteractionType = EInteractionType::EDefaultInteract;

	BoxCollisionComponent = CreateDefaultSubobject<UBoxComponent>(FName("CollisionBox"), true);
	BoxCollisionComponent->SetCollisionProfileName(FName("Trigger"));
	BoxCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ARA_InteractableActor::OnBoxBeginOverlap);
	BoxCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ARA_InteractableActor::OnBoxEndOverlap);
	RootComponent = BoxCollisionComponent;
}

// Called when the game starts or when spawned
void ARA_InteractableActor::BeginPlay()
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
}

void ARA_InteractableActor::Tick(float DeltaTime)
{
	fDeltaTime = DeltaTime;
}