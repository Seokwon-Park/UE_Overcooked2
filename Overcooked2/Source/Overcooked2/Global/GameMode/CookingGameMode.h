// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Global/GameMode/OC2GameMode.h"

#include "Global/OC2Struct.h"
#include "Global/OC2Enum.h"
#include "Global/OC2Const.h"

#include "CookingGameMode.generated.h"

class UOrderManageComponent;
class USpawnManageComponent;
class AIngredient;
class APlate;
class AOC2Actor;
class ACookingGameState;
class ACookingTable;
class ASpawnManager;
class AStageManager;

/**
 * �丮 ���� ��� Ŭ���� (ACookingGameMode)
 *
 * Overcooked2 ��Ÿ���� ��Ƽ�÷��� �丮 ������ �����ϴ� ���� ��� Ŭ�����Դϴ�.
 * ������ ���� ��ȯ, �÷��̾� �ʱ�ȭ, �ֹ� ���� ���� �����մϴ�.
 */
UCLASS()
class OVERCOOKED2_API ACookingGameMode : public AOC2GameMode
{
	GENERATED_BODY()

public:
	/** ������ - �⺻ ������ �ʱ�ȭ�մϴ�. */
	ACookingGameMode();

protected:
	/**
	 * ������ ���۵� �� ȣ��Ǵ� �Լ�
	 * �⺻���� ���� ���¿� ȯ���� �ʱ�ȭ�մϴ�.
	 */
	virtual void BeginPlay() override;

	/**
	 * �� �����Ӹ��� ȣ��Ǵ� �Լ�
	 * @param DeltaTime ���� �����Ӱ� ���� ������ ������ �ð� ���� (�� ����)
	 */
	virtual void Tick(float DeltaTime) override;

public:
	void AddPlate(APlate* Plate);
	APlate* GetPlate();

public:
	/** ���� �Լ� */
	void EntryStay();
	void Stay(float DeltaTime);
	void EntryStage();
	void Stage(float DeltaTime);
	void EntryScore();
	void TimeUp(float DeltaTime);
	void EntryTimeUp();
	void Score(float DeltaTime);
	void ChangeState(ECookingGameModeState State);

	/** ����� �޽����� ����ϴ� �Լ� (�׽�Ʈ �� �α� Ȯ�� �뵵) */
	void PrintDebugMessage();

	void PlayBackgroundSound();

	void StartStage();
public:
	/** ���� ���ӿ� ������ �÷��̾� ��Ʈ�ѷ� ��� */
	TArray<APlayerController*> PlayerControllers;

	/** ��� ������ �丮 ���̺� ��� */
	TArray<TSubclassOf<ACookingTable>> CookingTables;

	/** ���� �Ŵ��� ��ü */
	ASpawnManager* SpawnManagerActor = nullptr;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ASpawnManager> SpawnManagerClass = nullptr;

	/** �������� �Ŵ��� ��ü */
	AStageManager* StageManager = nullptr;

	UPROPERTY(EditAnywhere, Category = "Global|Manager")
	TSubclassOf<AStageManager> StageManagerSpawner = nullptr;

private:
	/** �ֹ��� �����ϴ� ������Ʈ */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Global|Order", meta = (AllowPrivateAccess = "true"))
	UOrderManageComponent* OrderManager = nullptr;

	/** ���� ���� ���¸� �����ϴ� ��ü */
	ACookingGameState* CookingGameState = nullptr;

	/** ���� ���� ����� ���� */
	ECookingGameModeState CurState = ECookingGameModeState::ECS_None;


	/** ���� ���õ� �÷��̾� �ε��� */
	int CurIdx = 0;

	/** ���� ���� üũ �ð� */
	float CheckTime = 0.0f;
	bool bShowTimesUpUI = false;

	TArray<APlate*> PlateArray;

	float GameTime = 180.0f;

};
