// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Cook/Plate.h"
#include <LevelContent/Cook/Ingredient.h>
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include <Global/Data/OC2GlobalData.h>
#include "Components/BillboardComponent.h"  
#include "Components/WidgetComponent.h"
#include <LevelContent/Cook/Widget/PlateIconWidget.h>
#include "LevelContent/Table/CookingTable.h"
#include "LevelContent/Table/NonTable/PlateSpawner.h"
#include "EngineUtils.h"

// Sets default values
APlate::APlate()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	CookingType = ECookingType::ECT_PLATE;

	IngredientMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("IngredientMesh"));


	FVector Scale = FVector(2.0f, 2.0f, 2.0f);
	StaticMeshComponent->SetRelativeScale3D(Scale);

	WidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("WidgetComponent"));
	WidgetComponent->SetupAttachment(RootComponent);

}

void APlate::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	IngredientMesh->SetIsReplicated(true); // ������Ʈ ��Ʈ��ũ ����ȭ
}

void APlate::Multicast_SubmitPlate_Implementation()
{
	SetActorLocation(UOC2Const::PlateSubmitLocation);
	CleanPlate();
	SetPlateState(EPlateState::DIRTY);

	FTimerHandle TimerHandle;

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		this,
		&APlate::SpawnPlate,
		3.0f,   // 3�� �� ����
		false   // �ݺ� ����(false�� 1ȸ ����)
	);
}

void APlate::SpawnPlate()
{
	if (nullptr != PlateSpawner)
	{
		PlateSpawner->PlaceItem(this);
		PlateSpawner->SetPlate(this);
	}
}

// Called when the game starts or when spawned
void APlate::BeginPlay()
{
	ACooking::BeginPlay();
	
	InitWidgetComponent();

	FindPlateSpawner();

	// Debug
}

void APlate::InitWidgetComponent()
{
	// ���� Ŭ���� ����
	WidgetComponent->SetWidgetClass(SubclassWidget); // WBP �������� ����
	UUserWidget* UserWidget = WidgetComponent->GetUserWidgetObject();
	if (nullptr != UserWidget)
	{
		IconWidget = Cast<UPlateIconWidget>(UserWidget);
	}

	WidgetComponent->SetDrawAtDesiredSize(true);   // ������ ���� ũ��� ����
	WidgetComponent->SetPivot(FVector2D(0.5f, 0.5f)); // �߽� ����
	WidgetComponent->SetWidgetSpace(EWidgetSpace::Screen); // ���� �������� 3Dó�� ���̰�
	WidgetComponent->bHiddenInGame = false;

	// ī�޶� ���ϵ��� ����
	WidgetComponent->SetTwoSided(true);
	WidgetComponent->SetTickWhenOffscreen(true);

	IconWidget->Init();
}

void APlate::FindPlateSpawner()
{
	// TActorIterator�� ����Ͽ� ���� �� ��� APrepTable ���͸� ��ȸ

	for (TActorIterator<ACookingTable> It(GetWorld()); It; ++It)
	{
		ACookingTable* PrepTableActor = *It;
		if (PrepTableActor->Tags.Contains("PlateSpawner"))
		{
			PlateSpawner = Cast<APlateSpawner>(PrepTableActor);
		}
	}
}

// Called every frame
void APlate::Tick(float DeltaTime)
{
	ACooking::Tick(DeltaTime);

}

bool APlate::IsDirtyPlate()
{
	if (nullptr == StaticMeshComponent)
	{
		return false;
	}

	return PlateState == EPlateState::DIRTY;
}

void APlate::WashPlate_Implementation()
{
	if (true == IsDirtyPlate())
	{
		PlateState = EPlateState::EMPTY;
		SetMesh();
	}
}

void APlate::SetPlateState_Implementation(EPlateState State)
{
	PlateState = State;
	SetMesh();
}

void APlate::CleanPlate_Implementation()
{
	PlateState = EPlateState::EMPTY;
	IngredientMesh->SetStaticMesh(nullptr);
	Ingredients.Empty();
	IconWidget->Reset();
	bIsCombinationSuccessful = false;
}

void APlate::SetMesh()
{
	UTexture* Texture = nullptr;
	if (EPlateState::DIRTY == PlateState)
	{
		Texture = DirtyTexture;
	}
	else
	{
		Texture = CleanTexture;
	}
	SetMaterialTexture(Texture);
}

