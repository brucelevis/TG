// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TG/Enumerations.h"
#include "DialogueSelectionMenu.generated.h"

/**
 * 
 */
UCLASS()
class TG_API UDialogueSelectionMenu : public UUserWidget
{
	GENERATED_BODY()
public:

	/* ###########################################################
						Initializes & references
	 ########################################################### */
	UFUNCTION(BlueprintImplementableEvent)
	void InitializeRefsInBP();
	void InitializeReferences();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UHorizontalBox* refDialogueMenuHorizontalBox;

	

	 /* #########################END############################## */
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(EditAnywhere, Category = "AWidgets")
		TSubclassOf<class UDialogueSelectButton> WBP_DialogueSelectButton;

	
public:
	void CreateDialogueButtons(const TArray<EDialogueSelectButtons>& iDialogueBtns);


private:
	bool bHaveTheButtonsAlreadyBeenCreated = false;
};
