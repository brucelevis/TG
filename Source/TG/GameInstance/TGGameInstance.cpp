// Fill out your copyright notice in the Description page of Project Settings.


#include "TGGameInstance.h"

UTGGameInstance::UTGGameInstance()
{

	//if main menu is done, here we set main menu as starting gamestate


}

void UTGGameInstance::SetTGGameState(ETGGameState iNewGameState)
{
	TGGameState = iNewGameState;
}

ETGGameState UTGGameInstance::GetTGGameState()
{
	return TGGameState;
}

