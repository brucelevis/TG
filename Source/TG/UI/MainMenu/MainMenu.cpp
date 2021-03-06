// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "MainMenuSlot.h"
#include "TG/Controllers/MainMenuPlayerController.h"
#include "Components/SizeBox.h"
#include "Components/UniformGridPanel.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "TG/Libraries/InventoryLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TG/StaticLibrary.h"
#include "TG/UI/MainMenu/MenuItem.h"
#include "Kismet/GameplayStatics.h"
#include "PaperSprite.h"
#include "Components/Image.h"

#define MAINMENU_ICONSIZE FVector2D(90, 90)

void UMainMenu::NativeConstruct()
{
	Super::NativeConstruct();

	InitializeBPReferences();
	hiddenSlotIndexes = UInventoryLibrary::GetMenuInventoryGridData(EMenuType::MAINMENU).hiddenIndexes;
	CreateMenuSlots();
	AddDelegateBindings();
	GetStartingSlot();
	CreateMenuStartingItems();
	UpdateTooltipText();
	
}

void UMainMenu::NativePreConstruct()
{
	Super::NativePreConstruct();	 
}

void UMainMenu::CreateMenuStartingItems()
{
	/*This is pretty bad hardcoded way to make menu, but it's simple. I don't really want to spend a day think of how
	to make it do it with datatable, I think for stuff like menus that wont get changed almost at all its fine*/

	//spawnItem(World, idx) idx - row-1 in DT_MenuItemData, its confusing but its just number tracking

 	//toolbar buttons
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 0), FVector2D(0, 4)); //play
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 3), FVector2D(1, 4)); //??
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 3), FVector2D(2, 4)); //??
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 1), FVector2D(3, 4)); //options
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 2), FVector2D(4, 4)); //exit

	//toolbar play
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 4), FVector2D(0, 3)); //select map1
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 5), FVector2D(0, 2)); //select map2
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 6), FVector2D(0, 1)); //select map3
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 7), FVector2D(0, 0)); //select map4

	//toolbar toolbar options
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 8), FVector2D(3, 3)); // setting1
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 9), FVector2D(3, 2)); // setting2
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 10), FVector2D(3, 1)); //setting3
	AddItemToInventoryCoords(AMenuItem::SpawnItem(this->GetWorld(), 11), FVector2D(3, 0)); //setting4

}

bool UMainMenu::AddItemToInventory(class AMenuItem* iItem)
{
	if (iItem == nullptr)
	{
		return false;
	}

	if (this->GetFirstEmptyInventorySlotIndex() == -1)
	{
		UE_LOG(LogTemp, Error, TEXT("MenuInventory is full"));
		return false;
	}
	int32 idx = this->GetFirstEmptyInventorySlotIndex();
	mainMenuSlotsInventory[idx]->menuSlotData.refMenuItem = iItem;
	mainMenuSlotsInventory[idx]->menuSlotData.menuInventorySlotState = EInventorySlotState::HASITEM;
	UE_LOG(LogTemp, Error, TEXT("Added menu item successfully."));
	//if i dont do this with timer and above 0.1 seconds timer icons are for some reason white
	FTimerHandle addItemRefreshSlots;
	GetWorld()->GetTimerManager().SetTimer(addItemRefreshSlots, this, &UMainMenu::RefreshMenuSlots, 0.15);
	return true;
}

bool UMainMenu::AddItemToInventoryCoords(class AMenuItem* iItem, FVector2D iCoords)
{
	if (iItem == nullptr)
	{
		return false;
	}

	if (mapRefMenuSlots.Find(iCoords) == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("invalid slot coordinates"));
		return false;
	}
	UMainMenuSlot* l_menuSlot = *mapRefMenuSlots.Find(iCoords);
	int32 idx = l_menuSlot->menuSlotData.slotIndex;
	mainMenuSlotsInventory[idx]->menuSlotData.refMenuItem = iItem;
	mainMenuSlotsInventory[idx]->menuSlotData.menuInventorySlotState = EInventorySlotState::HASITEM;
	UE_LOG(LogTemp, Error, TEXT("Added menu item with coords successfully."));
	//if i dont do this with timer and above 0.1 seconds timer icons are for some reason white
	FTimerHandle addItemRefreshSlots;
	GetWorld()->GetTimerManager().SetTimer(addItemRefreshSlots, this, &UMainMenu::RefreshMenuSlots, 0.15);
	return true;

}