void APlate::SetMaterialTexture(UTexture* Texture)
{
	// 1. ����ƽ �޽��� ��Ƽ������ �ٲܰǵ�

	// 2. �̹� �������� ������ ��Ƽ���� �ν��Ͻ� ���̳����� �����ϸ� 
	UMaterialInstanceDynamic* MaterialInstanceDynamic = Cast<UMaterialInstanceDynamic>(StaticMeshComponent->GetMaterial(0));
	if (nullptr != MaterialInstanceDynamic)
	{
		// 3. ���� ��Ƽ���� �ν��Ͻ� ���̳����� �״�� ����ϰ�
		MaterialInstanceDynamic->SetTextureParameterValue(FName(TEXT("DiffuseColorMap")), Texture);
		StaticMeshComponent->SetMaterial(0, MaterialInstanceDynamic);
		return;
	}

	// 4. ������ ������� ��Ƽ���� �ν��Ͻ� ���̳����� ���ٸ� == SetMaterial�� ó���̶��
	//UMaterialInterface* Material = StaticMeshComponent->GetMaterial(0);
	//if (nullptr != Material)
	//{
	//	// 5. ���� ������ ��Ƽ������ �Ⱦ��� ��Ƽ���� �ν��Ͻ� ���̳����� ���� �� ���̴�.
	//	UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(Material, this);
	//	if (nullptr != DynamicMaterial)
	//	{
	//		// 6. �ٲ� �ؽ�ó�� �����Ϳ��� ��������ٸ� �װɷ� �ٲ��.
	//		if (nullptr != Texture)
	//		{
	//			DynamicMaterial->SetTextureParameterValue(FName(TEXT("DiffuseAdd")), Texture);
	//			StaticMeshComponent->SetMaterial(0, DynamicMaterial);
	//		}
	//	}
	//}
}

void APlate::ForwardAttachToChef()
{
	FVector Offset = FVector(90, 0, -40);
	StaticMeshComponent->SetRelativeLocation(Offset);
}

void APlate::ForwardDetachToChef()
{
}

bool APlate::CanPlaceOnPlate(AIngredient* Ingredient)
{
	if (nullptr == Ingredient)
	{
		return false;
	}
	if (ECookingType::ECT_INGREDIENT != Ingredient->GetCookingType())
	{
		return false;
	}
	if (EPlateState::COMPLETED == PlateState || EPlateState::DIRTY == PlateState)
	{	// �̹� �ϼ��� �丮�� ��ô ���� ���ô� ��Ḧ �ø� �� ����.
		return false;
	}
	if (EIngredientState::EIS_NONE == Ingredient->GetCurIngredientState())
	{	// �������� ���� ���� ���ÿ� �ø� �� ����.
		return false;
	}
	if (nullptr == StaticMeshComponent)
	{
		return false;
	}
	return true;
}

void APlate::Add_Implementation(AIngredient* Ingredient)
{
	bIsCombinationSuccessful = false;

	if (false == CanPlaceOnPlate(Ingredient))
	{
		return;
	}

	// 1. ������ ��Ḧ �߰��Ѵ�.
	FRecipe Recipe;
	Recipe.IngredientType = Ingredient->GetIngredientType();
	Recipe.IngredientState = Ingredient->GetCurIngredientState();
	Ingredients.Add(Recipe);

	// 2. RecipeDataTable�� ���Ͽ� ������ ���̺� �ش� ��������� �����ϴ��� Ȯ��
	FPlateInitData InitData = UOC2GlobalData::GetPlateMesh(GetWorld(), Ingredients);

	// 3-1. �����͸� ȹ���ϴµ� �����ߴٸ�
	if (nullptr == InitData.StaticMesh) 
	{
		Ingredients.Pop(); // ��� �ڷᱸ������ �����ϰ� ����
		return;
	}
	else // 3-2. �����͸� ȹ���ϴµ� �����ߴٸ� 
	{
		IngredientMesh->SetStaticMesh(InitData.StaticMesh); // �޽� ����
		PlateState = EPlateState::OCCUPIED;
		if (nullptr != IngredientMesh)
		{
			// 3-3. ���� ���� �ö� �丮 �޽� ����
			SetIngredinetMesh(InitData);

			// 3-5. ������ �����ϴ� ���� ���忡�� ����
			Ingredient->RequestOC2ActorDestroy();

			// 4. Texture �߰�
			SetIngredinetTextures(InitData);
			return;
		}
	}
	return;
}

void APlate::SetIngredinetMesh(FPlateInitData Data)
{
	// 1. ���� ��� ����
	SetSimulatePhysics(false); // ������Ʈ�� �浹�� ���ư��� �����̴� ���� �����ϱ� ���� ������ ��� ����.

	// 2. IngredientMesh�� �浹ü�� ������ ����.
	IngredientMesh->AttachToComponent(StaticMeshComponent, FAttachmentTransformRules::KeepRelativeTransform);
	IngredientMesh->SetCollisionProfileName(TEXT("NoCollision"));
	IngredientMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	IngredientMesh->SetSimulatePhysics(false);

	// 3. Offset
	IngredientMesh->SetRelativeLocation(Data.OffsetLocation);
	IngredientMesh->SetRelativeRotation(Data.OffsetRotation);
	IngredientMesh->SetRelativeScale3D(Data.OffsetScale);

	// 4. ���� �ٽ� �Ѱ�
	SetSimulatePhysics(true);
	bIsCombinationSuccessful = true;
}

