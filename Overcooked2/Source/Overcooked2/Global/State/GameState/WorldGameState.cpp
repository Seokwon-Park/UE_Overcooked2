// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/State/GameState/WorldGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "EngineUtils.h"

#include "UI/WorldMap/WorldMapHUD.h"
#include "UI/WorldMap/UI/WorldMapUserWidget.h"
#include "UI/Loading/LoadingWidget.h"

#include "Global/OC2Global.h"
#include "Global/OC2GameInstance.h"
#include "LevelContent/WorldMap/WorldGameMode.h"
#include "LevelContent/WorldMap/WorldMapActor.h"

AWorldGameState::AWorldGameState()
{
	PrimaryActorTick.bCanEverTick = true;     // ƽ �����ϰ�
	PrimaryActorTick.bStartWithTickEnabled = true; // ���� �ú��� ƽ �ѱ�
}

void AWorldGameState::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<AWorldMapActor> It(GetWorld()); It; ++It)
	{
		WorldMapActor = *It;
	}

	PlayBackgroundSound();
}

void AWorldGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GEngine)
	{
		int32 UserCount = UOC2Global::GetOC2GameInstance(GetWorld())->GetUserCount();
		FString DebugMessage = FString::Printf(TEXT("UserCount: %d"), UserCount);
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DebugMessage);
	}

}

void AWorldGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AWorldGameState, CurUserCount);
}

void AWorldGameState::Server_AddUserCount_Implementation()
{
	if (true == HasAuthority())
	{
		CurUserCount++;
	}
}

void AWorldGameState::CheckClinetLoadingComplete()
{
	if (true == HasAuthority())
	{
		AWorldGameMode* GameMode = Cast<AWorldGameMode>(UGameplayStatics::GetGameMode(this));

		if (nullptr != GameMode)
		{
			CompleteArray.Add(0);

			int Test = GameMode->GetUserCount();

#if UE_BUILD_SHIPPING
			UE_LOG(LogTemp, Warning, TEXT("CurUserCount: %d"), CurUserCount); // Shipping ���忡���� ifdef�� ���α�
#endif
			if (CompleteArray.Num() == UOC2Global::GetOC2GameInstance(GetWorld())->GetUserCount())
			{
				UOC2Global::GetOC2GameInstance(GetWorld())->StartCookingStage();
			}
		}
	}
}

void AWorldGameState::PlayBackgroundSound()
{
	if (nullptr != WorldMapActor)
	{
		WorldMapActor->PlaySound();
	}
}

void AWorldGameState::Multicast_PlayZoomInAnmationUI_Implementation()
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	AWorldMapHUD* WorldMapHUD = Cast<AWorldMapHUD>(PlayerController->GetHUD());

	if (WorldMapHUD->WorldMapUserWidget != nullptr && WorldMapHUD != nullptr)
	{
		WorldMapHUD->WorldMapUserWidget->PlayZoomInAnimation();
	}
}
