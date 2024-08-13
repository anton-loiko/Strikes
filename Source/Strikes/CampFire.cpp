// Fill out your copyright notice in the Description page of Project Settings.


#include "CampFire.h"
#include  "Kismet/GameplayStatics.h"
#include  "TimerManager.h"

// Sets default values
ACampFire::ACampFire()
{
	// Create and initialize the box component
	MyBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("My Box Component"));
	MyBoxComponent->InitBoxExtent(FVector(50.f, 50.f, 50.f));

	// Set the box component as the root component
	RootComponent = MyBoxComponent;

	// Create and initialize the fire particle system component
	Fire = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("My Fire"));
	Fire->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	Fire->SetupAttachment(RootComponent);

	// Bind overlap events to corresponding functions
	MyBoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ACampFire::OnOverlapBegin);
	MyBoxComponent->OnComponentEndOverlap.AddDynamic(this, &ACampFire::OnOverlapEnd);

	// Initialize the flag for applying damage
	bCanApplyDamage = false;
}

void ACampFire::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	// Check if the overlapped actor is valid and not the current instance
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		// Enable damage application and store the actor and hit result
		bCanApplyDamage = true;
		MyCharacter = Cast<AActor>(OtherActor);
		MyHit = SweepResult;

		// Set a timer to repeatedly apply damage to the overlapping actor
		// 2.2f - so we don't conflict with the invincibility where 2.f 
		GetWorldTimerManager().SetTimer(
			FireTimerHandle,
			this,
			&ACampFire::ApplyFireDamage,
			2.2f,
			true,
			0.f
		);
	}
}

void ACampFire::OnOverlapEnd(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	// Disable damage application and clear the damage timer
	bCanApplyDamage = false;
	GetWorldTimerManager().ClearTimer(FireTimerHandle);
}

void ACampFire::ApplyFireDamage()
{
	// Apply fire damage to the character if damage can be applied
	if (bCanApplyDamage)
	{
		UGameplayStatics::ApplyPointDamage(
			MyCharacter,
			200.0f,
			GetActorLocation(),
			MyHit,
			nullptr,
			this,
			FireDamageType
		);
	}
}