void APlate::SetIngredinetTextures(FPlateInitData Data)
{
	TArray<UTexture2D*> Textures;
	for (size_t i = 0; i < Data.IngredientTextures.Num(); i++)
	{
		UTexture2D* Texture = Cast<UTexture2D>(Data.IngredientTextures[i]);
		if (nullptr != Texture)
		{
			Textures.Add(Texture);
		}
	}
	IconWidget->SetIngredientTextures(Textures);

}

void APlate::StackPlate_Implementation(APlate* Plate)
{
	if (PlateState == EPlateState::EMPTY || PlateState == EPlateState::DIRTY)
	{
		if (PlateState == Plate->PlateState) // ������ ������ �༮�� ���� �� �ִ�.
		{
			AnotherPlates.Add(Plate);
			
			// �̹� �׿��ִ� ���ø� ��ġ�� �Ŷ��
			// Debug �ʿ�. Plate->AnotherPlates[i]->AnotherPlates�� �׻� 0�̶�� Ȯ���� �� ������?
			if (false == Plate->AnotherPlates.IsEmpty())
			{
				for (size_t i = 0; i < Plate->AnotherPlates.Num(); i++)
				{
					AnotherPlates.Add(Plate->AnotherPlates[i]);
				}
				Plate->AnotherPlates.Empty();
			}

			ChangePlateMesh();
		}
	}
}

void APlate::ChangePlateMesh()
{
	int Count = AnotherPlates.Num();

	HideAnotherPlates();

	switch (Count)
	{
	case 0:
	{
		StackUpPlate(EPlateStackStatus::SINGLE, TEXT("SinglePlate"));
		break;
	}
	case 1:
	{
		StackUpPlate(EPlateStackStatus::DOUBLE, TEXT("DoublePlate"));
		break;
	}
	case 2:
	{
		StackUpPlate(EPlateStackStatus::TRIPLE, TEXT("TriplePlate"));
		break;
	}
	case 3:
	{
		StackUpPlate(EPlateStackStatus::FULL, TEXT("FullPlate"));
		break;
	}
	default:
		break;
	}
}

void APlate::StackUpPlate(EPlateStackStatus Status, FName Name)
{
	PlateStackStatus = Status;

	UStaticMesh* NewStaticMesh = UOC2GlobalData::GetResourceStaticMesh(GetWorld(), Name);
	StaticMeshComponent->SetStaticMesh(NewStaticMesh);
}

APlate* APlate::TakeCleanPlate()
{
	int32 Count = AnotherPlates.Num();
	if (0 == Count) // ���� ������ �ִ� �ٸ� ���ð� ���ٸ� �� �����͸� �Ѱ��ش�.
	{
		return this;
	}

	// �������� ���ø� ã�Ƽ� �ϳ� �����ش�.
	for (size_t i = 0; i < AnotherPlates.Num(); i++)
	{
		if (EPlateState::EMPTY == AnotherPlates[i]->PlateState)
		{
			// ���� �� ���� �ٽ� ���� ������ �����Ѵ�.
			AnotherPlates[i]->SetActorHiddenInGame(false);
			AnotherPlates[i]->SetActorEnableCollision(true);
			AnotherPlates[i]->SetActorTickEnabled(true);

			APlate* AnotherPlate = AnotherPlates[i];
			AnotherPlates.RemoveAt(i);
			return AnotherPlate;
		}
	}

	return nullptr; // ������ �༮�� ����.
	// ���� �༮�� ���ٸ� this�� ��� �ұ�?
}

void APlate::HideAnotherPlates()
{
	// ���� �� �����.
	for (int i = 0; i < AnotherPlates.Num(); i++)
	{
		AnotherPlates[i]->SetActorHiddenInGame(true);		// ���� ����
		AnotherPlates[i]->SetActorEnableCollision(false);	// �浹 ����
		AnotherPlates[i]->SetActorTickEnabled(false);		// Tick ����
	}
}

void APlate::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlate, Ingredients);
	DOREPLIFETIME(APlate, IngredientMesh);
	DOREPLIFETIME(APlate, PlateState);
	DOREPLIFETIME(APlate, bIsCombinationSuccessful);
	DOREPLIFETIME(APlate, AnotherPlates);
	DOREPLIFETIME(APlate, PlateStackStatus);
}