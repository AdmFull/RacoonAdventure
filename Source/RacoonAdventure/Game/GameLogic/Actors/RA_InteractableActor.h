// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RA_InteractableActor.generated.h"

//TODO: Remake ledder moving system using this kind of actor
//TODO: -//- with wall jumping
//TODO: -//- with flor platforms
//TODO: Create chests with loot
//TODO: Create interact with NPC
//TODO: Create Factory method for this class

UENUM()
enum class EInteractionType : uint8
{
	EDefaultInteract,
	ESaveGameInteract,
	EChestInteract,
	ENPCInteract,
	EDamageInteract
};

UCLASS()
class RACOONADVENTURE_API ARA_InteractableActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARA_InteractableActor();

protected:

	EInteractionType eInteractionType;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
		class UBoxComponent* BoxCollisionComponent;

	UFUNCTION()
		virtual void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) PURE_VIRTUAL(ARA_InteractableActor::OnBoxBeginOverlap, );

	UFUNCTION()
		virtual void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) PURE_VIRTUAL(ARA_InteractableActor::OnBoxEndOverlap, );

	UFUNCTION()
		virtual void OnInteract(AActor* OtherActor) PURE_VIRTUAL(ARA_InteractableActor::OnInteract, );

};
