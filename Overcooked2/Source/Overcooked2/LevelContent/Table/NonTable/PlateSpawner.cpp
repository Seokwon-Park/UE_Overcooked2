// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Table/NonTable/PlateSpawner.h"
#include "LevelContent/Cook/Plate.h"
#include <Net/UnrealNetwork.h>
#include <Global/OC2Global.h>
#include <Global/Data/OC2GlobalData.h>

APlateSpawner::APlateSpawner()
{
	bReplicates = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	PlateMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PlateMeshComponent"));
	PlateMeshComponent->SetupAttachment(RootComponent);
}

void APlateSpawner::BeginPlay()
{
	Super::BeginPlay();

	PlateMeshComponent->SetRelativeScale3D(FVector(2, 2, 2));
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

	// �����ʴ� Plate�� �������� �� ��, ������ ���� �ʴ´�. 
	// Stack ������ �ľ��ϰ� ������ ������.
	int PlateCount = Plate->GetPlateStackCount() + 1;
	AddPlate(PlateCount);

	MoveToServer(Plate);

	SetPlateMesh();
	
	CookingPtr = nullptr;

	//for (int i = 0; i < PlateMeshComponent->GetNumMaterials(); i++)
	//{
	//	// ���� ���̶���Ʈ
	//	UMaterialInterface* Material = PlateMeshComponent->GetMaterials()[i];
	//	if (nullptr != Material)
	//	{
	//		UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	//		if (nullptr != DynamicMaterial)
	//		{
	//			float Temp;
	//			PlateMeshComponent->GetMaterials()[i]->GetScalarParameterValue(FName("DiffuseAdd"), Temp);
	//			DiffuseColorMapWeights.Add(Temp);
	//			PlateMeshComponent->SetMaterial(i, DynamicMaterial);
	//		}
	//	}
	//}

	for (int i = 0; i < PlateMeshComponent->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(PlateMeshComponent->GetMaterial(i));
		if (nullptr != MaterialInstanceDynamic)
		{
			MaterialInstanceDynamic->SetTextureParameterValue(FName(TEXT("DiffuseColorMap")), DirtyTexture);
			MaterialInstanceDynamic->SetScalarParameterValue(FName("DiffuseAdd"), 1.0f);
			PlateMeshComponent->SetMaterial(i, MaterialInstanceDynamic);
			return;
		}
	}
}

ACooking* APlateSpawner::Interact(AActor* ChefActor)
{
	if (0 >= PlateNum)
	{
		return nullptr;
	}
	else
	{
		APlate* NewPlate = UOC2Global::GetPlate(GetWorld());
		if (nullptr != NewPlate)
		{
			NewPlate->RestorePlateToWorld(); // ���忡 ����, �浹 �Ѱ�
			NewPlate->SetPlateState(EPlateState::DIRTY); // ��Ƽ���� �ؽ�ó �ٲٰ�
			NewPlate->SetPlateStackCount(PlateNum - 1); // �޽� �ٲٰ�

			InitPlateNum(); // Plate 0�� �ʱ�ȭ
			SetPlateMesh(); // ������ ���� �޽� �����
			return NewPlate;
		}
	}

	return nullptr;
}

void APlateSpawner::PlaceItem(ACooking* ReceivedCooking)
{
	if (nullptr == ReceivedCooking)
	{
		return;
	}
	ACooking* TempCooking = ReceivedCooking;
}

void APlateSpawner::GetOwnedPlate_Implementation()
{
	if (nullptr != OwnedPlate) // ���� ���ø� �����Ѱ� ������
	{
		NewCooking = nullptr;
		NewCooking = Cast<ACooking>(OwnedPlate);
		if (nullptr != NewCooking)
		{
			// ���ø� ĳ���Ϳ��� �Ѱ��ְ� ���� ������ �ִ� ���ð� ����.
			OwnedPlate = nullptr;
			bOwnedPlate = true;
			return;
		}
	}

	bOwnedPlate = false;
}

void APlateSpawner::MoveToServer_Implementation(APlate* Plate)
{


	UOC2Global::MovePlate(GetWorld(), Plate);
}

void APlateSpawner::SetPlateMesh_Implementation()
{
	FVector InitPos = GetActorLocation() + FVector(0, 0, 50);
	switch (PlateNum)
	{
	case 0:
	{
		PlateMeshComponent->SetStaticMesh(nullptr);
		break;
	}
	case 1:
	{
		UStaticMesh* NewMesh = UOC2GlobalData::GetResourceStaticMesh(GetWorld(), TEXT("SinglePlate"));
		PlateMeshComponent->SetStaticMesh(NewMesh);

		PlateMeshComponent->SetRelativeLocation(InitPos + FVector(0, 0, 0));
		break;
	}
	case 2:
	{
		UStaticMesh* NewMesh = UOC2GlobalData::GetResourceStaticMesh(GetWorld(), TEXT("DoublePlate"));
		PlateMeshComponent->SetStaticMesh(NewMesh);

		PlateMeshComponent->SetRelativeLocation(InitPos + FVector(0, 0, 20));
		break;
	}
	case 3:
	{
		UStaticMesh* NewMesh = UOC2GlobalData::GetResourceStaticMesh(GetWorld(), TEXT("TriplePlate"));
		PlateMeshComponent->SetStaticMesh(NewMesh);

		PlateMeshComponent->SetRelativeLocation(InitPos + FVector(0, 0, 0));
		break;
	}
	case 4:
	{
		UStaticMesh* NewMesh = UOC2GlobalData::GetResourceStaticMesh(GetWorld(), TEXT("FullPlate"));
		PlateMeshComponent->SetStaticMesh(NewMesh);

		PlateMeshComponent->SetRelativeLocation(InitPos + FVector(0, 0, 60));
		break;
	}
	default:
		break;
	}
}

void APlateSpawner::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APlateSpawner, PlateMeshComponent);
	DOREPLIFETIME(APlateSpawner, PlateNum);
}

void APlateSpawner::AddPlate_Implementation(int Number)
{
	if (true == HasAuthority())
	{
		PlateNum += Number;
		if (0 > PlateNum)
		{
			PlateNum = 0;
		}
		if (4 < PlateNum)
		{
			PlateNum = 4;
		}
	}
}

void APlateSpawner::InitPlateNum_Implementation()
{
	PlateNum = 0;
}

