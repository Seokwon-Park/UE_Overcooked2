// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/OC2CharacterTestTable.h"
#include "Character/OC2Character.h"
#include "Global/GameMode/OC2GameMode.h"
#include "LevelContent/Cook/Dev/CookingDevGameMode.h"

// Sets default values
AOC2CharacterTestTable::AOC2CharacterTestTable()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	WhyDetailsNotShowingAlways = CreateDefaultSubobject<USceneComponent>("Attach");
	WhyDetailsNotShowingAlways->SetupAttachment(RootComponent);

}

AIngredient* AOC2CharacterTestTable::SpawnIngredient(AActor* ChefActor)
{
	FActorSpawnParameters SpawnParameters; // 적절한 오버로딩 함수 호출을 위해(회전값 추가), FActorSpawnParameters 사용
	FVector Location = FVector();
	if (nullptr != ChefActor)
	{
		Location = GetActorLocation() + FVector(0.0f, 0.0f, 100.0f);
	}
	FRotator Rotator = FRotator::ZeroRotator;

	// 1. 재료를 월드에 스폰한다.
	// Transform은 있지만 메시도 없는 빈 껍데기 상태
	AIngredient* NewIngredient = GetWorld()->SpawnActorDeferred<AIngredient>(ActorToSpawn, FTransform());

	// 2. 메시를 찾아서
	NewIngredient->SetType(IngredientType);

	NewIngredient->FinishSpawning(FTransform());

	return NewIngredient;
}

void AOC2CharacterTestTable::PlaceItem(ACooking* Cook)
{
	CurCook = Cook;
	CurCook->AttachToComponent(WhyDetailsNotShowingAlways, FAttachmentTransformRules::KeepRelativeTransform);
	CurCook->SetActorLocation(WhyDetailsNotShowingAlways->GetComponentLocation());
}

void AOC2CharacterTestTable::RequestSpawn_Implementation()
{
	auto GM = Cast<AOC2GameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		SpawnedByServer = GM->SpawnIngredientActor(IngredientType);
	}
}

ACooking* AOC2CharacterTestTable::Interact(AActor* ChefActor)
{
	AOC2Character* Character = Cast<AOC2Character>(ChefActor);

	if (CurCook == nullptr)
	{
		if (Character->IsHolding())
		{
			return nullptr;
		}
		RequestSpawn();
		return SpawnedByServer;
	}
	else
	{
		ACooking* Ret = CurCook;
		CurCook = nullptr;
		return Ret;
	}
}

// Called when the game starts or when spawned
void AOC2CharacterTestTable::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void AOC2CharacterTestTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