int32 UMainMenu::GetCurrentlySelectedSlotIndex()
{
// 	UMainMenuSlot* l_menuSlot = *mapRefMenuSlots.Find(currentlySelectedSlotCoord);
// 	return l_menuSlot->menuSlotData.slotIndex;
	return 0;
}


void UMainMenu::RefreshMenuSlots()
{
	UE_LOG(LogTemp, Error, TEXT("UMainMenu::RefreshMenuSlots()"));
	CreateMenuSlots();
	UpdateItemsFromMenuInventory(mainMenuSlotsInventory);
}

void UMainMenu::UpdateItemsFromMenuInventory(TArray<class UMainMenuSlot*> iMenuInventory)
{
	TArray<UMainMenuSlot*> mapValues;
	mapRefMenuSlots.GenerateValueArray(mapValues);
	int32 loopCounter = 0;
	for (auto& e : iMenuInventory)
	{
		if (e->menuSlotData.menuInventorySlotState == EInventorySlotState::HASITEM)
		{
			FSlateBrush brush;
			brush.SetResourceObject(e->menuSlotData.refMenuItem->currentMenuItemData.itemIcon);
			//update icon
			mapValues[loopCounter]->refSlotItemIcon->SetBrush(brush);
			mapValues[loopCounter]->refSlotItemIcon->SetBrushSize(MAINMENU_ICONSIZE);

			//add item pointer
			mapValues[loopCounter]->menuSlotData.refMenuItem = e->menuSlotData.refMenuItem;
		}
		loopCounter++;
	}
}


void UMainMenu::DebugAction1()
{
	//using this just to debug whatever crap i need currently
	
	/*AddItemToInventory(AMenuItem::SpawnItem(this->GetWorld(), 0));*/
	
}

void UMainMenu::DebugAction2()
{
	UMainMenuSlot* l_menuSlot = *mapRefMenuSlots.Find(currentlySelectedSlotCoord);
	if (l_menuSlot->menuSlotData.menuInventorySlotState == EInventorySlotState::HASITEM)
	{
		SetIndexesHidden();
		bHasOpenedToolbar = false;
	}
	
}

void UMainMenu::AddDelegateBindings()
{
	refMainMenuPlayerController = Cast<AMainMenuPlayerController>(GetWorld()->GetFirstPlayerController());
	refMainMenuPlayerController->delegateMenuMove.AddDynamic(this, &UMainMenu::MoveInMenu);
	refMainMenuPlayerController->delegateMenuUsePRESSED.AddDynamic(this, &UMainMenu::UseSelectedSlotPressed);
	refMainMenuPlayerController->delegateMenuUseRELEASED.AddDynamic(this, &UMainMenu::UseSelectedSlot);
	refMainMenuPlayerController->delegateDebugAction1.AddDynamic(this, &UMainMenu::DebugAction1);
	refMainMenuPlayerController->delegateDebugAction2.AddDynamic(this, &UMainMenu::DebugAction2);
}

void UMainMenu::InitializeRefs()
{

}



