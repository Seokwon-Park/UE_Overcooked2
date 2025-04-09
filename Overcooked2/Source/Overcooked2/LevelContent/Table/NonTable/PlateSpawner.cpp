// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Table/NonTable/PlateSpawner.h"
#include "LevelContent/Cook/Plate.h"

APlateSpawner::APlateSpawner()
{

}

void APlateSpawner::BeginPlay()
{
	Super::BeginPlay();
}

void APlateSpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APlateSpawner::SetPlate(class APlate* Plate)
{
	CookingPtr = Cast<APlate>(Plate);
	CookingPtr->SetCookingTable_Implementation(this);

	if (nullptr == Plate)
	{
		return;
	}

	if (nullptr == OwnedPlate) // 
	{
		OwnedPlate = Plate;
		OwnedPlate->AttachToComponent(ComponentForCooking, FAttachmentTransformRules::KeepRelativeTransform);
		OwnedPlate->SetActorLocation(ComponentForCooking->GetComponentLocation());
		OwnedPlate->AddActorLocalOffset(FVector(0, 0, 0));
	}
	else
	{
		OwnedPlate->StackPlate(Plate);
		if (1 == OwnedPlate->GetAnotherPlatesRef().Num())
		{
			OwnedPlate->AddActorLocalOffset(FVector(0, 0, 10));
		}
		if (3 == OwnedPlate->GetAnotherPlatesRef().Num())
		{
			OwnedPlate->AddActorLocalOffset(FVector(0, 0, 50));
		}
	}
	//PlateNum++;
	//PlateMap.Add(PlateNum, CookingPtr);
	//PlateMap[PlateNum]->SetActorLocation(ComponentForCooking->GetComponentLocation());
	//PlateMap[PlateNum]->AddActorLocalOffset(FVector::UnitZ() * 30.0f * (PlateNum -1));

	CookingPtr = nullptr;
}

ACooking* APlateSpawner::Interact(AActor* ChefActor)
{
	if (nullptr != OwnedPlate) // ���� ���ø� �����Ѱ� ������
	{
		ACooking* NewCooking = Cast<ACooking>(OwnedPlate);
		if (nullptr != NewCooking)
		{
			// ���ø� ĳ���Ϳ��� �Ѱ��ְ� ���� ������ �ִ� ���ð� ����.
			OwnedPlate = nullptr; 
			return NewCooking;
		}
	}

	// ���� ������ �ִ� ���ð� ���ٸ� �� ��ȯ
	return nullptr;

	//if (PlateNum > 0)
	//{
	//	CookingPtr = PlateMap[PlateNum];
	//	PlateMap.Remove(PlateNum);
	//	PlateNum--;
	//}
	//ACooking* CookingReturn = CookingPtr;
	//CookingPtr = nullptr;
	//return CookingReturn;
}

void APlateSpawner::PlaceItem(ACooking* ReceivedCooking)
{
	ACooking* TempCooking = ReceivedCooking;
}