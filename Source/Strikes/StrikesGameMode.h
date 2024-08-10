// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "StrikesCharacter.h"
#include "StrikesGameMode.generated.h"


UENUM()
enum class EGamePlayState
{
	/** Indicates that the game is currently active and in progress. */
	EPlaying,

	/** Indicates that the game has ended, either through winning or losing. */
	EGameOver,

	/** Indicates an undefined or unrecognized game state. */
	EUnknown
};

/**
 * Game mode class for the Strikes game.
 * This class manages the state of the game and interacts with the player character.
 */
UCLASS(minimalapi)
class AStrikesGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/**
	 * Default constructor for AStrikesGameMode.
	 * Initializes default properties for the game mode.
	 */
	AStrikesGameMode();

	/**
	 * Called when the game starts or when spawned.
	 * Sets the initial game state and retrieves the player character.
	 */
	virtual void BeginPlay() override;

	/**
	 * Called every frame.
	 * Checks the health of the player character and updates the game state if necessary.
	 * 
	 * @param DeltaTime The time elapsed since the last frame.
	 */
	virtual void Tick(float DeltaTime) override;

	/** Pointer to the player character. */
	AStrikesCharacter* MyCharacter;

	/**
	 * Gets the current state of the game.
	 * 
	 * @return The current game play state.
	 */
	UFUNCTION(BlueprintPure, Category="Health")
	EGamePlayState GetCurrentState() const;

	/**
	 * Sets a new game state and handles any necessary updates.
	 * 
	 * @param NewState The new game play state to set.
	 */
	void SetCurrentState(EGamePlayState NewState);

private:
	/** The current state of the game. */
	EGamePlayState CurrentState;

	/**
	 * Handles updates and actions based on the new game state.
	 * 
	 * @param NewState The new game play state to handle.
	 */
	void HandleNewState(EGamePlayState NewState) const;
};
