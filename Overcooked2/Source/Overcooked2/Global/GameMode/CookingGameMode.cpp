// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/GameMode/CookingGameMode.h"
#include "Global/Component/OrderManageComponent.h"

ACookingGameMode::ACookingGameMode()
{
	OrderManager = CreateDefaultSubobject<UOrderManageComponent>(TEXT("OrderManager"));
}

void ACookingGameMode::BeginPlay()
{
	Super::BeginPlay();

	ChangeState(ECookingGameModeState::ECS_Stay);
}

void ACookingGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurState)
	{
	case ECookingGameModeState::ECS_Stay:
		Stay(DeltaTime);
		break;
	case ECookingGameModeState::ECS_Stage:
		Stage(DeltaTime);
		break;
	case ECookingGameModeState::ECS_Score:
		Score(DeltaTime);
		break;
	default:
		break;
	}
}

void ACookingGameMode::PostLogin(APlayerController* NewPlayerController)
{
	Super::PostLogin(NewPlayerController);

	UE_LOG(LogTemp, Warning, TEXT("Player %s has joined the game!"), *NewPlayerController->GetName());

	PlayerControllers.Push(NewPlayerController);

	if (4 == PlayerControllers.Num())
	{
		ChangeState(ECookingGameModeState::ECS_Stage);
	}
}

void ACookingGameMode::EntryStay()
{
}

void ACookingGameMode::Stay(float DeltaTime)
{
}

void ACookingGameMode::EntryStage()
{
}

void ACookingGameMode::Stage(float DeltaTime)
{
}

void ACookingGameMode::EntryScore()
{
}

void ACookingGameMode::Score(float DeltaTime)
{
}

void ACookingGameMode::ChangeState(ECookingGameModeState State)
{
	CurState = State;

	switch (CurState)
	{
	case ECookingGameModeState::ECS_Stay:
		EntryStay();
		break;
	case ECookingGameModeState::ECS_Stage:
		EntryStage();
		break;
	case ECookingGameModeState::ECS_Score:
		EntryScore();
		break;
	default:
		break;
	}
}

void ACookingGameMode::PrintDebugMessage()
{
	switch (CurState)
	{
	case ECookingGameModeState::ECS_Stay:
		UE_LOG(LogTemp, Warning, TEXT("GameMode State : Stay"));
		break;
	case ECookingGameModeState::ECS_Stage:
		UE_LOG(LogTemp, Warning, TEXT("GameMode State : Stage"));
		break;
	case ECookingGameModeState::ECS_Score:
		UE_LOG(LogTemp, Warning, TEXT("GameMode State : Score"));
		break;
	default:
		break;
	}
}
