// Fill out your copyright notice in the Description page of Project Settings.


#include "MedKit.h"

// Sets default values
AMedKit::AMedKit()
{
	// Bind the overlap event to the OnOverlap function
	OnActorBeginOverlap.AddDynamic(this, &AMedKit::OnOverlap);
}

void AMedKit::OnOverlap(AActor* MyOverlappedActor, AActor* OtherActor)
{
	// Check if the overlapped actor is valid and not the current instance
	if ((OtherActor != nullptr) && (OtherActor != this))
	{
		// Check if the overlapped actor is valid and not the current instance
		MyCharacter = Cast<AStrikesCharacter>(OtherActor);

		// If the character is valid and health is less than 1, heal the character
		if (MyCharacter && MyCharacter->GetHealth() < 1.f)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				2.f,
				FColor::Green,
				FString::Printf(TEXT("Add Health"))
			);

			// Increase the character's health and destroy the medkit
			MyCharacter->UpdateHealth(100.f);
			Destroy();
		}
	}
}
