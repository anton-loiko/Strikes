// Copyright Epic Games, Inc. All Rights Reserved.

#include "StrikesCharacter.h"
#include "StrikesProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/KismetMathLibrary.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AStrikesCharacter

AStrikesCharacter::AStrikesCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));
}

void AStrikesCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Initialize health-related properties.

	// Set the maximum health value.
	FullHealth = 1000.f;

	// Set the current health to the maximum value.
	Health = FullHealth;

	// Set the health percentage to 100%.
	HealthPercentage = 1.f;

	// Allow the character to be damaged.
	bCanBeDamaged = true;

	// Initialize magic-related properties.

	// Set the maximum magic value.
	FullMagic = 100.f;

	// Set the current magic to the maximum value.
	Magic = FullMagic;

	// Set the magic percentage to 100%.
	MagicPercentage = 1.f;

	// Store the current magic percentage as the previous value.
	PreviousMagic = MagicPercentage;

	// Initialize the magic value to 0.
	MagicValue = 0.f;

	// Allow the character to use magic.
	bCanUseMagic = true;


	if (MagicCurve)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("SetMagicValue"));
		TimelineFinishedCallback.BindUFunction(this, FName("SetMagicState"));
		MyTimeline.AddInterpFloat(MagicCurve, TimelineCallback);
		MyTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
	}
}

void AStrikesCharacter::Tick(const float DeltaTime)
{
	Super::Tick(DeltaTime);

	MyTimeline.TickTimeline(DeltaTime);
}


//////////////////////////////////////////////////////////////////////////// Input

void AStrikesCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AStrikesCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AStrikesCharacter::Look);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error,
		       TEXT(
			       "'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."
		       ), *GetNameSafe(this));
	}
}


void AStrikesCharacter::InitializeMagicTimers()
{
	// Stop the magic timeline to halt any ongoing animation or updates.
	MyTimeline.Stop();

	// Clear the existing magic timer to ensure no pending callbacks.
	GetWorldTimerManager().ClearTimer(MagicTimerHandler);

	// Adjust the magic value by a fixed amount (e.g., reduce by 20 units).
	SetMagicChange(-20.f);

	// Set up a new timer to update magic value after a delay of 5 seconds.
	GetWorldTimerManager().SetTimer(
		MagicTimerHandler, this, &AStrikesCharacter::UpdateMagic, 5.f, false
	);
}

void AStrikesCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AStrikesCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

float AStrikesCharacter::GetHealth() const
{
	// Returns the current health percentage of the character.
	return HealthPercentage;
}

float AStrikesCharacter::GetMagic() const
{
	// Returns the current magic percentage of the character.
	return MagicPercentage;
}

FText AStrikesCharacter::GetHealthIntText() const
{
	// Converts the health percentage to a formatted string showing the health as a percentage (e.g., "75%").

	const int32 HP = FMath::RoundHalfFromZero(HealthPercentage * 100);
	const FString HPS = FString::FromInt(HP);
	const FString HealthHUD = HPS + FString(TEXT("%"));
	const FText HPText = FText::FromString(HealthHUD);

	return HPText;
}

FText AStrikesCharacter::GetMagicIntText() const
{
	// Converts the current and full magic values to a formatted string (e.g., "30/100").

	const int32 MP = FMath::RoundHalfFromZero(MagicPercentage * 100);
	const FString MPS = FString::FromInt(MP);
	const FString FullMPS = FString::FromInt(FullMagic);
	const FString MagicHUD = MPS + FString(TEXT("/") + FullMPS);
	const FText MagicText = FText::FromString(MagicHUD);

	return MagicText;
}

void AStrikesCharacter::SetDamageState()
{
	// Sets the character to a state where it can be damaged.
	bCanBeDamaged = true;
}

void AStrikesCharacter::DamageTimer()
{
	// Sets a timer to allow the character to be damaged after a delay.
	GetWorldTimerManager().SetTimer(
		MemberTimerHandler, this, &AStrikesCharacter::SetDamageState, 2.f, false
	);
}

void AStrikesCharacter::SetMagicValue()
{
	// Updates the magic value based on the current timeline position and curve.

	TimeLineValue = MyTimeline.GetPlaybackPosition();
	CurveFloatValue = PreviousMagic + MagicValue * MagicCurve->GetFloatValue(TimeLineValue);
	Magic = CurveFloatValue * FullHealth;
	Magic = FMath::Clamp(Magic, 0.0f, FullMagic);
	MagicPercentage = CurveFloatValue;
	MagicPercentage = FMath::Clamp(MagicPercentage, 0.0f, 1.0f);
}

void AStrikesCharacter::SetMagicState()
{
	// Sets the character to a state where magic can be used and resets the magic value.
	bCanUseMagic = true;
	MagicValue = 0.f;

	// Applies the default material to the weapon mesh if available.
	if (GunDefaultMaterial)
	{
		Mesh1P->SetMaterial(0, GunDefaultMaterial);
	}
}


bool AStrikesCharacter::PlayFlash()
{
	// Toggles the red flash effect on or off and returns whether the flash was active.

	if (!bRedFlash)
	{
		// Early exit if the flash effect is not active
		return false;
	}

	// Deactivate the flash effect
	bRedFlash = false;

	// Return true indicating the flash was active
	return true;
}


void AStrikesCharacter::ReceivePointDamage(float Damage, const UDamageType* DamageType, FVector HitLocation,
                                           FVector HitNormal, UPrimitiveComponent* HitComponent, FName BoneName,
                                           FVector ShotFromDirection,
                                           AController* InstigatedBy, AActor* DamageCauser, const FHitResult& HitInfo)
{
	// Disables the ability to take damage and triggers a red flash effect.
	// Updates health based on the damage received and starts a timer to re-enable damage capability.

	bCanBeDamaged = false;
	bRedFlash = true;
	UpdateHealth(-Damage);
	DamageTimer();
}

void AStrikesCharacter::UpdateHealth(const float HealthChange)
{
	// Modifies the current health by the specified amount and ensures it remains within valid range.
	// Updates health percentage based on the new health value.
	Health += HealthChange;
	Health = FMath::Clamp(Health, 0.0f, FullHealth);
	PreviousHealth = HealthPercentage;
	HealthPercentage = Health / FullHealth;
}


void AStrikesCharacter::UpdateMagic()
{
	// Updates the magic percentage and initiates a timeline to animate the change.
	// Sets the magic value to a default of 1 and starts the timeline.
	PreviousMagic = MagicPercentage;
	MagicPercentage = Magic / FullMagic;
	MagicValue = 1.f;
	MyTimeline.PlayFromStart();
}

void AStrikesCharacter::SetMagicChange(const float MagicChange)
{
	// Disables the ability to use magic and updates the magic value based on the change specified.
	// Updates the material of the weapon to indicate overheat and starts the timeline for magic change.
	bCanUseMagic = false;
	PreviousMagic = MagicPercentage;
	MagicValue = (MagicChange / FullMagic);

	// Sets the material to indicate the weapon is overheated if the material is available.
	if (GunOverheatMaterial)
	{
		Mesh1P->SetMaterial(0, GunOverheatMaterial);
	}

	// Starts the timeline to animate the change in magic value.
	MyTimeline.PlayFromStart();
}
