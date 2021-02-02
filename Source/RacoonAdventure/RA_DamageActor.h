// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UObject/UObjectGlobals.h"
#include "RA_DamageActor.generated.h"

UCLASS()
class RACOONADVENTURE_API ARA_DamageActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARA_DamageActor(const FObjectInitializer& ObjectInitializer);

	void InitializeDamage(FVector ImpulseDirection, float DefaultDamage, float DamageRadius, float Lifetime);

	void MigrateFrom(ARA_DamageActor* parent);

	UFUNCTION()
		void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Collision, meta = (AllowPrivateAccess = "true"))
		class USphereComponent* SphereCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Sprite, meta = (AllowPrivateAccess = "true"))
		class UPaperFlipbookComponent* ActorFlipbook;

	FTimerHandle    LifetimeTimer;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	float fSphereRadius = 0.f;

	float fDefaultDamage = 0.f;

	float fLifetimeTime = 1.0f;

	FVector vImpulseDirection;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	bool bIsInitialized = false;
};
