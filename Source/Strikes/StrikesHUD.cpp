// Fill out your copyright notice in the Description page of Project Settings.


#include "StrikesHUD.h"

#include "Blueprint/UserWidget.h"

AStrikesHUD::AStrikesHUD()
{
	// Find and set the HUD widget class from the specified path
	static ConstructorHelpers::FClassFinder<UUserWidget> HealthBarObj(TEXT("/Game/FirstPerson/UI/WBP_Health_UI"));
	HUDWidgetClass = HealthBarObj.Class;
}

void AStrikesHUD::BeginPlay()
{
	Super::BeginPlay();

	// Create and display the HUD widget if the class is valid
	if (HUDWidgetClass != nullptr)
	{
		// Create an instance of the HUD widget using the class found in the constructor
		CurrentWidget = CreateWidget<UUserWidget>(GetWorld(), HUDWidgetClass);

		// If the widget was created successfully, add it to the viewport to display it on screen
		if (CurrentWidget)
		{
			CurrentWidget->AddToViewport();
		}
	}
}
