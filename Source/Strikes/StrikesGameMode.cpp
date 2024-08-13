// Copyright Epic Games, Inc. All Rights Reserved.

#include "StrikesGameMode.h"

#include "AsyncTreeDifferences.h"
#include "StrikesCharacter.h"
#include "StrikesHUD.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AStrikesGameMode::AStrikesGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(
		TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// Set the HUD class to use the custom AStrikesHUD class for the game's user interface.
	HUDClass = AStrikesHUD::StaticClass();
}

void AStrikesGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Set the initial game state to playing
	SetCurrentState(EGamePlayState::EPlaying);

	// Get the player character and cast it to our custom character class
	MyCharacter = Cast<AStrikesCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
}

void AStrikesGameMode::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MyCharacter)
	{
		// Check if the player's health is nearly zero and set the game state to Game Over if true
		if (FMath::IsNearlyZero(MyCharacter->GetHealth(), 0.001f))
		{
			SetCurrentState(EGamePlayState::EGameOver);
		}
	}
}

EGamePlayState AStrikesGameMode::GetCurrentState() const
{
	return CurrentState;
}

void AStrikesGameMode::SetCurrentState(EGamePlayState NewState)
{
	CurrentState = NewState;
	HandleNewState(NewState);
}

void AStrikesGameMode::HandleNewState(const EGamePlayState NewState) const
{
	switch (NewState)
	{
	case EGamePlayState::EPlaying:
		{
			// Do nothing for the playing state
			// TODO: If not changing, move this case down to the default
			break;
		}
	case EGamePlayState::EGameOver:
		{
			// Reload the current level to restart the game
			UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
			break;
		}
	case EGamePlayState::EUnknown:
	default:
		{
			// Do nothing for unknown or default states
			break;
		}
	}
}
