// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "Components/UniformGridPanel.h"
#include "InventorySlot.h"
#include "TG/Libraries/InventoryLibrary.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/SizeBox.h"
#include "TG/Controllers/ExploreController.h"
#include "TG/TGCharacter.h"
#include "TG/Components/InventoryComponent.h"
#include "PaperSprite.h"
#include "TG/Components/AbilityComponent.h"
#include "Components/BoxComponent.h"
#include "TG/StaticLibrary.h"



void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitializeRefsInBP();
	InitializeRefs();
	AddDelegateBindings();
	GetStartingSlot();



	FTimerHandle invWidgetInitializeWithTimer;
	GetWorld()->GetTimerManager().SetTimer(invWidgetInitializeWithTimer, this, &UInventoryWidget::InitializeWithTimer, 0.5f, false);

	/* ###########################################################
						Default values
	 ########################################################### */

	bAreWeDoingAMoveAction = false;
	bAreWeMovingFromArtifactSlot = false;

	 /* #########################END############################## */
}

void UInventoryWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	InitializeRefsInBP();
	FTimerHandle dsa;
	GetWorld()->GetTimerManager().SetTimer(dsa, this, &UInventoryWidget::CreateInventorySlots, 0.3f, false);
}

void UInventoryWidget::CreateInventorySlots()
{
	if (refInventoryGridPanel == nullptr || refPlayerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("UInventoryWidget::CreateInventorySlots() refInventoryGridPanel == nullptr"));
		return;
	}
	TArray<UInventorySlot*> slotOld = refPlayerCharacter->GetInventoryComponent()->refItemInventory;
	int32 l_column = UInventoryLibrary::GetInventoryGridData(EInventoryType::BAG).columns;
	int32 l_rows = UInventoryLibrary::GetInventoryGridData(EInventoryType::BAG).rows;
	TArray<int32> l_artifactIndexes = UInventoryLibrary::GetInventoryGridData(EInventoryType::BAG).artifactIndexes;
	int32 l_nrOfSlots = (l_rows * l_column) - 1;
	mapRefInventorySlots.Empty();
	refPlayerCharacter->GetInventoryComponent()->refItemInventory.Empty();
	refInventoryGridPanel->ClearChildren();
	invSize = UInventoryLibrary::GetInventorySlotSize(EInventoryType::BAG);
	for (int i = 0; i <= l_nrOfSlots; i++)
	{
		//dont forget to update the class in WBP with the BP one
		UInventorySlot* wInvSlot = CreateWidget<UInventorySlot>(GetWorld(), invSlotClass);
		int32 l_currRow = i - ((i / l_column) * l_column);
		int32 l_currColumn =  i / -l_column;
		refInventoryGridPanel->AddChildToUniformGrid(wInvSlot,
			l_currColumn, l_currRow);
		l_currColumn = FMath::Abs(l_currColumn);
		mapRefInventorySlots.Add(FVector2D(l_currRow, l_currColumn), wInvSlot);
		wInvSlot->slotData.inventorySlotState = EInventorySlotState::EMPTY;
		if (slotOld.IsValidIndex(1)) //TODO !! idk i have no idea how this doesnt or yet hasnt caused a bug
		{
			if (slotOld[i]->slotData.inventorySlotState == EInventorySlotState::EMPTY)
			{
				wInvSlot->slotData.inventorySlotState = EInventorySlotState::EMPTY;
				wInvSlot->slotData.refItem = nullptr;
			}
			else
			{
				wInvSlot->slotData.inventorySlotState = EInventorySlotState::HASITEM;
				wInvSlot->slotData.refItem = slotOld[i]->slotData.refItem;
			}
		}
		
		//gets hardcoded inventory slots from the library, i dont like the for here, but since the array should just have 0-10 entries
		//i think its ok
		for (auto& e : l_artifactIndexes)
		{
			if (i == e)
			{
				wInvSlot->slotData.slotType = ESlotType::ARTIFACT;
				wInvSlot->UpdateInventoryButtonBackgroundImage();
			}
		}
		
		wInvSlot->slotData.slotIndex = i;
		wInvSlot->slotData.inventoryType = EInventoryType::BAG;
		wInvSlot->refWSizeBoxSlotSize->SetWidthOverride(invSize);
		wInvSlot->refWSizeBoxSlotSize->SetHeightOverride(invSize);
		
		//add inventory slot reference to the player's inventory component
		refPlayerCharacter->GetInventoryComponent()->refItemInventory.Add(wInvSlot);
	}

	HighlightSelectedSlot();
}

