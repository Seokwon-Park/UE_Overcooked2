// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Table/NonTable/SinkTable.h"
#include "LevelContent/Cook/Plate.h"
#include <Character/OC2Character.h>
#include "Components/WidgetComponent.h"
#include <LevelContent/Cook/Widget/GaugeTextureWidget.h>
#include <Global/Component/TimeEventComponent.h>
#include <Net/UnrealNetwork.h>
#include <Global/OC2Global.h>
#include <Global/Data/OC2GlobalData.h>
#include "Kismet/GameplayStatics.h"

ASinkTable::ASinkTable()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	ProgressBarComponent = CreateDefaultSubobject<UWidgetComponent>("ProgressBar");
	ProgressBarComponent->SetupAttachment(RootComponent);

	CleanPlateComponent = CreateDefaultSubobject<USceneComponent>("CleanPlate");
	CleanPlateComponent->SetupAttachment(RootComponent);

	{
		ComponentForDishes1 = CreateDefaultSubobject<USceneComponent>("DirtyPlate1");
		ComponentForDishes1->SetupAttachment(ComponentForCooking);
		DirtyPlateComponents.Add(ComponentForDishes1);

		ComponentForDishes2 = CreateDefaultSubobject<USceneComponent>("DirtyPlate2");
		ComponentForDishes2->SetupAttachment(ComponentForCooking);
		DirtyPlateComponents.Add(ComponentForDishes2);

		ComponentForDishes3 = CreateDefaultSubobject<USceneComponent>("DirtyPlate3");
		ComponentForDishes3->SetupAttachment(ComponentForCooking);
		DirtyPlateComponents.Add(ComponentForDishes3);

		ComponentForDishes4 = CreateDefaultSubobject<USceneComponent>("DirtyPlate4");
		ComponentForDishes4->SetupAttachment(ComponentForCooking);
		DirtyPlateComponents.Add(ComponentForDishes4);
	}

	TimeEventComponent = CreateDefaultSubobject<UTimeEventComponent>(TEXT("TimeEventComponent"));
}

void ASinkTable::BeginPlay()
{
	Super::BeginPlay();

	InitProgressBar();

	SoundEffect = UOC2GlobalData::GetTableBaseSound(GetWorld(), "Washing");
}

void ASinkTable::InitProgressBar()
{
	// ���� Ŭ���� ����
	ProgressBarComponent->SetWidgetClass(TSubClassWidget); // WBP �������� ����
	UUserWidget* UserWidget = ProgressBarComponent->GetUserWidgetObject();
	if (nullptr != UserWidget)
	{
		WidgetPtr = Cast<UGaugeTextureWidget>(UserWidget);
		WidgetPtr->SetPosition(FVector2D{ 5.0f, 10.0f });
	}
	ProgressBarComponent->SetDrawAtDesiredSize(true);   // ������ ���� ũ��� ����
	ProgressBarComponent->SetPivot(FVector2D(0.5f, 0.5f)); // �߽� ����
	ProgressBarComponent->SetWidgetSpace(EWidgetSpace::Screen); // ���� �������� 3Dó�� ���̰�
	ProgressBarComponent->bHiddenInGame = true;

	// ī�޶� ���ϵ��� ����
	ProgressBarComponent->SetTwoSided(true);
	ProgressBarComponent->SetTickWhenOffscreen(true);
	ProgressBarComponent->SetWorldLocation(this->GetActorLocation());
}

ACooking* ASinkTable::Interact(AActor* ChefActor)
{
	// �������� ���ð� �ϳ� �̻� ������ ĳ���Ϳ��� �ϳ��� �� �� �ִ�.

	if (true == HasAuthority())
	{
		if (true == CleanPlates.IsEmpty())
		{
			return nullptr;
		}
		CleanPlates.Last()->BaseFromSelf();
		CookingPtr = CleanPlates.Pop();
	
		return CookingPtr;
	}

	return nullptr;
}

// ���ð� ��ũ��� ������ ����
void ASinkTable::PlaceItem(ACooking* ReceivedCooking)
{
	PlacePlates(ReceivedCooking);
}

