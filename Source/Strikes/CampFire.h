// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/BoxComponent.h"
#include "CampFire.generated.h"

UCLASS()
class STRIKES_API ACampFire : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ACampFire();

public:
	// Particle system component for visualizing the fire effect
	UPROPERTY(EditAnywhere)
	UParticleSystemComponent* Fire;

	// Box component used for detecting overlaps with other actors
	UPROPERTY(EditAnywhere)
	UBoxComponent* MyBoxComponent;

	// Type of damage applied by the campfire
	UPROPERTY(EditAnywhere)
	TSubclassOf<UDamageType> FireDamageType;

	// Actor currently interacting with the campfire
	UPROPERTY(EditAnywhere)
	AActor* MyCharacter;

	// Information about the hit result from the overlap
	UPROPERTY(EditAnywhere)
	FHitResult MyHit;

	// Flag indicating whether the campfire can apply damage
	bool bCanApplyDamage;

	// Timer handle for managing the periodic application of damage
	FTimerHandle FireTimerHandle;

	// Handles the beginning of an overlap event
	UFUNCTION()
	void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	// Handles the end of an overlap event
	UFUNCTION()
	void OnOverlapEnd(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);


	// Applies fire damage to the actor
	UFUNCTION()
	void ApplyFireDamage();
};