int32 UInventoryWidget::GetCurrentlySelectedSlotIndex()
{
	UInventorySlot* l_invSlot = *mapRefInventorySlots.Find(currentlySelectedSlotCoord);
	return l_invSlot->slotData.slotIndex;
}

void UInventoryWidget::RefreshInventorySlots()
{
	UE_LOG(LogTemp, Error, TEXT("UInventoryWidget::RefreshInventorySlots()"));
	CreateInventorySlots();
}

void UInventoryWidget::InitializeWithTimer()
{
	UpdateTooltipData();
}

void UInventoryWidget::AddDelegateBindings()
{
	refExplorePlayerController = Cast<AExploreController>(GetWorld()->GetFirstPlayerController());
	refExplorePlayerController->delegateInventoryMove.AddDynamic(this, &UInventoryWidget::MoveInInventory);
	//this is initialized with InitializeRefs, which at least atm is called before this so its safe
	refPlayerCharacter->GetInventoryComponent()->delegateInventoryUpdate.AddDynamic(this, &UInventoryWidget::UpdateItemsFromPlayerInventory);
	refExplorePlayerController->delegateInventoryUseItem.AddDynamic(this, &UInventoryWidget::UseSelectedItem);
	refExplorePlayerController->delegateInventoryDropItem.AddDynamic(this, &UInventoryWidget::DropSelectedItem);
	refExplorePlayerController->delegateInventoryMoveItem.AddDynamic(this, &UInventoryWidget::InventoryMoveActionSelectedItem);

}

void UInventoryWidget::InitializeRefs()
{
	//init player pawn ref
	refPlayerCharacter = Cast<ATGCharacter>(GetWorld()->GetFirstPlayerController()->GetPawn());
}

void UInventoryWidget::MoveInInventory(EMoveDirections iMoveDir)
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
		l_targetDirection.X -= 1;
		UE_LOG(LogTemp, Warning, TEXT("LEFT"));
		break;
	case EMoveDirections::RIGHT:
		l_targetDirection.X += 1;
		UE_LOG(LogTemp, Warning, TEXT("RIGHT"));
		break;
	}
	if (mapRefInventorySlots.Find(l_targetDirection))
	{
		SelectNeighbourSlot(l_targetDirection);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("direction where I wanted to move was invalid."));
	}

}

void UInventoryWidget::HighlightSelectedSlot()
{
	UInventorySlot* l_ref = *mapRefInventorySlots.Find(currentlySelectedSlotCoord);
	l_ref->slotData.bIsSelected = true;
	l_ref->UpdateInventoryButtonBackgroundType();
}

void UInventoryWidget::DeHighlightLastSelectedSlot()
{
	UInventorySlot* l_ref = *mapRefInventorySlots.Find(lastSelectedSlotCoord);
	l_ref->slotData.bIsSelected = false;
	l_ref->UpdateInventoryButtonBackgroundType();
}

void UInventoryWidget::SelectNeighbourSlot(FVector2D iTarget)
{
	lastSelectedSlotCoord = currentlySelectedSlotCoord;
	currentlySelectedSlotCoord = iTarget;
	DeHighlightLastSelectedSlot();
	HighlightSelectedSlot();
	UpdateTooltipData();
}

void UInventoryWidget::GetStartingSlot()
{
	//makes the selection start somewhere from the middle, not really accurate and well thought out, good enough
// 	float l_column = UInventoryLibrary::GetInventoryGridRowsColumns(EInventoryType::BAG).columns;
// 	float l_rows = UInventoryLibrary::GetInventoryGridRowsColumns(EInventoryType::BAG).rows;
// 	l_column = FMath::RoundHalfToEven(l_column / 2);
// 	l_rows = FMath::RoundHalfToEven(l_rows / 2);
// 
// 	//this is just so that with 3x3 selection would be in the middle, there's probably a proper math formula for this
// 	l_column -= 1;
// 	l_rows -= 1;
// 
// 	//clamp it so it doesnt go over the column/row sizes, this function needs const parameters so making temp consts
// 	const int32 l_tmpClmn = l_column;
// 	const int32 l_tmpRows = l_rows;
// 	l_column = FMath::Clamp(l_tmpClmn, 0, UInventoryLibrary::GetInventoryGridRowsColumns(EInventoryType::BAG).columns);
// 	l_rows = FMath::Clamp(l_tmpRows, 0, UInventoryLibrary::GetInventoryGridRowsColumns(EInventoryType::BAG).rows);
// 	FVector2D midVec = FVector2D(l_column, l_rows);
// 	currentlyActiveSlot = midVec; //replace this
	

	//above stuff is attempt to make it start from middle but it breaks with certain sizes, fk it just starts in 0, simple
	currentlySelectedSlotCoord = FVector2D(0,0);
}

