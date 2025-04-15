// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Map/OC2Map.h"
#include <Global/Data/OC2GlobalData.h>
#include <Global/OC2Const.h>
#include "Kismet/GameplayStatics.h"

AOC2Map::AOC2Map()
{
	FVector Scale(100.0f, 100.0f, 100.0f);
	SetActorRelativeScale3D(Scale);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	RootComponent = StaticMeshComponent;
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	//StaticMeshComponent->SetCollisionResponseToChannels
	StaticMeshComponent->ComponentTags.Add(TEXT("Floor"));
	
	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(RootComponent);
	AudioComponent->bAutoActivate = false; // �ڵ���� ����

	AmbienceAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AmbienceAudioComponent"));
	AmbienceAudioComponent->SetupAttachment(RootComponent);
	AmbienceAudioComponent->bAutoActivate = false; // �ڵ���� ����

}

void AOC2Map::PlaySound()
{
	if (nullptr != AudioComponent)
	{
		AudioComponent->Play(); // ��� ����
	}
	if (nullptr != AmbienceAudioComponent)
	{
		AmbienceAudioComponent->Play(); // ��� ����
	}

}

void AOC2Map::StopSound()
{
	if (nullptr != AudioComponent)
	{
		AudioComponent->Stop(); // ����
	}
	if (nullptr != AmbienceAudioComponent)
	{
		AmbienceAudioComponent->Stop(); // ����
	}
}

void AOC2Map::BeginPlay()
{
	Super::BeginPlay();
	{
		FString BackgroundSound = TEXT("TheNeonCity");
		USoundBase* TheNeonCitySound = UOC2GlobalData::GetCookingBaseSound(GetWorld(), *BackgroundSound);
		if (nullptr != TheNeonCitySound && nullptr != AudioComponent)
		{
			AudioComponent->SetSound(TheNeonCitySound); // ���� ����
			AudioComponent->bIsUISound = true; // ��ġ �����ϰ� 2Dó�� ���
			AudioComponent->SetVolumeMultiplier(UOC2Const::BGSoundVolume);
		}
	}
	{
		FString NewSound = TEXT("SushiCrowdAmbience");
		USoundBase* SoundBase = UOC2GlobalData::GetCookingBaseSound(GetWorld(), *NewSound);
		if (nullptr != SoundBase && nullptr != AmbienceAudioComponent)
		{
			AmbienceAudioComponent->SetSound(SoundBase); // ���� ����
			AmbienceAudioComponent->bIsUISound = true; // ��ġ �����ϰ� 2Dó�� ���
			AmbienceAudioComponent->SetVolumeMultiplier(UOC2Const::AmbienceVolume);
		}
	}
	//PlaySound();
	//UGameplayStatics::PlaySound2D(GetWorld(), TheNeonCitySound);
	
}

void AOC2Map::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
