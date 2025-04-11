// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Table/NonTable/SinkTable.h"
#include "LevelContent/Cook/Plate.h"
#include <Character/OC2Character.h>
#include "Components/WidgetComponent.h"
#include <LevelContent/Cook/Widget/GaugeTextureWidget.h>
#include <Global/Component/TimeEventComponent.h>
#include <Net/UnrealNetwork.h>
#include <Global/OC2Global.h>

ASinkTable::ASinkTable()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ProgressBarComponent = CreateDefaultSubobject<UWidgetComponent>("ProgressBar");
	ProgressBarComponent->SetupAttachment(RootComponent);

	CleanPlateComponent = CreateDefaultSubobject<USceneComponent>("CleanPlate");
	CleanPlateComponent->SetupAttachment(RootComponent);

	{
		ComponentForDishes1 = CreateDefaultSubobject<UStaticMeshComponent>("DirtyPlate1");
		ComponentForDishes1->SetupAttachment(ComponentForCooking);
		DirtyPlateComponents.Add(ComponentForDishes1);

		ComponentForDishes2 = CreateDefaultSubobject<UStaticMeshComponent>("DirtyPlate2");
		ComponentForDishes2->SetupAttachment(ComponentForCooking);
		DirtyPlateComponents.Add(ComponentForDishes2);

		ComponentForDishes3 = CreateDefaultSubobject<UStaticMeshComponent>("DirtyPlate3");
		ComponentForDishes3->SetupAttachment(ComponentForCooking);
		DirtyPlateComponents.Add(ComponentForDishes3);

		ComponentForDishes4 = CreateDefaultSubobject<UStaticMeshComponent>("DirtyPlate4");
		ComponentForDishes4->SetupAttachment(ComponentForCooking);
		DirtyPlateComponents.Add(ComponentForDishes4);
	}

	TimeEventComponent = CreateDefaultSubobject<UTimeEventComponent>(TEXT("TimeEventComponent"));

}

void ASinkTable::BeginPlay()
{
	Super::BeginPlay();

	InitProgressBar();

	SetAllPlateHidden();
}

void ASinkTable::InitProgressBar()
{
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

void ASinkTable::AddDirtyPlateNum_Implementation(int Value)
{
	DirtyPlateNum += Value;
	if (4 < DirtyPlateNum)
	{
		DirtyPlateNum = 4;
	}
	else if (0 > DirtyPlateNum)
	{
		DirtyPlateNum = 0;
	}
}

void ASinkTable::AddCleanPlateNum_Implementation(int Value)
{
	CleanPlateNum += Value;

	if (4 < CleanPlateNum)
	{
		CleanPlateNum = 4;
	}
	else if (0 > CleanPlateNum)
	{
		CleanPlateNum = 0;
	}
}

ACooking* ASinkTable::Interact(AActor* ChefActor)
{
	// �������� ���ð� �ϳ� �̻� ������ ĳ���Ϳ��� �ϳ��� �� �� �ִ�.
	if (0 >=  CleanPlateNum)
	{
		return nullptr;
	}

	AddCleanPlateNum(-1);

	if (0 == CleanPlateNum) // 1���� ������ ������ �̸� ����� �����Ա� ������ �����͸� ��ȯ
	{
		APlate* NewPlate = BottomPlate;
		BottomPlate = nullptr;

		return NewPlate;
	}

	APlate* NewPlate = UOC2Global::GetPlate(GetWorld());
	if (nullptr != NewPlate)
	{
		NewPlate->RestorePlateToWorld();
		return NewPlate;
	}

	return nullptr;
}

// ���ð� ��ũ��� ������ ����
void ASinkTable::PlaceItem(ACooking* ReceivedCooking)
{
	PlacePlates(ReceivedCooking);
}

// ���ð� ��ũ��� ������ ����
void ASinkTable::PlacePlates_Implementation(ACooking* ReceivedCooking)
{
	if (nullptr == ReceivedCooking)
	{
		return;
	}

	if (ECookingType::ECT_PLATE == ReceivedCooking->GetCookingType())
	{
		APlate* TempPlate = Cast<APlate>(ReceivedCooking);
		if (nullptr == TempPlate)
		{
			return;
		}

		// 1. ��Ƽ �÷���Ʈ�� ���� �� �ִ�.
		if (true == TempPlate->IsDirtyPlate())
		{
			//										N - 1�� + 1�� = N��
			AddDirtyPlateNum(TempPlate->GetPlateStackCount() + 1);
			TempPlate->HiddenPlateToWorld();
			SetPlateVisibility(DirtyPlateNum); // Render
		}
	}
}

void ASinkTable::DoTheDishes(AOC2Character* ChefActor)
{
	ChefPtr = Cast<AOC2Character>(ChefActor);

	// ������ ��ũ�븦 ������ �ʾҰ� �������� ���ð� �����ִٸ�
	if (nullptr != ChefActor && 0 < DirtyPlateNum)
	{
		ChefPtr->Washing(true);

		Timer = 0.0f;
		bTimerActivated = true;
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Magenta, "Washing...");

		HideProgressBar(false);
		//KeepWashing = true;
		//ProgressBarComponent->SetHiddenInGame(false);
	}
}

void ASinkTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ������ ������ ��ȣ�ۿ��� �ߴ��ϰ� ��������
	CheckChefIsWashing();

	UpdateProgressBar(DeltaTime);

	WashingIsDone();

	// 1. �������� ������ �� ���� ������ �� ���ð� �����ִٸ� �ٽ� ������ ���� ȣ��
	if (true == bWashingDone && 0 < DirtyPlateNum)
	{
		DoTheDishes(ChefPtr); // ������ ó���� �� �� �ҷ��ش�. ���Ŀ��� ���� �ҷ���
		bWashingDone = false;
		return;
	}
	else if (true == bWashingDone)
	{
		// ������ ��� �ߴ�
		ChefPtr->Washing(false);
		bWashingDone = false;
		return;
	}
}

void ASinkTable::CheckChefIsWashing()
{
	// ������ ���� ��ȣ�ۿ��� �ϴ� �߿�
	if (nullptr != ChefPtr)
	{
		// �ٸ� �ൿ���� ��ȣ�ۿ��� �ߴܵǾ��ٸ�
		if (false == ChefPtr->IsWashing())
		{
			bTimerActivated = false;
			ChefPtr = nullptr;
			HideProgressBar(true);
		}
	}
}

void ASinkTable::UpdateProgressBar(float DeltaTime)
{
	if (true == bTimerActivated /*&& true == KeepWashing*/)
	{
		Timer += DeltaTime;

		if (Timer > 2.0f)
		{
			// Ʈ���� ����
			bWashingDone = true;
		}
	}

	Ratio = (Timer / 0.4f) * 0.2f;
	WidgetPtr->SetProgressTimeRatio(Ratio);
}

// ������ �Ϸ����� �� �� �� ȣ��, ��ũ��� ���ø� �ϳ� �̵���Ų��.
void ASinkTable::WashingIsDone_Implementation()
{
	// ������ ���̶�� ����
	if (false == bWashingDone)
	{
		return;
	}

	bTimerActivated = false; // Ÿ�̸� ����

	if (0 >= DirtyPlateNum)
	{
		DirtyPlateNum = 0;
		return;
	}

	if (nullptr != GetWorld()->GetAuthGameMode())
	{
		int a = 0;
	}
	if (nullptr == GetWorld()->GetAuthGameMode())
	{
		int a = 0;
	}

	AddDirtyPlateNum(-1);
	AddCleanPlateNum(1);

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Turquoise, "Washing Done");

	HideProgressBar(true);

	// Ŭ�� �÷���Ʈ�� ��ũ�뿡 ������Ų��.
	if (nullptr == BottomPlate)
	{
		BottomPlate = UOC2Global::GetPlate(GetWorld());
		if (nullptr != BottomPlate)
		{
			BottomPlate->AttachToComponent(CleanPlateComponent, FAttachmentTransformRules::KeepRelativeTransform);
			BottomPlate->SetActorLocation(CleanPlateComponent->GetComponentLocation());

			// Ŭ�� �÷���Ʈ�� Num�� �����ϸ� �׾��ش�.
			BottomPlate->AddActorLocalOffset(FVector::UnitZ() * 15.0f * (CleanPlateNum));
			BottomPlate->RestorePlateToWorld();
		}
	}

	// ������ �÷���Ʈ�� �������� CleanPlate ������ ������ �����Ѵ�.
	BottomPlate->SetPlateStackCount(CleanPlateNum - 1);

	// ��ũ�� �ȿ� �ִ� Plate ���� ������
	SetPlateVisibility(DirtyPlateNum); 
}

void ASinkTable::HideProgressBar_Implementation(bool Value)
{
	ProgressBarComponent->SetHiddenInGame(Value);
}

void ASinkTable::SetPlateVisibility_Implementation(int Index)
{
	SetAllPlateHidden();
	for (size_t i = 0; i < Index; i++)
	{
		DirtyPlateComponents[i]->SetVisibility(true);
	}
}

void ASinkTable::SetAllPlateHidden_Implementation()
{
	for (int32 i = 0; i < DirtyPlateComponents.Num(); i++)
	{
		DirtyPlateComponents[i]->SetVisibility(false);
	}
}

void ASinkTable::SetPlateVisibilityWithIndex/*_Implementation*/(int Index, bool Value)
{
	DirtyPlateComponents[Index]->SetVisibility(Value);
}

void ASinkTable::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASinkTable, ChefPtr);
	DOREPLIFETIME(ASinkTable, Timer);
	DOREPLIFETIME(ASinkTable, bTimerActivated);
	DOREPLIFETIME(ASinkTable, bWashingDone);
	DOREPLIFETIME(ASinkTable, Ratio);
	DOREPLIFETIME(ASinkTable, ComponentForDishes1);
	DOREPLIFETIME(ASinkTable, ComponentForDishes2);
	DOREPLIFETIME(ASinkTable, ComponentForDishes3);
	DOREPLIFETIME(ASinkTable, ComponentForDishes4);
	DOREPLIFETIME(ASinkTable, DirtyPlateComponents);
	//DOREPLIFETIME(ASinkTable, CleanPlateNum);
	//DOREPLIFETIME(ASinkTable, DirtyPlateNum);
	//DOREPLIFETIME(ASinkTable, DirtyPlates);
	//DOREPLIFETIME(ASinkTable, CleanPlates);
}