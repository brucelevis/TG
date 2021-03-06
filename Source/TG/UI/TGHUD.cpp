// Fill out your copyright notice in the Description page of Project Settings.


#include "TGHUD.h"
#include "TG/UI/IngameMenu/IngameMenu.h"
#include "TG/UI/IngameInventoryMenu/IngameInventoryMenu.h"
#include "TG/Controllers/ExploreController.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "ExplorerMode/ExplorerModeScreen.h"

void ATGHUD::BeginPlay()
{
	Super::BeginPlay();
	InitializeReferences();
	InitializeWidgets();




}


void ATGHUD::InitializeWidgets()
{

	if (ExplorerModeScreenClass != nullptr)
	{
		refExplorerModeScreen = CreateWidget<UExplorerModeScreen>(GetWorld(), ExplorerModeScreenClass);
		if (refExplorerModeScreen != nullptr)
		{
			refExplorerModeScreen->AddToViewport();
		}
	}

	if (IngameMenuClass != nullptr)
	{
		refIngameMenu = CreateWidget<UIngameMenu>(GetWorld(), IngameMenuClass);
		if (refIngameMenu != nullptr)
		{
			refIngameMenu->AddToViewport();
			IngameMenuClose();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("set the BP widget in TGHUD BP"));
	}


	// 	if (IngameInventoryMenu!= nullptr)
	// 	{
	// 		refIngameInventoryMenu = CreateWidget<UIngameInventoryMenu>(GetWorld(), IngameInventoryMenu);
	// 		if (refIngameInventoryMenu != nullptr)
	// 		{
	// 			refIngameInventoryMenu->AddToViewport();
	// 		}
	// 	}
	// 	else
	// 	{
	// 		UE_LOG(LogTemp, Error, TEXT("set the BP widget in TGHUD BP"));
	// 	}

	InitializeIngameMenuComponents();

}

void ATGHUD::InitializeReferences()
{
	 refExplorePlayerController = Cast<AExploreController>(GetWorld()->GetFirstPlayerController());
}

void ATGHUD::InitializeIngameMenuComponents()
{
	IngameMenuReturnButton = this->refIngameMenu->refReturnGameButton;
	IngameMenuReturnButton->OnClicked.AddDynamic(this, &ATGHUD::IngameMenu_ReturnButtonClicked);
	

	IngameMenuOptionsButton = this->refIngameMenu->refOptionsButton;
	IngameMenuOptionsButton->OnClicked.AddDynamic(this, &ATGHUD::IngameMenu_OptionsButtonClicked);

	IngameMenuQuitToMenuButton = this->refIngameMenu->refQuitGameButton;
	IngameMenuQuitToMenuButton->OnClicked.AddDynamic(this, &ATGHUD::IngameMenu_QuitButtonClicked);

}

void ATGHUD::IngameMenuToggle()
{
	//simple toggle on/off
	if (refIngameMenu != nullptr)
	{	
		refIngameMenu->GetVisibility() == ESlateVisibility::Visible ? this->IngameMenuClose() : this->IngameMenuOpen();
	}
}

void ATGHUD::IngameMenuOpen()
{
	UWorld* World = GetWorld();
	//!! TODO also do a gamestate check
	UE_LOG(LogTemp, Log, TEXT("OpenIngameMenu()"));
	checkSlow(refIngameMenu->GetVisibility() == ESlateVisibility::Hidden); //only want to open from closed
	refIngameMenu->SetVisibility(ESlateVisibility::Visible);
		FInputModeUIOnly InputMode;
		refExplorePlayerController->SetInputMode(InputMode);
		SetIsFocusable(true);
		IngameMenuReturnButton->SetKeyboardFocus();
		UGameplayStatics::SetGamePaused(this, true);

		if (refExplorerModeScreen != nullptr)
		{
			refExplorerModeScreen->SetVisibility(ESlateVisibility::Hidden);
		}
	
}

void ATGHUD::IngameMenuClose()
{
	UWorld* World = GetWorld();
	//!! TODO also do a gamestate check
	UE_LOG(LogTemp, Log, TEXT("CloseIngameMenu()"));
	checkSlow(refIngameMenu->GetVisibility() == ESlateVisibility::Visible); // only want to closed from open
	refIngameMenu->SetVisibility(ESlateVisibility::Hidden);


		FInputModeGameOnly InputMode;
		refExplorePlayerController->SetInputMode(InputMode);
		SetIsFocusable(false);
		UGameplayStatics::SetGamePaused(this, false);
		
		if (refExplorerModeScreen != nullptr)
		{
			refExplorerModeScreen->SetVisibility(ESlateVisibility::Visible);
		}
}

void ATGHUD::IngameMenu_ReturnButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("ATGHUD::IngameMenu_ReturnButtonClicked()"));
	this->IngameMenuToggle();
}

void ATGHUD::IngameMenu_OptionsButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("ATGHUD::IngameMenu_OptionsButtonClicked()"));
	SetIsFocusable(false);
}

void ATGHUD::IngameMenu_QuitButtonClicked()
{
	UE_LOG(LogTemp, Log, TEXT("ATGHUD::IngameMenu_QuitButtonClicked()"));
	FName NextLevelName = TEXT("MapMainMenu");
	UGameplayStatics::OpenLevel(
		this->GetWorld(),
		NextLevelName);

	SetIsFocusable(false);
}

void ATGHUD::SetIsFocusable(bool ibIsFocusable)
{
	refIngameMenu->bIsFocusable = ibIsFocusable;

	if (ibIsFocusable)
	{
		refIngameMenu->SetKeyboardFocus();
	}
}