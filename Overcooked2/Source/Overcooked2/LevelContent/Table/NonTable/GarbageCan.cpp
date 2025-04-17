// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Table/NonTable/GarbageCan.h"
#include "Kismet/GameplayStatics.h"
#include "Global/GameMode/OC2GameMode.h"
#include <Global/Data/OC2GlobalData.h>
#include <Net/UnrealNetwork.h>

AGarbageCan::AGarbageCan()
{

}

void AGarbageCan::BeginPlay()
{
	Super::BeginPlay();

	SoundEffect = UOC2GlobalData::GetTableBaseSound(GetWorld(), "TrashCan");
}

void AGarbageCan::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bDestroyGarbage == true)
	{
		Timer -= DeltaTime;
		// ...������ ȸ����Ű��...
		GarbageTransform(DeltaTime);
	}

	if (Timer < 0 && bDestroyGarbage == true)
	{
		DestroyIngredient();
	}
}

void AGarbageCan::PlaceItem(ACooking* Item)
{
	if (nullptr == Item)
	{
		return;
	}

	// Ingredient�� ���ֱ�. Plate�� ��� AOC2Character�� ���
	if (true == Item->IsCookingType(ECookingType::ECT_INGREDIENT))
	{
		CookingPtr = Item;
		CookingPtr->AttachToComponent(ComponentForCooking, FAttachmentTransformRules::KeepRelativeTransform);
		CookingPtr->SetActorLocation(ComponentForCooking->GetComponentLocation());

		bDestroyGarbage = true;
		Timer = 1.5f;

		PlaySoundEffect();
	}
}

void AGarbageCan::DestroyIngredient()
{
	AOC2GameMode* GM = Cast<AOC2GameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->DestroyOC2Actor(CookingPtr);
		CookingPtr = nullptr;
		bDestroyGarbage = false;
	}
}

void AGarbageCan::GarbageTransform(float DeltaTime)
{
	FRotator Rotation = { 0.0, 0.0, 90.0 };
	if (CookingPtr == nullptr) return;
	//FTransform Location = { 0.0, 0.0, -10.0 };
	CookingPtr->AddActorWorldRotation(Rotation * DeltaTime * 2.0);
	FVector Scale = CookingPtr->GetActorScale3D();
	CookingPtr->SetActorScale3D(Scale * 0.985);
	//Garbage->AddActorWorldTransform(Location);
}

void AGarbageCan::PlaySoundEffect_Implementation()
{
	if (nullptr != SoundEffect)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SoundEffect);
	}
}