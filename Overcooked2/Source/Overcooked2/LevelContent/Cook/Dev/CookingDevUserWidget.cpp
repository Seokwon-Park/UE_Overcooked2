// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Cook/Dev/CookingDevUserWidget.h"
#include <LevelContent/Cook/Ingredient.h>
#include "Kismet/GameplayStatics.h"
#include <LevelContent/Cook/Dev/CookingDevGameMode.h>
#include <Global/OC2Global.h>
#include <LevelContent/Cook/Dev/CookingDevHUD.h>
#include <Character/OC2Character.h>
#include <LevelContent/Cook/Dev/CookingDevPlayerState.h>

void UCookingDevUserWidget::ServerSpawnIngredient(EIngredientType Type)
{

	// Pawn���� ��Ʈ��ũ ����ȭ ���� HUD -> SpawnManager -> GameMode
	//APlayerController* Controller = GetOwningPlayer();
	//if (nullptr != Controller)
	//{
	//	AOC2Character* Pawn = Cast<AOC2Character>(Controller->GetPawn());
	//	//Pawn->SpawnManager->SetIngredientType(Type);
	//}

	// PlayerState���� ��Ʈ��ũ ����ȭ ����
	APlayerController* Controller = GetOwningPlayer();
	ACookingDevPlayerState* PlayerState = Controller->GetPlayerState<ACookingDevPlayerState>();
	if (nullptr != PlayerState)
	{
		PlayerState->SetIngredientType(Type);
	}
}

void UCookingDevUserWidget::SpawnPlate(bool bIsClean)
{
	APlayerController* Controller = GetOwningPlayer();
	ACookingDevPlayerState* PlayerState = Controller->GetPlayerState<ACookingDevPlayerState>();
	if (nullptr != PlayerState)
	{
		PlayerState->SpawnPlate(bIsClean);
	}
}

void UCookingDevUserWidget::ChangeState(EIngredientState State)
{
	APlayerController* Controller = GetOwningPlayer();
	ACookingDevPlayerState* PlayerState = Controller->GetPlayerState<ACookingDevPlayerState>();
	if (nullptr != PlayerState)
	{
		PlayerState->ChangeState(State);
	}
}

void UCookingDevUserWidget::PlaceOnthePlate()
{
	APlayerController* Controller = GetOwningPlayer();
	ACookingDevPlayerState* PlayerState = Controller->GetPlayerState<ACookingDevPlayerState>();
	if (nullptr != PlayerState)
	{
		PlayerState->PlaceOnthePlate();
	}
}


void UCookingDevUserWidget::PlaceOnthePot()
{
	APlayerController* Controller = GetOwningPlayer();
	ACookingDevPlayerState* PlayerState = Controller->GetPlayerState<ACookingDevPlayerState>();
	if (nullptr != PlayerState)
	{
		PlayerState->PlaceOnThePot();
	}
}

void UCookingDevUserWidget::Wash()
{
	APlayerController* Controller = GetOwningPlayer();
	ACookingDevPlayerState* PlayerState = Controller->GetPlayerState<ACookingDevPlayerState>();
	if (nullptr != PlayerState)
	{
		PlayerState->Wash();
	}
}

void UCookingDevUserWidget::StackPlate()
{
	APlayerController* Controller = GetOwningPlayer();
	ACookingDevPlayerState* PlayerState = Controller->GetPlayerState<ACookingDevPlayerState>();
	if (nullptr != PlayerState)
	{
		PlayerState->StackPlate();
	}
}

void UCookingDevUserWidget::CleanPlate()
{
	APlayerController* Controller = GetOwningPlayer();
	ACookingDevPlayerState* PlayerState = Controller->GetPlayerState<ACookingDevPlayerState>();
	if (nullptr != PlayerState)
	{
		PlayerState->CleanPlate();
	}
}

void UCookingDevUserWidget::Reset()
{
	APlayerController* Controller = GetOwningPlayer();
	ACookingDevPlayerState* PlayerState = Controller->GetPlayerState<ACookingDevPlayerState>();
	if (nullptr != PlayerState)
	{
		PlayerState->Reset();
	}
}

void UCookingDevUserWidget::StartServer()
{
	FString OpenLevel = TEXT("");
	FString LevelPath = TEXT(""); // ������ ���� ��θ� ������ ����

	UOC2Global::GetAssetPackageName(UWorld::StaticClass(), TEXT("CookingDevLevel"), LevelPath);
	OpenLevel = FString::Printf(TEXT(":%s%s"), *Port, *LevelPath);
	// :30000/Game/Map/CookingDevLevel

	UGameplayStatics::OpenLevel(GetWorld(), *OpenLevel, true, TEXT("listen"));
}

void UCookingDevUserWidget::Connect()
{
	FString ConnectLevelName = FString::Printf(TEXT("%s:%s"), *IP, *Port);
	// 127.0.0.1:30000

	UGameplayStatics::OpenLevel(GetWorld(), FName(*ConnectLevelName));

	// Debug
	UE_LOG(LogTemp, Warning, TEXT("ConnectLevelName: %s"), *ConnectLevelName);
}