void UInventoryWidget::UpdateItemsFromPlayerInventory(TArray<class UInventorySlot*> iPlayerInventory)
{
	UE_LOG(LogTemp, Warning, TEXT(" UInventoryWidget::UpdateItemsFromPlayerInventory()"));
	TArray<UInventorySlot*> mapValues;
	mapRefInventorySlots.GenerateValueArray(mapValues);
	int32 loopCounter = 0;
	for (auto& e : iPlayerInventory)
	{
		if (e->slotData.inventorySlotState == EInventorySlotState::HASITEM)
		{
			FSlateBrush brush;
			brush.SetResourceObject(e->slotData.refItem->currentItemData.itemIcon);
			//update icon
			mapValues[loopCounter]->refWItemIcon->SetBrush(brush);
			mapValues[loopCounter]->refWItemIcon->SetBrushSize(FVector2D(45, 45)); //magic numbers

			//add item pointer
			mapValues[loopCounter]->slotData.refItem = e->slotData.refItem;
		}
		loopCounter++;
	}
}

void UInventoryWidget::UpdateTooltipData()
{


	//lets find the selected slot data
	UInventorySlot* l_currInventorySlot = *mapRefInventorySlots.Find(currentlySelectedSlotCoord);
	if (l_currInventorySlot == nullptr)
	{
		refTooltipBox->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget: l_currInventorySlot == nullptr"));
		return;
	}

	if (l_currInventorySlot->slotData.inventorySlotState == EInventorySlotState::EMPTY)
	{
		refTooltipBox->SetVisibility(ESlateVisibility::Hidden);
		UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget: l_currInventorySlot->slotData.refItem == nullptr"));
		return;
	}

	FItemData l_selectedItemData = l_currInventorySlot->slotData.refItem->currentItemData;



	//update refTextItemName
	refTextItemName->SetText(l_selectedItemData.itemName);
	//update refTextItemRarity && update refBorderName, border color
	switch (l_selectedItemData.itemRarity)
	{
	case EItemRarity::COMMON:
		refBorderName->SetBrushColor(FLinearColor::Gray); //hardcode color
		refTextItemRarity->SetText(FText::FromString("Common"));
		break;
	case EItemRarity::MAGIC:
		refBorderName->SetBrushColor(FLinearColor::Blue); //hardcode color
		refTextItemRarity->SetText(FText::FromString("Magic"));
		break;
	case EItemRarity::ULTRA:
		refBorderName->SetBrushColor(FLinearColor::Green); //hardcode color
		refTextItemRarity->SetText(FText::FromString("UltrA"));
		break;
	}

	//update refTExtItemType
	switch (l_selectedItemData.itemType)
	{
	case EItemType::TOME:
		refTextItemType->SetText(FText::FromString("tome"));
		break;
	case EItemType::TOKEN:
		refTextItemType->SetText(FText::FromString("token"));
		break;
	case EItemType::SOUL:
		refTextItemType->SetText(FText::FromString("soul"));
		break;
	case EItemType::MYSTICAL:
		refTextItemType->SetText(FText::FromString("mystical"));
		break;
	case EItemType::ARTIFACT:
		refTextItemType->SetText(FText::FromString("artifact"));
		break;
	default:
		break;
	}
	//update refTextItemDescription
	refTextItemDescription->SetText(FText::FromString(l_selectedItemData.itemDescription));

	refTooltipBox->SetVisibility(ESlateVisibility::Visible);

	//update refVerticalBoxItemEffects, here we add a child of another widget we create later TODO !!

}

