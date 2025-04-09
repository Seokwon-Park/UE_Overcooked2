// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Cook/Dev/CookingDevPlayerState.h"
#include <Net/UnrealNetwork.h>
#include <LevelContent/Cook/Dev/CookingDevGameMode.h>
#include <LevelContent/Cook/Ingredient.h>

ACookingDevPlayerState::ACookingDevPlayerState()
{
	bReplicates = true;
}

void ACookingDevPlayerState::SetIngredientType_Implementation(EIngredientType Type)
{
	IngredientType = Type;
	RequestSpawnIngredient();
}

void ACookingDevPlayerState::RequestSpawnIngredient_Implementation()
{
	ACookingDevGameMode* GameMode = Cast<ACookingDevGameMode>(GetWorld()->GetAuthGameMode());
	if (nullptr != GameMode)
	{
		AIngredient* NewIngredient = GameMode->SpawnIngredientActor(IngredientType);
		GameMode->AddIngredient(NewIngredient);
	}
}

void ACookingDevPlayerState::SpawnPlate_Implementation(bool bIsClean)
{
	if (false == bIsClean)
	{
		PlateState = EPlateState::DIRTY;
	}
	ACookingDevGameMode* GameMode = Cast<ACookingDevGameMode>(GetWorld()->GetAuthGameMode());
	if (nullptr != GameMode)
	{
		APlate* NewPlate = GameMode->SpawnPlateActor(PlateState);
		GameMode->AddPlate(NewPlate);
	}
}

void ACookingDevPlayerState::ChangeState_Implementation(EIngredientState State)
{
	ACookingDevGameMode* GameMode = Cast<ACookingDevGameMode>(GetWorld()->GetAuthGameMode());
	if (nullptr != GameMode)
	{
		GameMode->ChangeState(State);
	}
}

void ACookingDevPlayerState::PlaceOnthePlate_Implementation()
{
	ACookingDevGameMode* GameMode = Cast<ACookingDevGameMode>(GetWorld()->GetAuthGameMode());
	if (nullptr != GameMode)
	{
		GameMode->PlaceOnthePlate();
	}
}

void ACookingDevPlayerState::PlaceOnThePot_Implementation()
{
	ACookingDevGameMode* GameMode = Cast<ACookingDevGameMode>(GetWorld()->GetAuthGameMode());
	if (nullptr != GameMode)
	{
		GameMode->PlaceOnThePot();
	}
}

void ACookingDevPlayerState::Wash_Implementation()
{
	ACookingDevGameMode* GameMode = Cast<ACookingDevGameMode>(GetWorld()->GetAuthGameMode());
	if (nullptr != GameMode)
	{
		GameMode->Wash();
	}
}

void ACookingDevPlayerState::Reset_Implementation()
{
	ACookingDevGameMode* GameMode = Cast<ACookingDevGameMode>(GetWorld()->GetAuthGameMode());
	if (nullptr != GameMode)
	{
		GameMode->Reset();
	}
}

void ACookingDevPlayerState::StackPlate_Implementation()
{
	ACookingDevGameMode* GameMode = Cast<ACookingDevGameMode>(GetWorld()->GetAuthGameMode());
	if (nullptr != GameMode)
	{
		GameMode->StackPlate();
	}
}

void ACookingDevPlayerState::CleanPlate_Implementation()
{
	ACookingDevGameMode* GameMode = Cast<ACookingDevGameMode>(GetWorld()->GetAuthGameMode());
	if (nullptr != GameMode)
	{
		GameMode->CleanPlate();
	}
}

void ACookingDevPlayerState::BeginPlay()
{
	Super::BeginPlay();

}

void ACookingDevPlayerState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ACookingDevPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACookingDevPlayerState, IngredientType);
	DOREPLIFETIME(ACookingDevPlayerState, IngredientState);
}
