// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RA_InteractableActor.h"
#include "RA_InteractableSaveActor.generated.h"

/**
 * 
 */
UCLASS()
class RACOONADVENTURE_API ARA_InteractableSaveActor : public ARA_InteractableActor
{
	GENERATED_BODY()

	ARA_InteractableSaveActor(const FObjectInitializer& ObjectInitializer);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

	virtual bool OnInteract(AActor* OtherActor) override;
};