// ���ð� ��ũ��� ������ ���� (������ �迭�� �����Ѵ�.)
void ASinkTable::PlacePlates_Implementation(ACooking* ReceivedCooking)
{
	if (false == HasAuthority())
	{
		return;
	}
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
			for (int32 i = 0; i < TempPlate->Plates.Num(); i++)
			{
				TempPlate->Plates[i]->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
				DirtyPlates.Add(TempPlate->Plates[i]);
			}

			TempPlate->Plates.Empty();
		}

		for (int32 i = 0; i < DirtyPlates.Num(); i++)
		{
			DirtyPlates[i]->AttachToComponent(DirtyPlateComponents[i], FAttachmentTransformRules::KeepRelativeTransform);
			DirtyPlates[i]->SetActorLocation(DirtyPlateComponents[i]->GetComponentLocation());
			DirtyPlates[i]->SetActorRotation(DirtyPlateComponents[i]->GetComponentRotation());
			DirtyPlates[i]->SetActorScale3D(PlateSize * 0.6);
		}
	}

}

void ASinkTable::DoTheDishes(AOC2Character* ChefActor)
{
	ChefPtr = Cast<AOC2Character>(ChefActor);

	// ������ ��ũ�븦 ������ �ʾҰ� �������� ���ð� �����ִٸ�
	if (nullptr != ChefActor && 0 < DirtyPlates.Num())
	{
		ChefPtr->Washing(true);

		Timer = 0.0f;
		bTimerActivated = true;

		HideProgressBar(false);
	}

	/*if (nullptr != SoundEffect)
	{
		UGameplayStatics::PlaySound2D(GetWorld(), SoundEffect);
	}*/
}

void ASinkTable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		// ������ ������ ��ȣ�ۿ��� �ߴ��ϰ� ��������
		CheckChefIsWashing();

		UpdateProgressBar(DeltaTime);

		WashingIsDone();

		RepeatWashing();
	}
}

void ASinkTable::CheckChefIsWashing_Implementation()
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

void ASinkTable::FinishWashing_Implementation()
{
	bWashingDone = true;
}

void ASinkTable::UpdateProgressBar/*_Implementation*/(float DeltaTime)
{
	if (true == bTimerActivated)
	{
		Timer += DeltaTime;

		if (Timer > 2.0f)
		{
			// Ʈ���� ����
			FinishWashing();
		}
	}

	Ratio = (Timer / 0.4f) * 0.2f;
	WidgetPtr->SetProgressTimeRatio(Ratio);
}

// ������ �Ϸ����� �� �� �� ȣ��, ��ũ��� ���ø� �ϳ� �̵���Ų��.
void ASinkTable::WashingIsDone_Implementation()
{
	if (false == HasAuthority())
	{
		return;
	}

	// ������ ���̶�� ����
	if (false == bWashingDone)
	{
		return;
	}

	bTimerActivated = false; // Ÿ�̸� ����

	if (false == DirtyPlates.IsEmpty())
	{
		CleanPlates.Add(DirtyPlates.Pop());
		APlate* NewCleanPlate = CleanPlates.Last();
		NewCleanPlate->SetPlateState(EPlateState::EMPTY);
	}

	int Result = CleanPlates.Num();

	HideProgressBar(true);

	CleanPlates.Last()->AttachToComponent(CleanPlateComponent, FAttachmentTransformRules::KeepRelativeTransform);
	CleanPlates.Last()->SetActorLocation(CleanPlateComponent->GetComponentLocation());
	CleanPlates.Last()->AddActorWorldOffset(FVector::UnitZ()* 10.0f * (CleanPlates.Num() - 1));
	CleanPlates.Last()->SetActorScale3D(PlateSize);

}

void ASinkTable::HideProgressBar_Implementation(bool Value)
{
	ProgressBarComponent->SetHiddenInGame(Value);
}

void ASinkTable::RepeatWashing_Implementation()
{
	// 1. �������� ������ �� ���� ������ �� ���ð� �����ִٸ� �ٽ� ������ ���� ȣ��
	if (true == bWashingDone && 0 < DirtyPlates.Num())
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

void ASinkTable::SetAttachToDirtyPlate()
{
	for (size_t i = 0; i < DirtyPlates.Num(); i++)
	{
		FVector Result = DirtyPlateComponents[i]->GetComponentLocation();
		//DirtyPlates[i]->SetActorRotation(DirtyPlateComponents[i]->GetComponentRotation());
		DirtyPlates[i]->AttachToComponent(DirtyPlateComponents[i], FAttachmentTransformRules::KeepRelativeTransform);
		DirtyPlates[i]->SetActorLocation(DirtyPlateComponents[i]->GetComponentLocation());
	}
}

void ASinkTable::OnRep_ChangeProgress()
{
	WidgetPtr->SetProgressTimeRatio(Ratio);
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
	DOREPLIFETIME(ASinkTable, CleanPlateComponent);
	DOREPLIFETIME(ASinkTable, DirtyPlateComponents);
	DOREPLIFETIME(ASinkTable, ProgressBarComponent);
}