void UInventoryWidget::UseSelectedItem()
{
	//lets find the selected slot data
	if (!mapRefInventorySlots.Contains(currentlySelectedSlotCoord))
	{
		return;
	}
	UInventorySlot* l_currInventorySlot = *mapRefInventorySlots.Find(currentlySelectedSlotCoord);

	UE_LOG(LogTemp, Warning, TEXT("\n SLOTDATA \n Index: %i \n slotType: %s \n slotType: %s"),
		l_currInventorySlot->slotData.slotIndex,
		*UStaticLibrary::GetEnumValueAsString("ESlotType", l_currInventorySlot->slotData.slotType),
		*UStaticLibrary::GetEnumValueAsString("EInventoryType", l_currInventorySlot->slotData.inventoryType)
		);

	if (l_currInventorySlot->slotData.refItem != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Item Name: %s"),
			*l_currInventorySlot->slotData.refItem->currentItemData.itemName.ToString());
	}

	/*switch, different functionality based on slot types, normal slot, artifact slot, destroyed slot, frozen slot whatever i 
	might implement in future if i actually keep developing this idea*/

	switch (l_currInventorySlot->slotData.slotType)
	{
	case ESlotType::NORMAL:

		/*the check what kind of item we are using we're doing in the abilitycomp, including equiping items etc*/
		
		refPlayerCharacter->GetAbilityComponent()->CastAbility(l_currInventorySlot->slotData.slotIndex);

		break;
	case ESlotType::ARTIFACT:

		break;
	case ESlotType::SLOTTYPE2:
		break;
	case ESlotType::SLOTTYPE3:
		break;
	case ESlotType::SLOTTYPE4:
		break;
	case ESlotType::SLOTTYPE5:
		break;
	case ESlotType::SLOTTYPE6:
		break;
	}
	

}

void UInventoryWidget::DropSelectedItem()
{
	if (!mapRefInventorySlots.Contains(currentlySelectedSlotCoord))
	{
		return;
	}
	UInventorySlot* l_currInventorySlot = *mapRefInventorySlots.Find(currentlySelectedSlotCoord);
	if (l_currInventorySlot == nullptr || l_currInventorySlot->slotData.refItem == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget: l_currInventorySlot == nullptr"));
		return;
	}

	//cant drop hearth/soul item
	if (l_currInventorySlot->slotData.refItem->currentItemData.itemType == EItemType::SOUL)
	{
		return;
	}

	FVector playerLoc = refPlayerCharacter->GetActorLocation();
	if (refPlayerCharacter->bIsFacingRight)
	{
		playerLoc.X += 100;
	}
	else
	{
		playerLoc.X -= 100;
	}
	l_currInventorySlot->slotData.refItem->SetActorLocation(playerLoc, true);
	l_currInventorySlot->slotData.refItem->SetActorHiddenInGame(false);
	l_currInventorySlot->slotData.refItem->GetCollisionComp()->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	RefreshInventorySlots();
	refPlayerCharacter->GetInventoryComponent()->DeleteItemFromInventory(l_currInventorySlot->slotData.slotIndex);
}

