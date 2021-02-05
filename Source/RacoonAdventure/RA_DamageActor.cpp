// Fill out your copyright notice in the Description page of Project Settings.


#include "RA_DamageActor.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
ARA_DamageActor::ARA_DamageActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleCollision = ObjectInitializer.CreateDefaultSubobject<UCapsuleComponent>(this, FName("CollisionCapsule"));
	//CapsuleCollision->SetCollisionProfileName(FName("Trigger"));
	CapsuleCollision->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	CapsuleCollision->Mobility = EComponentMobility::Movable;
	CapsuleCollision->OnComponentBeginOverlap.AddDynamic(this, &ARA_DamageActor::OnBoxBeginOverlap);
	CapsuleCollision->AddWorldRotation(FRotator(90.f, 90.f, 90.f));
	RootComponent = CapsuleCollision;

	ActorFlipbook = ObjectInitializer.CreateOptionalDefaultSubobject<UPaperFlipbookComponent>(this, TEXT("DefaultAttackAnimation"));
	ActorFlipbook->SetupAttachment(RootComponent);
}

void ARA_DamageActor::InitializeDamage(FVector ImpulseDirection, float DefaultDamage, float DamageRadius, float Lifetime)
{
	vImpulseDirection = ImpulseDirection;
	fDefaultDamage = DefaultDamage;
	fSphereRadius = DamageRadius;
	fLifetimeTime = Lifetime;
	CapsuleCollision->InitCapsuleSize(DamageRadius, DamageRadius * 2);

	CapsuleCollision->SetSimulatePhysics(true);
	CapsuleCollision->AddImpulse(ImpulseDirection * 50, NAME_None, true);
	
	GetWorld()->GetTimerManager().SetTimer(LifetimeTimer, [this]()
	{
		Destroy();
	}, fLifetimeTime, 1);
}

void ARA_DamageActor::MigrateFrom(ARA_DamageActor* parent)
{
	this->CapsuleCollision = parent->CapsuleCollision;
	RootComponent = CapsuleCollision;
	this->ActorFlipbook = parent->ActorFlipbook;
	//ActorFlipbook->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ARA_DamageActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ARA_DamageActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//DEBUG SHIT
	DrawDebugCapsule(GetWorld(), GetActorLocation(), CapsuleCollision->GetScaledCapsuleHalfHeight(),
		CapsuleCollision->GetScaledCapsuleRadius(), CapsuleCollision->GetComponentQuat(), FColor::Red);
}

void ARA_DamageActor::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{

		//Peredelat vse nahoi
		if (!OtherActor->ActorHasTag("player") && !GetOwner()->ActorHasTag("player"))
		{
			UGameplayStatics::ApplyDamage(OtherActor, fDefaultDamage, NULL, NULL, NULL);
			Destroy();
		}
	}
}

