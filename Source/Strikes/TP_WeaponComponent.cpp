// Copyright Epic Games, Inc. All Rights Reserved.


#include "TP_WeaponComponent.h"
#include "StrikesCharacter.h"
#include "StrikesProjectile.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Animation/AnimInstance.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}


void UTP_WeaponComponent::Fire()
{
	if (Character == nullptr || Character->GetController() == nullptr)
	{
		return;
	}

	// Attempt to fire a projectile if:
	// - ProjectileClass is valid 
	// - The character's magic is above a small threshold (not nearly zero)
	// - The character is allowed to use magic
	if (ProjectileClass != nullptr && !FMath::IsNearlyZero(Character->Magic, 0.001f) &&
		Character->bCanUseMagic)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			APlayerController* PlayerController = Cast<APlayerController>(Character->GetController());
			const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = GetOwner()->GetActorLocation() + SpawnRotation.RotateVector(MuzzleOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride =
				ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// Spawn the projectile at the muzzle
			World->SpawnActor<AStrikesProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		}

		// Try and play the sound if specified
		if (FireSound != nullptr)
		{
			UGameplayStatics::PlaySoundAtLocation(this, FireSound, Character->GetActorLocation());
		}

		// Try and play a firing animation if specified
		if (FireAnimation != nullptr)
		{
			// Get the animation object for the arms mesh
			UAnimInstance* AnimInstance = Character->GetMesh1P()->GetAnimInstance();
			if (AnimInstance != nullptr)
			{
				AnimInstance->Montage_Play(FireAnimation, 1.f);
			}
		}

		// Initialize all magic-related timers and reset their states.
		Character->InitializeMagicTimers();
	}
}

bool UTP_WeaponComponent::AttachWeapon(AStrikesCharacter* TargetCharacter)
{
	Character = TargetCharacter;

	if (Character)
	{
		// Subscribe to the overheat event
		Character->OnOverheat.AddDynamic(this, &UTP_WeaponComponent::OnOverheatEvent);
	}


	// Check that the character is valid, and has no weapon component yet
	if (Character == nullptr || Character->GetInstanceComponents().FindItemByClass<UTP_WeaponComponent>())
	{
		return false;
	}

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(Character->GetMesh1P(), AttachmentRules, FName(TEXT("GripPoint")));

	// add the weapon as an instance component to the character
	Character->AddInstanceComponent(this);

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(FireMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(
			PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);
		}
	}

	return true;
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Check if the character is valid before proceeding.
	if (Character == nullptr)
	{
		return;
	}

	// Attempt to retrieve the PlayerController from the character.
	if (APlayerController* PlayerController = Cast<APlayerController>(Character->GetController()))
	{
		// Attempt to retrieve the EnhancedInputLocalPlayerSubsystem from the PlayerController's local player.
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<
			UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Remove the input mapping context to clean up input bindings.
			Subsystem->RemoveMappingContext(FireMappingContext);
		}
	}
}


void UTP_WeaponComponent::SetOverheat(const bool bOverheat)
{
	// Check if the character is valid before proceeding.
	if (!Character)
	{
		return;
	}

	// Retrieve all child components of the character's mesh.
	TArray<USceneComponent*> ChildComponents;
	Character->GetMesh1P()->GetChildrenComponents(true, ChildComponents);

	// Check if the first child component is a skeletal mesh component.
	if (USkeletalMeshComponent* GunComponent = Cast<USkeletalMeshComponent>(ChildComponents[0]))
	{
		// Set the material of the mesh based on whether the weapon is overheating.
		GunComponent->SetMaterial(
			0,
			bOverheat ? Character->GunOverheatMaterial : Character->GunDefaultMaterial
		);
	}
}


void UTP_WeaponComponent::OnOverheatEvent(bool bOverheat)
{
	// Handle the overheat event by updating the weapon's material.
	SetOverheat(bOverheat);
}
