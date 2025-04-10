// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Table/NonTable/SinkTable.h"
#include "LevelContent/Cook/Plate.h"
#include <Character/OC2Character.h>
#include "Components/WidgetComponent.h"
#include <LevelContent/Cook/Widget/GaugeTextureWidget.h>
#include <Global/Component/TimeEventComponent.h>
#include <Net/UnrealNetwork.h>

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

ACooking* ASinkTable::Interact(AActor* ChefActor)
{
	// �������� ���ð� �ϳ� �̻� ������ ĳ���Ϳ��� �ϳ��� �� �� �ִ�.
	if (CleanPlates.Num() > 0)
	{
		APlate* NewPlate = CleanPlates.Last();
		CookingPtr = Cast<ACooking>(NewPlate);
		if (nullptr != CookingPtr)
		{
			CleanPlates.Pop();

			CleanPlateNum = CleanPlates.Num();
			return CookingPtr;
		}
		else
		{
			return nullptr;
		}

	}
	else
	{
		return nullptr;
	}
}

// ���ð� ��ũ��� ������ ����
void ASinkTable::PlaceItem(ACooking* ReceivedCooking)
{
	PlacePlates(ReceivedCooking);
}

// ���ð� ��ũ��� ������ ����
void ASinkTable::PlacePlates_Implementation(ACooking* ReceivedCooking)
{

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
			// 2-1. �÷���Ʈ�� �ϳ��� ���´ٸ�
			DirtyPlates.Add(TempPlate);

			// 2-2. �÷���Ʈ�� ���� �����
			if (true != TempPlate->GetAnotherPlatesRef().IsEmpty())
			{
				int PlateNum = TempPlate->GetAnotherPlatesRef().Num();

				for (int i = 0; i < PlateNum; i++)
				{
					APlate* NewPlate = TempPlate->GetAnotherPlatesRef()[i];
					NewPlate->ResetForCleaning();

					DirtyPlates.Add(NewPlate);
				}
			}

			// AnotherPlateRef�� ����, �޽� �ʱ�ȭ, ������ Plates �ʱ�ȭ
			TempPlate->ResetForCleaning();

			// 3. �������Ѵ�.
			SetPlateVisibility(DirtyPlates.Num());

			// 4. Plates �� ���忡�� ���ܵ� �༮���� ����� ���ͽ�Ų��.
			for (int32 i = DirtyPlateNum; i < DirtyPlates.Num(); i++)
			{
				APlate* NewPlate = DirtyPlates[i];
				NewPlate->HiddenPlateToWorld();
				NewPlate->SetCookingTable_Implementation(this);
			}

			DirtyPlateNum = DirtyPlates.Num();
		}
	}
}

void ASinkTable::DoTheDishes(AOC2Character* ChefActor)
{
	ChefPtr = Cast<AOC2Character>(ChefActor);

	if (nullptr != ChefActor && false == DirtyPlates.IsEmpty())
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

	// 1. �������� ������ ��
	if (true == bWashingDone && false == DirtyPlates.IsEmpty())
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
			//ProgressBarComponent->SetHiddenInGame(true);
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

	if (0 == DirtyPlates.Num())
	{
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

	// �ε����� ������ ���ø� empty ���·� �ٲٰ�
	APlate* NewPlate = DirtyPlates.Last();
	
	NewPlate->RestorePlateToWorld(); // ���� ������
	NewPlate->WashPlate(); // �ʱ� ���·� �����.

	// CleanPlates�� �߰�, DirtyPlates�� ������ ���� ����
	CleanPlates.Add(NewPlate);
	DirtyPlates.Pop();

	// ��ũ���� �÷���Ʈ ������ ���¸� �ٲ�����Ѵ�.
	//for (size_t i = 0; i < DirtyPlates.Num(); i++)
	//{

	//}

	DirtyPlateNum = DirtyPlates.Num();
	SetPlateVisibility(DirtyPlateNum);

	CleanPlateNum = CleanPlates.Num();

	GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Turquoise, "Washing Done");

	if (CleanPlateNum == 0)
	{
		return;
	}

	// Ŭ�� �÷���Ʈ�� ��ũ�뿡 ������Ų��.
	NewPlate->AttachToComponent(CleanPlateComponent, FAttachmentTransformRules::KeepRelativeTransform);
	NewPlate->SetActorLocation(CleanPlateComponent->GetComponentLocation());

	// Ŭ�� �÷���Ʈ�� Num�� �����ϸ� �׾��ش�.
	NewPlate->AddActorLocalOffset(FVector::UnitZ() * 10.0f * (CleanPlateNum - 1));


	//APlate* PlacedPlate = Cast<APlate>(CookingPtr);
	//PlacedPlate->WashPlate();
	//CookingPtr = Cast<APlate>(PlacedPlate);

	//CookingPtr->DetachAllSceneComponents();
	//ProgressBarComponent->SetHiddenInGame(true);
	HideProgressBar(true);
}

void ASinkTable::HideProgressBar_Implementation(bool Value)
{
	ProgressBarComponent->SetHiddenInGame(Value);
}

void ASinkTable::SetPlateVisibility/*_Implementation*/(int Index)
{
	SetAllPlateHidden();
	for (size_t i = 0; i < Index; i++)
	{
		DirtyPlateComponents[i]->SetVisibility(true);
	}
}

void ASinkTable::SetAllPlateHidden/*_Implementation*/()
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
	//DOREPLIFETIME(ASinkTable, DirtyPlates);
	//DOREPLIFETIME(ASinkTable, CleanPlates);
}