void UMainMenu::CreateMenuSlots()
{
	/*if i miracelously get this to working then add stuff to the uinventorylibrary, sizes and all kinds of stats for the different
	menu types*/
	if (refMenuUniformGridPanel == nullptr)
	{
		return;
	}

	TArray<UMainMenuSlot*> slotOld = mainMenuSlotsInventory;
	int32 l_column = UInventoryLibrary::GetMenuInventoryGridData(EMenuType::MAINMENU).columns;
	int32 l_rows = UInventoryLibrary::GetMenuInventoryGridData(EMenuType::MAINMENU).rows;
	int32 l_nrOfSlots = (l_rows * l_column) - 1;
	mapRefMenuSlots.Empty();
	mainMenuSlotsInventory.Empty();
	refMenuUniformGridPanel->ClearChildren();
	menuInvSize = UInventoryLibrary::GetMenuInventorySlotSize(EMenuType::MAINMENU);
	for (int i = 0; i <= l_nrOfSlots; i++)
	{
		/*dnt forget to update the class in wbp*/
		UMainMenuSlot* wMenuInvSlot = CreateWidget<UMainMenuSlot>(GetWorld(), menuSlotClass);
		int32 l_currColumn = i / -l_column;
		int32 l_currRow = i - ((i / l_column) * l_column);
		refMenuUniformGridPanel->AddChildToUniformGrid(wMenuInvSlot, l_currColumn, l_currRow);
		l_currColumn = FMath::Abs(l_currColumn);
		mapRefMenuSlots.Add(FVector2D(l_currRow, l_currColumn), wMenuInvSlot);
		wMenuInvSlot->menuSlotData.slotCoords = FVector2D(l_currRow, l_currColumn);
		wMenuInvSlot->menuSlotData.slotIndex = i;
		wMenuInvSlot->menuSlotData.menuInventorySlotState = EInventorySlotState::EMPTY;
		wMenuInvSlot->menuSlotData.menuInventorySlotVisibility = EInventoryVisibilityState::VISIBLE;
		wMenuInvSlot->menuSlotData.menuType = EMenuType::MAINMENU;
		wMenuInvSlot->refWSizeBoxSlotSize->SetHeightOverride(menuInvSize);
		wMenuInvSlot->refWSizeBoxSlotSize->SetWidthOverride(menuInvSize);

		if (slotOld.IsValidIndex(0)) //just checks if the thing is empty or not
		{
			switch (slotOld[i]->menuSlotData.menuInventorySlotState)
			{
			case EInventorySlotState::EMPTY:
				wMenuInvSlot->menuSlotData.menuInventorySlotState = EInventorySlotState::EMPTY;
				wMenuInvSlot->menuSlotData.refMenuItem = nullptr;
				break;
			case EInventorySlotState::HASITEM:
				wMenuInvSlot->menuSlotData.menuInventorySlotState = EInventorySlotState::HASITEM;
				wMenuInvSlot->menuSlotData.refMenuItem = slotOld[i]->menuSlotData.refMenuItem;
				break;
			case EInventorySlotState::DISABLED:
				wMenuInvSlot->menuSlotData.menuInventorySlotState = EInventorySlotState::DISABLED;
				break;
			}
		}

		for (auto& e : hiddenSlotIndexes)
		{
			if (i == e)
			{
				wMenuInvSlot->menuSlotData.menuInventorySlotVisibility = EInventoryVisibilityState::HIDDEN;
				//set opacity of the slot to 0
				SetSlotVisibility(wMenuInvSlot, EInventoryVisibilityState::HIDDEN);
			}
		}

		

		mainMenuSlotsInventory.Add(wMenuInvSlot);

	}
	

	HighlightSelectedSlot();
}



void UMainMenu::UseSelectedSlotPressed()
{
	if (!mapRefMenuSlots.Contains(currentlySelectedSlotCoord))
	{
		return;
	}
	UMainMenuSlot* l_currMenuSlot = *mapRefMenuSlots.Find(currentlySelectedSlotCoord);
	if (l_currMenuSlot->menuSlotData.refMenuItem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("no item in slot"));
		return;
	}
	//sets the icon to pressed
	FSlateBrush brush;
	brush.SetResourceObject(l_currMenuSlot->menuSlotData.refMenuItem->currentMenuItemData.itemIconPressed);
	//update icon
	l_currMenuSlot->refSlotItemIcon->SetBrush(brush);
	l_currMenuSlot->refSlotItemIcon->SetBrushSize(MAINMENU_ICONSIZE);

	//Play sfx
	UGameplayStatics::PlaySound2D(GetWorld(), l_currMenuSlot->menuSlotData.refMenuItem->currentMenuItemData.sfxUseItem);
}