void UInventoryWidget::InventoryMoveActionSelectedItem()
{
	
	if (!mapRefInventorySlots.Contains(currentlySelectedSlotCoord))
	{
		return;
	}

	UInventorySlot* l_currInventorySlot = *mapRefInventorySlots.Find(currentlySelectedSlotCoord);
	UInventorySlot* l_lastInventorySlot = *mapRefInventorySlots.Find(lastSelectedItemForMoveSlotCoord);
	int32 destinationSlotIdx = l_currInventorySlot->slotData.slotIndex;

	/*if we are not doing move action, select item to be moved*/
	if (!bAreWeDoingAMoveAction)
	{
		//if there is nothing in slot, return
		if (l_currInventorySlot == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("There is nothing in slot"));
			return;
		}

		//set selected item opacity lower
		FLinearColor l_slotColor;
		l_slotColor = l_currInventorySlot->refWItemIcon->ColorAndOpacity;
		l_oldSlotColor = l_slotColor;
		l_slotColor = l_slotColor.CopyWithNewOpacity(0.4);
		l_currInventorySlot->refWItemIcon->SetColorAndOpacity(l_slotColor);

		//store the coordinate of this selected slot, atm need it to switch the opacity back in the old slot
		lastSelectedItemForMoveSlotCoord = currentlySelectedSlotCoord;

		//store index of the slot we want to do the move action on
		bAreWeDoingAMoveAction = true;
		currentlySelectedItemForMoveIndex = l_currInventorySlot->slotData.slotIndex;
		//if we are moving an artifact inside artifact slot we want to be able to move/swap with artifacts inside artifact slot
		if (l_currInventorySlot->slotData.slotType == ESlotType::ARTIFACT)
		{
			bAreWeMovingFromArtifactSlot = true;
		}		
	}
	else
	{
		//if we are trying to move to the same slot, do nothing
		if (l_currInventorySlot->slotData.slotIndex == currentlySelectedItemForMoveIndex)
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget::InventoryActionSelectedItem()  attempting move on the same slot, return"));
			bAreWeDoingAMoveAction = false;
			bAreWeMovingFromArtifactSlot = false;
			l_lastInventorySlot->refWItemIcon->SetColorAndOpacity(l_oldSlotColor);
			return;
		}

		//check if item that we want to move has been removed from the inventory before the swap
		if (!refPlayerCharacter->GetInventoryComponent()->CheckIfSlotHasItem(currentlySelectedItemForMoveIndex))
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget::InventoryActionSelectedItem() item we're attempting to move has been deleted/dropped"));
			bAreWeDoingAMoveAction = false;
			bAreWeMovingFromArtifactSlot = false;
			l_lastInventorySlot->refWItemIcon->SetColorAndOpacity(l_oldSlotColor);
			return;
		}

		/*check if we can actually move the item to the destination slot
		example: we can't just move an item to the artifact slot, that is a special action 
		(but we can move items out of the artifact slot)*/

		
		if (l_currInventorySlot->slotData.slotType == ESlotType::ARTIFACT && !bAreWeMovingFromArtifactSlot)
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget::InventoryActionSelectedItem()  can't move item to inventory slot, return"));
			bAreWeDoingAMoveAction = false;
			bAreWeMovingFromArtifactSlot = false;
			l_lastInventorySlot->refWItemIcon->SetColorAndOpacity(l_oldSlotColor);
			return;
		}

		/*check if there already is an item in the slot, if there is, swap items*/
		/*with artifacts need to make so cant swap with outside items*/
		if (l_currInventorySlot->slotData.refItem != nullptr && l_currInventorySlot->slotData.slotType == ESlotType::ARTIFACT ||
			l_currInventorySlot->slotData.refItem != nullptr && !bAreWeMovingFromArtifactSlot)
		{
			//does the swap logic inside the inventory component
			refPlayerCharacter->GetInventoryComponent()->SwapItemInInventory(currentlySelectedItemForMoveIndex, destinationSlotIdx);
			bAreWeMovingFromArtifactSlot = false;
			bAreWeDoingAMoveAction = false;
			UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget::InventoryActionSelectedItem()  swapped item successfully"));
			l_lastInventorySlot->refWItemIcon->SetColorAndOpacity(l_oldSlotColor);
			return;
		}

		/*if slot is empty, move item to the slot*/
		if (l_currInventorySlot->slotData.refItem == nullptr)
		{
			RefreshInventorySlots();
			refPlayerCharacter->GetInventoryComponent()->MoveItemToAnotherSlot(currentlySelectedItemForMoveIndex, destinationSlotIdx);
			bAreWeMovingFromArtifactSlot = false;
			bAreWeDoingAMoveAction = false;
			UE_LOG(LogTemp, Warning, TEXT("UInventoryWidget::InventoryActionSelectedItem() moved item successfully."));
			l_lastInventorySlot->refWItemIcon->SetColorAndOpacity(l_oldSlotColor);
			return;
		}		
	}	
}

int32 UInventoryWidget::GetFirstEmptyInventorySlotIndex()
{
	TArray<UInventorySlot*> mapValues;
	mapRefInventorySlots.GenerateValueArray(mapValues);
	for (auto& e : mapValues)
	{
		//will add item if slot is empty and slottype is normal, so we wouldn't add item to artifact or any other slot types
		if (e->slotData.inventorySlotState == EInventorySlotState::EMPTY && e->slotData.slotType == ESlotType::NORMAL)
		{
			return e->slotData.slotIndex;
		}
	}
	//inventory apparently is full
	return -1;

}





