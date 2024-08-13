// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "Components/TimelineComponent.h"
#include "StrikesCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOverheat, bool, bOverheat);

UCLASS(config=Game)
class AStrikesCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

public:
	AStrikesCharacter();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaTime) override;

public:
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;

	// Health Properties

	/**
	 * Total amount of health the character can have.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float FullHealth;

	/**
	 * Current health of the character.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float Health;

	/**
	 * Percentage of current health compared to the full health.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	float HealthPercentage;

	/**
	 * Previous value of health before the last update.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float PreviousHealth;

	/**
	 * Indicates if the screen should flash red (e.g., when taking damage).
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Health")
	bool bRedFlash;

	// Magic Properties

	/**
	 * Total amount of magic the character can have.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	float FullMagic;

	/**
	 * Current amount of magic the character has.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	float Magic;

	/**
	 * Percentage of current magic compared to the full magic.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	float MagicPercentage;

	/**
	 * Previous value of magic before the last update.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	float PreviousMagic;

	/**
	 * Amount of magic to be used or changed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	float MagicValue;

	/**
	 * Curve used to modify magic values over time.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Magic")
	UCurveFloat* MagicCurve;


	/**
	 * Timeline for managing magic-related events or animations.
	 */
	UPROPERTY(EditAnywhere, Category="Magic")
	FTimeline MyTimeline;

	/**
	 * Timer handle for managing time-based events related to magic.
	 */
	UPROPERTY(EditAnywhere, Category="Magic")
	FTimerHandle MemberTimerHandler;

	/**
	 * Timer handle for managing magic-specific time-based events.
	 */
	UPROPERTY(EditAnywhere, Category="Magic")
	FTimerHandle MagicTimerHandler;

	// Internal Properties

	// Value of the curve float for current magic status.
	float CurveFloatValue;

	// Current value of the timeline for magic events.	
	float TimeLineValue;

	// Indicates if the character can be damaged.
	bool bCanBeDamaged;

	// Indicates if the character can use magic.
	bool bCanUseMagic;


	// Health Functions

	/**
	 * Gets the current health of the character.
	 * 
	 * @return Current health value.
	 */
	UFUNCTION(BlueprintPure, Category="Health")
	float GetHealth() const;

	/**
	 * Gets the current health as a formatted text.
	 * 
	 * @return Formatted health text.
	 */
	UFUNCTION(BlueprintPure, Category="Health")
	FText GetHealthIntText() const;

	// Magic Functions
	/**
	 * Gets the current amount of magic the character has.
	 * 
	 * @return Current magic value.
	 */
	UFUNCTION(BlueprintPure, Category="Magic")
	float GetMagic() const;

	/**
	 * Gets the current magic amount as a formatted text.
	 * 
	 * @return Formatted magic text.
	 */
	UFUNCTION(BlueprintPure, Category="Magic")
	FText GetMagicIntText() const;

	/**
	 * Function called on a timer to handle damage-related events.
	 */
	UFUNCTION()
	void DamageTimer();

	/**
	 * Sets the state of the character in response to damage.
	 */
	UFUNCTION()
	void SetDamageState();

	/**
	 * Sets the amount of magic the character can use.
	 */
	UFUNCTION()
	void SetMagicValue();

	/**
	 * Sets the state of the magic, including usage or changes.
	 */
	UFUNCTION()
	void SetMagicState();

	/**
	 * Changes the magic value based on the given input.
	 * 
	 * @param MagicChange The new value to set for magic.
	 */
	UFUNCTION()
	void SetMagicChange(float MagicChange);

	/**
	 * Updates the magic value based on current conditions.
	 */
	UFUNCTION()
	void UpdateMagic();

	/**
	 * Determines if a red flash effect should be played.
	 * 
	 * @return True if the flash should be played, otherwise false.
	 */
	UFUNCTION(BlueprintPure, Category="Health")
	bool PlayFlash();

	/**
	 * Default material for the weapon.
	 */
	UPROPERTY(EditAnywhere, Category="Magic")
	UMaterialInterface* GunDefaultMaterial;

	/**
	 * Material applied to the weapon when it overheats.
	 */
	UPROPERTY(EditAnywhere, Category="Magic")
	UMaterialInterface* GunOverheatMaterial;

	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCauser
	) override;

	/**
	 * Updates the health of the character based on the given change.
	 * 
	 * @param HealthChange The amount by which to change the health.
	 */
	UFUNCTION(BlueprintCallable, Category="Health")
	void UpdateHealth(float HealthChange);

	/**
	 * Resets and initializes the magic-related timers and updates.
	 * Stops any ongoing magic timeline, clears and sets up a new magic timer, and updates the magic value.
	 */
	void InitializeMagicTimers();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

	// Delegate for overheat events
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnOverheat OnOverheat;

	// Function to trigger overheat event
	void TriggerOverheat(bool bOverheat);
};
