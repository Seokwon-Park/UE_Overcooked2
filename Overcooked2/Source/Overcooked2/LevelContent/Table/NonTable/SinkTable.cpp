// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Table/NonTable/SinkTable.h"
#include "LevelContent/Cook/Plate.h"
#include <Character/OC2Character.h>
#include "Components/WidgetComponent.h"
#include <LevelContent/Cook/Widget/GaugeTextureWidget.h>

ASinkTable::ASinkTable()
{
	ProgressBarComponent = CreateDefaultSubobject<UWidgetComponent>("ProgressBar");
	ProgressBarComponent->SetupAttachment(RootComponent);

	CleanPlateComponent = CreateDefaultSubobject<USceneComponent>("CleanPlate");
	CleanPlateComponent->SetupAttachment(RootComponent);
}

void ASinkTable::BeginPlay()
{
	Super::BeginPlay();

	// ���� Ŭ���� ����
	ProgressBarComponent->SetWidgetClass(TSubClassWidget); // WBP �������� ����
	UUserWidget* UserWidget = ProgressBarComponent->GetUserWidgetObject();
	if (nullptr != UserWidget)
	{
		WidgetPtr = Cast<UGaugeTextureWidget>(UserWidget);
	}
	ProgressBarComponent->SetDrawAtDesiredSize(true);   // ������ ���� ũ��� ����
	ProgressBarComponent->SetPivot(FVector2D(0.5f, 0.5f)); // �߽� ����
	ProgressBarComponent->SetWidgetSpace(EWidgetSpace::Screen); // ���� �������� 3Dó�� ���̰�
	ProgressBarComponent->bHiddenInGame = true;

	// ī�޶� ���ϵ��� ����
	ProgressBarComponent->SetTwoSided(true);
	ProgressBarComponent->SetTickWhenOffscreen(true);
}

void ASinkTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckChefIsWashing();

	if (true == bTimerActivated)
	{
		Timer += DeltaTime;

		if (Timer > 2.0f)
		{
			bWashingDone = true;
		}
	}

	Ratio = (Timer / 0.4f) * 0.2f;
	WidgetPtr->SetProgressTimeRatio(Ratio);

	if (bWashingDone == true)
	{
		WashingIsDone();
	}
}

ACooking* ASinkTable::Interact(AActor* ChefActor)
{
	ACooking* CookingReturn = CookingPtr;
	CookingPtr = nullptr;

	return CookingReturn;
}


void ASinkTable::PlaceItem(ACooking* ReceivedCooking)
{
	if (ECookingType::ECT_PLATE == ReceivedCooking->GetCookingType())
	{
		APlate* TempPlate = Cast<APlate>(ReceivedCooking);
		if (true == TempPlate->IsDirtyPlate())
		{
			CookingPtr = ReceivedCooking;
			CookingPtr->SetCookingTable_Implementation(this);
			CookingPtr->AttachToComponent(ComponentForCooking, FAttachmentTransformRules::KeepRelativeTransform);
			CookingPtr->SetActorLocation(ComponentForCooking->GetComponentLocation());
		}
	}
}

void ASinkTable::DoTheDishes(AOC2Character* ChefActor)
{
	ChefPtr = Cast<AOC2Character>(ChefActor);

	if (nullptr != ChefActor && nullptr != CookingPtr)
	{
		ChefPtr->Washing(true);

		Timer = 0.0f;
		bTimerActivated = true;
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, "Washing...");

		ProgressBarComponent->SetHiddenInGame(false);
	}
}

void ASinkTable::WashingIsDone()
{
	bTimerActivated = false;

	APlate* PlacedPlate = Cast<APlate>(CookingPtr);
	PlacedPlate->WashPlate();
	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Turquoise, "Washing Done");
	CookingPtr = Cast<APlate>(PlacedPlate);

	//CookingPtr->DetachAllSceneComponents();
	CookingPtr->AttachToComponent(CleanPlateComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CookingPtr->SetActorLocation(CleanPlateComponent->GetComponentLocation());
	ProgressBarComponent->SetHiddenInGame(true);

	ChefPtr->Washing(false);
	ChefPtr = nullptr;
	bWashingDone = false;
}

void ASinkTable::CheckChefIsWashing()
{
	if (nullptr != ChefPtr)
	{
		if (false == ChefPtr->IsWashing())
		{
			bTimerActivated = false;
			ChefPtr = nullptr;
			ProgressBarComponent->SetHiddenInGame(true);
		}
	}
}
