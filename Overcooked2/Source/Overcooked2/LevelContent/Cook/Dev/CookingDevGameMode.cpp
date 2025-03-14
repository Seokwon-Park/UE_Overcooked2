// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Cook/Dev/CookingDevGameMode.h"
#include <LevelContent/Cook/Ingredient.h>
#include <LevelContent/Cook/Dev/CookingDevHUD.h>
#include <LevelContent/Cook/Dev/CookingDevUserWidget.h>
#include <Global/OC2Global.h>
#include <LevelContent/Cook/Plate.h>

ACookingDevGameMode::ACookingDevGameMode()
{
}

void ACookingDevGameMode::ChangeState(EIngredientState State)
{
	if (true == Ingredients.IsEmpty())
	{
		return;
	}

	EIngredientState IngredientState = State;
	for (int i = 0; i < Ingredients.Num(); i++)
	{
		AIngredient* Ingredient = Cast<AIngredient>(Ingredients[i]);
		if (IngredientState == Ingredient->GetCurIngredientState())
		{
			continue;
		}
		else
		{
			Ingredient->ChangeState(IngredientState);
		}
	}
}

void ACookingDevGameMode::PlaceOnthePlate()
{
	if (true == Ingredients.IsEmpty())
	{
		return;
	}
	AIngredient* TargetIngredient = Ingredients[0];
	if (false == TargetIngredient->IsCooked())
	{
		return;
	}
	if (true == Plates[0]->Add(TargetIngredient)) 
	{
		// 재료 추가 성공 시
		Ingredients.RemoveAt(0);
		return;
	}

	// 재료 추가 실패 시
	int a = 0;
	
}

void ACookingDevGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ACookingDevGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
