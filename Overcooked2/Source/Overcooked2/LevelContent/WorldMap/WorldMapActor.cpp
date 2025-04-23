// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/WorldMap/WorldMapActor.h"

#include "Components/AudioComponent.h"

#include "Global/OC2Global.h"
#include "Global/Data/OC2GlobalData.h"

AWorldMapActor::AWorldMapActor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false; // �ڵ���� ����

}

void AWorldMapActor::BeginPlay()
{
	Super::BeginPlay();

	FString BackgroundSound = TEXT("Background");
	USoundBase* WorldBGSound = UOC2GlobalData::GetWorldBaseSound(GetWorld(), *BackgroundSound);

	if (nullptr != WorldBGSound && nullptr != AudioComponent)
	{
		AudioComponent->SetSound(WorldBGSound); // ���� ����
		AudioComponent->bIsUISound = true; // ��ġ �����ϰ� 2Dó�� ���
		AudioComponent->SetVolumeMultiplier(UOC2Const::BGSoundVolume);
	}
	
	SetActorHiddenInGame(true);
}

// Called every frame
void AWorldMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldMapActor::PlaySound()
{
	if (nullptr != AudioComponent)
	{
		AudioComponent->Play(); // ��� ����
	}
}

void AWorldMapActor::StopSound()
{
	if (nullptr != AudioComponent)
	{
		AudioComponent->Stop(); // ����
	}
}