void UMainMenu::UseSelectedSlot()
{
	if (!mapRefMenuSlots.Contains(currentlySelectedSlotCoord))
	{
		return;
	}
	UMainMenuSlot* l_currMenuSlot = *mapRefMenuSlots.Find(currentlySelectedSlotCoord);
	if (l_currMenuSlot->menuSlotData.refMenuItem == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("no item in slot"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("\n SLOTDATA \n slotIndex: %i \n itemName: %s \n menuType: %s \n menuInventorySlotState: %s"),
		l_currMenuSlot->menuSlotData.slotIndex,
		*l_currMenuSlot->menuSlotData.refMenuItem->currentMenuItemData.itemName,
		*UStaticLibrary::GetEnumValueAsString("EMenuType", l_currMenuSlot->menuSlotData.menuType ),
		*UStaticLibrary::GetEnumValueAsString("EInventorySlotState", l_currMenuSlot->menuSlotData.menuInventorySlotState)
	);

	//sets the icon to normal from pressed
	FSlateBrush brush;
	brush.SetResourceObject(l_currMenuSlot->menuSlotData.refMenuItem->currentMenuItemData.itemIcon);
	//update icon
	l_currMenuSlot->refSlotItemIcon->SetBrush(brush);
	l_currMenuSlot->refSlotItemIcon->SetBrushSize(MAINMENU_ICONSIZE);

	switch (l_currMenuSlot->menuSlotData.refMenuItem->currentMenuItemData.itemType)
	{
	case EMenuItemButtonType::NONE:
		break;
	case EMenuItemButtonType::STARTGAME:
		//open quest select

		UGameplayStatics::OpenLevel(
			this->GetWorld(),
			TEXT("MapDemoLevel"));

		break;
	case EMenuItemButtonType::OPENSETTINGS:
		//open settings bars
		break;
	case EMenuItemButtonType::QUITGAME:

		UKismetSystemLibrary::QuitGame(GetWorld(), this->GetOwningPlayer(), EQuitPreference::Quit, false);

		break;
	case EMenuItemButtonType::EXPANDING:
		//menu button that opens a toolbar of other selection

		if (l_currMenuSlot->menuSlotData.menuInventorySlotState == EInventorySlotState::HASITEM)
		{

			SetIndexesVisible();
			bHasOpenedToolbar = true;
		}
		break;
	case EMenuItemButtonType::SELECTMAP1:
		UGameplayStatics::OpenLevel(
			this->GetWorld(),
			TEXT("MapDemoLevel"));
		break;
	case EMenuItemButtonType::SELECTMAP2:
		break;
	case EMenuItemButtonType::SELECTMAP3:
		break;
	case EMenuItemButtonType::SELECTMAP4:
		break;
	case EMenuItemButtonType::I:
		break;
	case EMenuItemButtonType::J:
		break;
	case EMenuItemButtonType::K:
		break;
	case EMenuItemButtonType::L:
		break;
	case EMenuItemButtonType::M:
		break;
	default:
		break;

	}




}

void UMainMenu::HighlightSelectedSlot()
{
	UMainMenuSlot* l_ref = *mapRefMenuSlots.Find(currentlySelectedSlotCoord);
	l_ref->menuSlotData.bIsSelected = true;
	l_ref->UpdateInventoryButtonBackgroundType();
}

void UMainMenu::DeHighlightSelectedSlot()
{
	UMainMenuSlot* l_ref = *mapRefMenuSlots.Find(lastSelectedSlotCoord);
	l_ref->menuSlotData.bIsSelected = false;
	l_ref->UpdateInventoryButtonBackgroundType();
}

void UMainMenu::SelectNeightbourSlot(FVector2D iTarget)
{
	//sets the icon back to normal from pressed
	UMainMenuSlot* l_currMenuSlot = *mapRefMenuSlots.Find(currentlySelectedSlotCoord);
	//sets the icon to pressed
	if (l_currMenuSlot->menuSlotData.refMenuItem != nullptr)
	{
	FSlateBrush brush;
	brush.SetResourceObject(l_currMenuSlot->menuSlotData.refMenuItem->currentMenuItemData.itemIcon);
	//update icon
	l_currMenuSlot->refSlotItemIcon->SetBrush(brush);
	l_currMenuSlot->refSlotItemIcon->SetBrushSize(MAINMENU_ICONSIZE);

	//Play sfx
	UGameplayStatics::PlaySound2D(GetWorld(), l_currMenuSlot->menuSlotData.refMenuItem->currentMenuItemData.sfxHoverItem);
	}

	lastSelectedSlotCoord = currentlySelectedSlotCoord;
	currentlySelectedSlotCoord = iTarget;
	DeHighlightSelectedSlot();
	HighlightSelectedSlot();
	UpdateTooltipText();

	

}

void UMainMenu::GetStartingSlot()
{
	for (auto& e : mainMenuSlotsInventory)
	{
		if (e->menuSlotData.menuInventorySlotVisibility != EInventoryVisibilityState::HIDDEN &&
			e->menuSlotData.menuInventorySlotState != EInventorySlotState::DISABLED)
		{
			
			currentlySelectedSlotCoord = e->menuSlotData.slotCoords;
			HighlightSelectedSlot();
			break;
		}
	}

}



void UMainMenu::GetCurrentSelectedRowIndexes(FVector2D iCurrentlySelectedSlot)
{
	//lets find the selected slot data
	UMainMenuSlot* l_currMenuInventorySlot = *mapRefMenuSlots.Find(currentlySelectedSlotCoord);
	if (l_currMenuInventorySlot == nullptr)
	{
		refMenuTooltipText->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget: l_currInventorySlot == nullptr"));
		return;
	}

	//so, for finding row, we just need to +1 or -1 X. For COlumn 
	FVector2D l_currCoord = currentlySelectedSlotCoord;
	//add +1 to X.
	//if the coordinate is valid, add the valid slots index to array
	//if coordinate isn't valid, break loop and do the same loop for -1 to x.

	int yCoord = l_currCoord.Y;
	for (int xCoord = l_currCoord.X; mapRefMenuSlots.Find(FVector2D(xCoord, yCoord)) != nullptr; xCoord++)
	{
		auto& l_slots = *mapRefMenuSlots.Find(FVector2D(xCoord, yCoord));
		UE_LOG(LogTemp, Warning, TEXT("GetCurrentlySelectedRowIndexes();  FVector(%s), index %i"),
			*l_slots->menuSlotData.slotCoords.ToString(),
			l_slots->menuSlotData.slotIndex);


	}
}

TArray<FVector2D> UMainMenu::GetCurrentSelectedColumnCoords(FVector2D iCurrentlySelectedSlot)
{
	TArray<FVector2D> l_coords;
	//lets find the selected slot data
	UMainMenuSlot* l_currMenuInventorySlot = *mapRefMenuSlots.Find(currentlySelectedSlotCoord);
	if (l_currMenuInventorySlot == nullptr)
	{
		refMenuTooltipText->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget: l_currInventorySlot == nullptr"));
		return l_coords;
	}

	//so, for finding row, we just need to +1 or -1 X. For COlumn 
	FVector2D l_currCoord = currentlySelectedSlotCoord;
	//add +1 to X.
	//if the coordinate is valid, add the valid slots index to array
	//if coordinate isn't valid, break loop and do the same loop for -1 to x.
	

	//searches all valid slots from one side
	int xCoord = l_currCoord.X;
	for (int yCoord = l_currCoord.Y; mapRefMenuSlots.Find(FVector2D(xCoord, yCoord)) != nullptr; yCoord++)
	{
		auto& l_slots = *mapRefMenuSlots.Find(FVector2D(xCoord, yCoord));
		UE_LOG(LogTemp, Warning, TEXT("GetCurrentlySelectedColumnIndexes();  FVector(%s), index %i"),
			*l_slots->menuSlotData.slotCoords.ToString(),
			l_slots->menuSlotData.slotIndex);

		l_coords.Add(l_slots->menuSlotData.slotCoords);
	}
	//searches all valid slots from other side
	for (int yCoord = l_currCoord.Y; mapRefMenuSlots.Find(FVector2D(xCoord, yCoord)) != nullptr; yCoord--)
	{
		auto& l_slots = *mapRefMenuSlots.Find(FVector2D(xCoord, yCoord));
		UE_LOG(LogTemp, Warning, TEXT("GetCurrentlySelectedColumnIndexes();  FVector(%s), index %i"),
			*l_slots->menuSlotData.slotCoords.ToString(),
			l_slots->menuSlotData.slotIndex);

		l_coords.Add(l_slots->menuSlotData.slotCoords);
	}
	return l_coords;
	
}

void UMainMenu::UpdateTooltipText()
{
	//lets find the selected slot data
	UMainMenuSlot* l_currMenuInventorySlot = *mapRefMenuSlots.Find(currentlySelectedSlotCoord);
	if (l_currMenuInventorySlot == nullptr)
	{
		refMenuTooltipText->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget: l_currInventorySlot == nullptr"));
		return;
	}

	if (l_currMenuInventorySlot->menuSlotData.menuInventorySlotState == EInventorySlotState::EMPTY)
	{
		refMenuTooltipText->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget: l_currInventorySlot->slotData.refItem == nullptr"));
		return;
	}

	FMenuItemData l_selectedMenuItemData = l_currMenuInventorySlot->menuSlotData.refMenuItem->currentMenuItemData;
	
	//update item name
	refMenuTooltipText->SetText(FText::FromString(l_selectedMenuItemData.itemName));
	refMenuTooltipText->SetVisibility(ESlateVisibility::Visible);
}

void UMainMenu::SetIndexesVisible()
{


	if (!bHasSetIndexesVisible)
	{
		oldHiddenSlotIndexes = hiddenSlotIndexes;
	}
	//go through the indexes and find these inside the map and set the map slot visible
	for (auto& e : GetCurrentSelectedColumnCoords(currentlySelectedSlotCoord))
	{
		if (e != currentlySelectedSlotCoord)
		{
			UMainMenuSlot* l_menuSlot = *mapRefMenuSlots.Find(e);
			SetSlotVisibility(l_menuSlot, EInventoryVisibilityState::VISIBLE);
			hiddenSlotIndexes.Remove(l_menuSlot->menuSlotData.slotIndex);
		}
	}

	RefreshMenuSlots();
	bHasSetIndexesVisible = true;
}

void UMainMenu::SetIndexesHidden()
{
	hiddenSlotIndexes = oldHiddenSlotIndexes;
	//go through the indexes and find these inside the map and set the map slot visible
	for (auto& e : GetCurrentSelectedColumnCoords(currentlySelectedSlotCoord))
	{
		if (e != currentlySelectedSlotCoord)
		{
			UMainMenuSlot* l_menuSlot = *mapRefMenuSlots.Find(e);
			l_menuSlot->menuSlotData.menuInventorySlotVisibility = EInventoryVisibilityState::HIDDEN;
			SetSlotVisibility(l_menuSlot, EInventoryVisibilityState::HIDDEN);
			
		}
	}
	RefreshMenuSlots();
	bHasSetIndexesVisible = false;
}

void UMainMenu::SetSlotVisibility(class UMainMenuSlot* iSlot, EInventoryVisibilityState iSlotVisibility)
{
	if (iSlotVisibility == EInventoryVisibilityState::HIDDEN)
	{
		iSlot->SetRenderOpacity(0);
	}
	else
	{
		iSlot->SetRenderOpacity(1);
	}
}

int32 UMainMenu::GetFirstEmptyInventorySlotIndex()
{
	TArray<UMainMenuSlot*> mapValues;
	mapRefMenuSlots.GenerateValueArray(mapValues);
	for (auto& e : mapValues)
	{
		//will add item if slot is empty
		if (e->menuSlotData.menuInventorySlotState == EInventorySlotState::EMPTY)
		{
			return e->menuSlotData.slotIndex;
		}
	}

	//inv full
	return -1;

}

void UMainMenu::MoveInMenu(EMoveDirections iMoveDir)
{
	FVector2D l_targetDirection = currentlySelectedSlotCoord;
	switch (iMoveDir)
	{
	case EMoveDirections::UP:
		UE_LOG(LogTemp, Warning, TEXT("UP"));
		l_targetDirection.Y += 1;
		break;
	case EMoveDirections::DOWN:
		l_targetDirection.Y -= 1;
		UE_LOG(LogTemp, Warning, TEXT("DOWN"));
		break;
	case EMoveDirections::LEFT:
		
		//Hardcoded: I can only move left right if Y == 4
		if (currentlySelectedSlotCoord.Y == 4)
		{
			l_targetDirection.X -= 1;
		//quick hack, if i want to do vertical movement in the menus too then i need to update this a little bit more
		if (bHasOpenedToolbar)
		{
			SetIndexesHidden();
		}
		}
		
		UE_LOG(LogTemp, Warning, TEXT("LEFT"));
		break;
	case EMoveDirections::RIGHT:
		//Hardcoded: I can only move left right if Y == 4
		if (currentlySelectedSlotCoord.Y == 4)
		{
			l_targetDirection.X += 1;
			
		//quick hack, if i want to do vertical movement in the menus too then i need to update this a little bit more
		if (bHasOpenedToolbar)
		{
			SetIndexesHidden();
		}
		}
		UE_LOG(LogTemp, Warning, TEXT("RIGHT"));
		break;
	} 
	//if slot is not valid or is hidden then dont move
	if (mapRefMenuSlots.Find(l_targetDirection))
	{
		UMainMenuSlot* l_slot = *mapRefMenuSlots.Find(l_targetDirection);
		if (l_slot->menuSlotData.menuInventorySlotVisibility != EInventoryVisibilityState::HIDDEN &&
			l_slot->menuSlotData.menuInventorySlotState != EInventorySlotState::DISABLED)
		{
			SelectNeightbourSlot(l_targetDirection);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("direction where I wanted to move is hidden."));
		}

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("direction where I wanted to move is invalid."));
	}
	
}

