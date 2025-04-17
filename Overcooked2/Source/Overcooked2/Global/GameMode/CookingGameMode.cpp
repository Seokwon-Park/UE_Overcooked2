// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/GameMode/CookingGameMode.h"

#include "Global/OC2GameInstance.h"
#include "Global/GameFramework/OC2Actor.h"
#include "Global/Component/OrderManageComponent.h"
#include "Global/Component/SpawnManageComponent.h"
#include "Global/Data/IngredientDataTable.h"
#include "Global/Data/OC2GlobalData.h"
#include "Global/OC2Global.h"
#include "Global/State/GameState/CookingGameState.h"
#include "Global/Manager/SpawnManager.h"
#include "Global/Manager/StageManager.h"

#include "UI/Cooking/CookingHUD.h"
#include "UI/Cooking/UI/CookingWidget.h"

#include "Character/OC2Character.h"

#include "Kismet/GameplayStatics.h"


ACookingGameMode::ACookingGameMode()
{
	OrderManager = CreateDefaultSubobject<UOrderManageComponent>(TEXT("OrderManager"));

	ChefHeadNames.Add(UOC2Const::ChefEagleHeadName);
	ChefHeadNames.Add(UOC2Const::ChefMouseHeadName);
	ChefHeadNames.Add(UOC2Const::ChefPandaHeadName);
	ChefHeadNames.Add(UOC2Const::ChefPigHeadName);
}

void ACookingGameMode::BeginPlay()
{
	Super::BeginPlay();

	CookingGameState = GetGameState<ACookingGameState>();

	if (true == HasAuthority()) // ���������� ����
	{
		SpawnManagerActor = GetWorld()->SpawnActor<ASpawnManager>(SpawnManagerClass);
	}
	if (true == HasAuthority())
	{
		StageManager = GetWorld()->SpawnActor<AStageManager>(StageManagerSpawner);
	}

	ChangeState(ECookingGameModeState::ECS_Stay);


	CurIdx = 0;
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

	//PlayerControllerArray.Add(NewPlayerController);

	//if (PlayerControllerArray.Num() == 1)
	//{
	//	ChangeState(ECookingGameModeState::ECS_Stay);
	//}
}

void ACookingGameMode::AddPlate(APlate* Plate)
{
	PlateArray.Add(Plate);
}

APlate* ACookingGameMode::GetPlate()
{
	if (PlateArray.Num() > 0)
	{
		APlate* LastPlate = PlateArray.Last();
		PlateArray.Pop();

		return LastPlate;
	}
	else
	{
		UE_LOG(OVERCOOKED_LOG, Display, TEXT("PlateArray is empty!"));
		return nullptr;
	}
}

void ACookingGameMode::InitChef()
{
	UWorld* World = GetWorld();

	if (nullptr == World)
	{
		return;
	}

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (nullptr != PlayerController)
		{
			ACharacter* DefaultCharacter = PlayerController->GetCharacter();

			if (nullptr != DefaultCharacter)
			{
				AOC2Character* OC2Character = Cast<AOC2Character>(DefaultCharacter);

				if (nullptr != OC2Character)
				{
					OC2Character->SetCharacterName(ChefHeadNames[CurIdx]);
					CurIdx++;
				}
			}
		}
	}
}

void ACookingGameMode::EntryStay()
{
	CheckTime = 0.0f;

	UWorld* World = GetWorld();

	if (nullptr == World)
	{
		return;
	}

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (nullptr != PlayerController)
		{
			ACharacter* DefaultCharacter = PlayerController->GetCharacter();

			if (nullptr != DefaultCharacter)
			{
				AOC2Character* OC2Character = Cast<AOC2Character>(DefaultCharacter);

				if (nullptr != OC2Character)
				{
					OC2Character->SetMoveEnabled(false); // �̵� �Ұ�
				}
			}
		}
	}
}

void ACookingGameMode::Stay(float DeltaTime)
{
	CheckTime += DeltaTime;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (nullptr != PlayerController)
	{
		ACookingHUD* CookingHUD = Cast<ACookingHUD>(PlayerController->GetHUD());

		if (nullptr != CookingHUD && nullptr != CookingHUD->CookWidget)
		{
			if (true == CookingHUD->CookWidget->GetIsReady())
			{
				ChangeState(ECookingGameModeState::ECS_Stage);

				if (nullptr != CookingGameState)
				{
					CookingGameState->Multicast_StartGame();
				}
			}
		}
	}
}

void ACookingGameMode::EntryStage()
{
	InitChef();
	StageManager->bProgress = true;

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&ACookingGameMode::PlayBackgroundSound,
		3.0f,   // 3�� �� ����
		false   // �ݺ� ����(false�� 1ȸ ����)
	);

	UWorld* World = GetWorld();

	if (nullptr == World)
	{
		return;
	}

	for (FConstPlayerControllerIterator Iterator = World->GetPlayerControllerIterator(); Iterator; ++Iterator)
	{
		APlayerController* PlayerController = Iterator->Get();

		if (nullptr != PlayerController)
		{
			ACharacter* DefaultCharacter = PlayerController->GetCharacter();

			if (nullptr != DefaultCharacter)
			{
				AOC2Character* OC2Character = Cast<AOC2Character>(DefaultCharacter);

				if (nullptr != OC2Character)
				{
					OC2Character->SetMoveEnabled(true); // �̵� �Ұ�
				}
			}
		}
	}

	CheckTime = 0.0f;
}

void ACookingGameMode::Stage(float DeltaTime)
{
	if (nullptr != CookingGameState)
	{
		CookingGameState->Multicast_SettingTimer(DeltaTime);
	}

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	if (nullptr != PlayerController)
	{
		ACookingHUD* CookingHUD = Cast<ACookingHUD>(PlayerController->GetHUD());

		if (nullptr != CookingHUD && nullptr != CookingHUD->CookWidget)
		{
			if (true == CookingHUD->CookWidget->IsShowScoreWidget())
			{
				ChangeState(ECookingGameModeState::ECS_Score);
			}
		}
	}
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

void ACookingGameMode::PlayBackgroundSound()
{
	CookingGameState->Multicast_PlayGameMapSound();
}
