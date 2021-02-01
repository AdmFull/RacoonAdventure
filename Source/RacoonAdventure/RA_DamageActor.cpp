// Fill out your copyright notice in the Description page of Project Settings.


#include "RA_DamageActor.h"
#include "PaperFlipbookComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
ARA_DamageActor::ARA_DamageActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, FName("CollisionSphere"));
	SphereCollision->InitSphereRadius(fSphereRadius);
	//SphereCollision->SetSimulatePhysics(true);
	SphereCollision->SetCollisionProfileName(FName("Trigger"));
	SphereCollision->Mobility = EComponentMobility::Movable;
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ARA_DamageActor::OnBoxBeginOverlap);
	RootComponent = SphereCollision;

	ActorFlipbook = CreateOptionalDefaultSubobject<UPaperFlipbookComponent>(FName("DamageObjectSprite"));
	ActorFlipbook->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARA_DamageActor::BeginPlay()
{
	Super::BeginPlay();
	
	/*UWorld* CurWorld = GEngine->GetWorldFromContextObject(this, EGetWorldErrorMode::LogAndReturnNull);
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(CurWorld, 0);
	SetActorLocation(PlayerPawn->GetActorLocation());*/
	SphereCollision->AddImpulse(FVector(100.f, 0.f, 0.f), NAME_None, true);
}

// Called every frame
void ARA_DamageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DEBUG SHIT
	DrawDebugSphere(GetWorld(), GetActorLocation(), fSphereRadius, 20, FColor::Purple, false, -1, 0, 1);
}

void ARA_DamageActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		UGameplayStatics::ApplyDamage(OtherActor, fDefaultDamage, NULL, NULL, NULL);
		Destroy();
	}
}

