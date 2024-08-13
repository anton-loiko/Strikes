// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "StrikesHUD.generated.h"

/**
 * 
 */
UCLASS()
class STRIKES_API AStrikesHUD : public AHUD
{
	GENERATED_BODY()

public:
	AStrikesHUD();
	//
	// /** Primary draw call for the HUD */
	// virtual void DrawHUD() override;

	virtual void BeginPlay() override;

private:
	// TODO: Impelement simple Crosshair
	// /** Crosshair asset pointer */
	// UTexture2D* CrosshairTex;

	// Class of the HUD widget to be used. This should be set in the editor or through code.
	// The widget class must derive from UUserWidget and is used to create instances of the HUD widget.
	UPROPERTY(EditAnywhere, Category="Health")
	TSubclassOf<UUserWidget> HUDWidgetClass;

	// Instance of the HUD widget currently being displayed.
	// This is the actual widget that will be added to the viewport.
	UPROPERTY(EditAnywhere, Category="Health")
	UUserWidget* CurrentWidget;
};
