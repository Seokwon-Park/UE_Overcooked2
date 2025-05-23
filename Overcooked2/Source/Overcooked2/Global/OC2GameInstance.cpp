// Fill out your copyright notice in the Description page of Project Settings.


#include "Global/OC2GameInstance.h"
#include "Overcooked2.h"

#include "Global/Data/RecipeDataTable.h"
#include "Global/Data/IngredientDataTable.h"
#include "Global/Data/OrderDataTable.h"

#include "Global/GameMode/CookingGameMode.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"

#include "LevelContent/Cook/Cooking.h"
#include "LevelContent/Cook/Plate.h"

UOC2GameInstance::UOC2GameInstance()
{
	ChefHeadNames.Add(UOC2Const::ChefEagleHeadName);
	ChefHeadNames.Add(UOC2Const::ChefMouseHeadName);
	ChefHeadNames.Add(UOC2Const::ChefPandaHeadName);
	ChefHeadNames.Add(UOC2Const::ChefPigHeadName);
	ChefHeadNames.Add(UOC2Const::ChefOwlHeadName);
}

UOC2GameInstance::~UOC2GameInstance()
{
}

void UOC2GameInstance::CreateRoom()
{
	FString LobbyLevelName = LobbyLevel.GetLongPackageName();
	FString OpenLevelName = FString::Printf(TEXT(":%s%s"), *Port, *LobbyLevelName);

	if (false == LobbyLevelName.IsEmpty())
	{
		UGameplayStatics::OpenLevel(GetWorld(), *OpenLevelName, true, TEXT("listen"));
	}
	else
	{
		UE_LOG(OVERCOOKED_LOG, Warning, TEXT("Lobby Level not setting"));
	}
}

void UOC2GameInstance::JoinRoom(FString IP, APlayerController* Controller)
{
	FString OpenLevelName = LobbyLevel.GetLongPackageName();
	FString ConnectLevelName = FString::Printf(TEXT("%s:%s%s"), *IP, *Port, *OpenLevelName);

	if (Controller != nullptr)
	{
		Controller->ClientTravel(ConnectLevelName, ETravelType::TRAVEL_Absolute);
	}
}

void UOC2GameInstance::StartGame()
{
	FString OpenLevelPath = WorldLevel.GetLongPackageName();

	GetWorld()->ServerTravel(OpenLevelPath + TEXT("?listen"));
}

void UOC2GameInstance::StartCookingStage()
{
	FString OpenLevelPath = CookingLevel.GetLongPackageName();

	if (false == GetWorld()->ServerTravel(OpenLevelPath + TEXT("?listen")))
	{
		UE_LOG(OVERCOOKED_LOG, Fatal, TEXT("Server travel failed!!!!"));
	}
}

EIngredientType UOC2GameInstance::GetIngredientType(const FString& RowName)
{
	if (nullptr != IngredientDataTable)
	{

		FIngredientDataRow* IngredientData = IngredientDataTable->FindRow<FIngredientDataRow>(*RowName, nullptr);

		if (nullptr != IngredientData)
		{
			return IngredientData->IngredientType;
		}
	}

	return EIngredientType::EIT_NONE;
}

const TArray<FIngredientCookDataRow>& UOC2GameInstance::GetIngredientCookDataRows(const FString& RowName)
{
	static const TArray<FIngredientCookDataRow> EmptyArray;

	if (nullptr != IngredientDataTable)
	{
		FIngredientDataRow* IngredientData = IngredientDataTable->FindRow<FIngredientDataRow>(*RowName, nullptr);

		if (nullptr != IngredientData)
		{
			return IngredientData->StateRows;
		}
	}

	return EmptyArray;
}

UStaticMesh* UOC2GameInstance::GetIngredientStaticMesh(const FString& RowName)
{
	if (nullptr != IngredientDataTable)
	{
		FIngredientDataRow* IngredientData = IngredientDataTable->FindRow<FIngredientDataRow>(*RowName, nullptr);

		if (nullptr != IngredientData)
		{
			return IngredientData->BaseMesh;
		}
	}

	return nullptr;
}

FName UOC2GameInstance::GetIngredientDataTableRowName(EIngredientType IngredientType)
{
	switch (IngredientType)
	{
	case EIngredientType::EIT_FISH:
		return TEXT("Fish");
	//case EIngredientType::EIT_CABBAGE:
	//	return TEXT("Cabbage");
	case EIngredientType::EIT_PRAWN:
		return TEXT("Prawn");
	//case EIngredientType::EIT_TOMATO:
	//	return TEXT("Tomato");
	case EIngredientType::EIT_SEAWEED:
		return TEXT("Seaweed");
	case EIngredientType::EIT_RICE:
		return TEXT("Rice");
	case EIngredientType::EIT_CUCUMBER:
		return TEXT("Cucumber");
	default:
		return TEXT("");
	}
}

const FIngredientDataRow& UOC2GameInstance::GetIngredientDataRow(const FName& RowName)
{
	static FIngredientDataRow EmptyData;

	if (nullptr != IngredientDataTable)
	{
		FIngredientDataRow* IngredientData = IngredientDataTable->FindRow<FIngredientDataRow>(RowName, nullptr);

		if (nullptr != IngredientData)
		{
			return *IngredientData;
		}
	}

	return EmptyData;
}

const FIngredientDataRow& UOC2GameInstance::GetIngredientDataRow(EIngredientType FindIngredientType)
{
	static FIngredientDataRow EmptyData;

	// 데이터 테이블의 모든 행의 이름을 가져오기
	TArray<FName> RowNames = IngredientDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		// 현재 행을 가져오기
		FIngredientDataRow* IngredientData = IngredientDataTable->FindRow<FIngredientDataRow>(RowName, nullptr);

		if (nullptr != IngredientData && IngredientData->IngredientType == FindIngredientType)
		{
			return *IngredientData; // 원하는 값이 있는 행을 찾으면 반환
		}
	}

	return EmptyData;
}

const FResourceTextureDataRow& UOC2GameInstance::GetResourceTextureDataRow(const FName& RowName)
{
	static FResourceTextureDataRow EmptyData;

	if (nullptr != ResourceTextureDataTable)
	{
		FResourceTextureDataRow* ResourceTextureData = ResourceTextureDataTable->FindRow<FResourceTextureDataRow>(RowName, nullptr);

		if (nullptr != ResourceTextureData)
		{
			return *ResourceTextureData;
		}
	}

	return EmptyData;
}

const FResourceNiagaraDataRow& UOC2GameInstance::GetResourceNiagaraDataRow(const FName& RowName)
{
	static FResourceNiagaraDataRow EmptyData;

	if (nullptr != ResourceNiagaraDataTable)
	{
		FResourceNiagaraDataRow* ResourceNiagaraDataRow = ResourceNiagaraDataTable->FindRow<FResourceNiagaraDataRow>(RowName, nullptr);

		if (nullptr != ResourceNiagaraDataRow)
		{
			return *ResourceNiagaraDataRow;
		}
	}

	return EmptyData;
}

const FResourceStaticMeshDataRow& UOC2GameInstance::GetResourceStaticMeshDataRow(const FName& RowName)
{
	static FResourceStaticMeshDataRow EmptyData;

	if (nullptr != ResourceStaticMeshDataTable)
	{
		FResourceStaticMeshDataRow* ResourceStaticMeshDataRow = ResourceStaticMeshDataTable->FindRow<FResourceStaticMeshDataRow>(RowName, nullptr);

		if (nullptr != ResourceStaticMeshDataRow)
		{
			return *ResourceStaticMeshDataRow;
		}
	}

	return EmptyData;
}

const FResourceMaterialDataRow& UOC2GameInstance::GetResourceMaterialDataRow(const FName& RowName)
{
	static FResourceMaterialDataRow EmptyData;

	if (nullptr != ResourceMaterialDataTable)
	{
		FResourceMaterialDataRow* ResourceMaterialDataRow = ResourceMaterialDataTable->FindRow<FResourceMaterialDataRow>(RowName, nullptr);

		if (nullptr != ResourceMaterialDataRow)
		{
			return *ResourceMaterialDataRow;
		}
	}

	return EmptyData;
}

const FResourceSoundWaveDataRow& UOC2GameInstance::GetResourceSoundWaveDataRow(const FName& RowName)
{
	static FResourceSoundWaveDataRow EmptyData;

	if (nullptr != ResourceSoundWaveDataTable)
	{
		FResourceSoundWaveDataRow* ResourceSoundWaveDataRow = ResourceSoundWaveDataTable->FindRow< FResourceSoundWaveDataRow>(RowName, nullptr);

		if (nullptr != ResourceSoundWaveDataRow)
		{
			return *ResourceSoundWaveDataRow;
		}
	}

	return EmptyData;
}

const FResourceCookingSoundWaveDataRow& UOC2GameInstance::GetResourceCookingSoundWaveDataRow(const FName& RowName)
{
	static FResourceCookingSoundWaveDataRow EmptyData;

	if (nullptr != ResourceCookingSoundWaveDataTable)
	{
		FResourceCookingSoundWaveDataRow* ResourceSoundWaveDataRow = ResourceCookingSoundWaveDataTable->FindRow<FResourceCookingSoundWaveDataRow>(RowName, nullptr);

		if (nullptr != ResourceSoundWaveDataRow)
		{
			return *ResourceSoundWaveDataRow;
		}
	}

	return EmptyData;
}

const FResourceCharacterSoundWaveDataRow& UOC2GameInstance::GetResourceCharacterSoundWaveDataRow(const FName& RowName)
{
	static FResourceCharacterSoundWaveDataRow EmptyData;

	if (nullptr != ResourceCharacterSoundWaveDataTable)
	{
		FResourceCharacterSoundWaveDataRow* ResourceSoundWaveDataRow = ResourceCharacterSoundWaveDataTable->FindRow<FResourceCharacterSoundWaveDataRow>(RowName, nullptr);

		if (nullptr != ResourceSoundWaveDataRow)
		{
			return *ResourceSoundWaveDataRow;
		}
	}

	return EmptyData;
}

const FResourceTableSoundWaveDataRow& UOC2GameInstance::GetResourceTableSoundWaveDataRow(const FName& RowName)
{
	static FResourceTableSoundWaveDataRow EmptyData;

	if (nullptr != ResourceTableSoundWaveDataTable)
	{
		FResourceTableSoundWaveDataRow* ResourceSoundWaveDataRow = ResourceTableSoundWaveDataTable->FindRow<FResourceTableSoundWaveDataRow>(RowName, nullptr);

		if (nullptr != ResourceSoundWaveDataRow)
		{
			return *ResourceSoundWaveDataRow;
		}
	}

	return EmptyData;
}

const FResourceUISoundWaveDataRow& UOC2GameInstance::GetResourceUISoundWaveDataRow(const FName& RowName)
{
	static FResourceUISoundWaveDataRow EmptyData;

	if (nullptr != ResourceUISoundWaveDataTable)
	{
		FResourceUISoundWaveDataRow* ResourceSoundWaveDataRow = ResourceUISoundWaveDataTable->FindRow<FResourceUISoundWaveDataRow>(RowName, nullptr);

		if (nullptr != ResourceSoundWaveDataRow)
		{
			return *ResourceSoundWaveDataRow;
		}
	}

	return EmptyData;
}

const FResourceWorldSoundWaveDataRow& UOC2GameInstance::GetResourceWorldSoundWaveDataRow(const FName& RowName)
{
	static FResourceWorldSoundWaveDataRow EmptyData;

	if (nullptr != ResourceWorldSoundWaveDataTable)
	{
		FResourceWorldSoundWaveDataRow* ResourceSoundWaveDataRow = ResourceWorldSoundWaveDataTable->FindRow<FResourceWorldSoundWaveDataRow>(RowName, nullptr);

		if (nullptr != ResourceSoundWaveDataRow)
		{
			return *ResourceSoundWaveDataRow;
		}
	}

	return EmptyData;
}

FPlateInitData UOC2GameInstance::GetPlateMesh(TArray<FRecipe>& Recipes)
{
	static FPlateInitData EmptyArray;
	
	TArray<FName> RowNames = RecipeDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		// 현재 행을 가져오기
		FRecipeDataRow* RecipeData = RecipeDataTable->FindRow<FRecipeDataRow>(RowName, nullptr);

		if (RecipeData->RequireIngredients.Num() == Recipes.Num()
			&& true == FindRecipe(RecipeData, Recipes))
		{
			FPlateInitData PlateInitData;
			PlateInitData.StaticMesh = RecipeData->FoodMesh;
			PlateInitData.OffsetLocation = RecipeData->OffsetLocation;
			PlateInitData.OffsetRotation = RecipeData->OffsetRotation;
			PlateInitData.OffsetScale = RecipeData->OffsetScale;

			for (int j = 0; j < RecipeData->RequireIngredients.Num(); j++)
			{
				PlateInitData.IngredientTextures.Push(RecipeData->RequireIngredients[j].IngredientTexture);
			}

			return PlateInitData;
		}
	}

	return EmptyArray;
}

bool UOC2GameInstance::FindRecipe(const FRecipeDataRow* RecipeDataRow, TArray<FRecipe>& Recipes)
{
	if (nullptr == RecipeDataRow || 0 == RecipeDataRow->RequireIngredients.Num()|| 0 == Recipes.Num())
	{
		return false;
	}

	for (int i = 0; i < RecipeDataRow->RequireIngredients.Num(); i++)
	{
		EIngredientType IngredientType = RecipeDataRow->RequireIngredients[i].IngredientType;
		EIngredientState IngredientState = RecipeDataRow->RequireIngredients[i].IngredientState;

		bool bFound = false;

		for (int j = 0; j < Recipes.Num(); j++)
		{
			if (Recipes[j].IngredientType == IngredientType && Recipes[j].IngredientState == IngredientState)
			{
				bFound = true;
				break;
			}
		}

		if (false == bFound)
		{
			return false;
		}
	}

	return true;
}

bool UOC2GameInstance::CheckRecipe(TArray<FRecipe>& Recipes)
{
	TArray<FName> RowNames = RecipeDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		// 현재 행을 가져오기
		FRecipeDataRow* RecipeData = RecipeDataTable->FindRow<FRecipeDataRow>(RowName, nullptr);

		if (RecipeData->RequireIngredients.Num() == Recipes.Num()
			&& true == FindRecipe(RecipeData, Recipes))
		{
			
			return true;
		}
	}

	return false;
}

FOrder UOC2GameInstance::GetOrderByStageAndIndex(EOC2Stage OC2Stage, int Index)
{
	FOrder Result;

	if (nullptr != OrderDataTableMap.Find(OC2Stage))
	{
		UDataTable** FoundTable = OrderDataTableMap.Find(OC2Stage);

		UDataTable* CurOrderDataTable = *FoundTable;

		TArray<FName> RowNames = CurOrderDataTable->GetRowNames();

		if (false == RowNames.IsValidIndex(Index))
		{
			UE_LOG(OVERCOOKED_LOG, Error, TEXT("Order return is failed data table row name is undefined"));
			return Result;
		}

		FName RowName = RowNames[Index];

		FOrderDataRow* OrderData = CurOrderDataTable->FindRow<FOrderDataRow>(RowName, nullptr);

		if (nullptr == OrderData)
		{
			UE_LOG(OVERCOOKED_LOG, Error, TEXT("Order data cast is failed check struct type"));
		}
		else
		{
			Result.OrderTexutre = OrderData->OrderTexutre;
			Result.RequireIngredients = OrderData->RequireIngredients;
		}
	}

	return Result;
}

FOrder UOC2GameInstance::GetOrderByRecipes(ACooking* Cooking)
{
	APlate* Plate = Cast<APlate>(Cooking);

	TArray<FRecipe> Recipes;

	for (int i = 0; i < Plate->GetIngredients().Num(); i++)
	{
		FRecipe Recipe;

		Recipe.IngredientState = Plate->GetIngredient(i).IngredientState;
		Recipe.IngredientType = Plate->GetIngredient(i).IngredientType;

		Recipes.Add(Recipe);
	}

	FOrder Order;

	TArray<FName> RowNames = RecipeDataTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		// 현재 행을 가져오기
		FRecipeDataRow* RecipeData = RecipeDataTable->FindRow<FRecipeDataRow>(RowName, nullptr);

		if (RecipeData->RequireIngredients.Num() == Recipes.Num()
			&& true == FindRecipe(RecipeData, Recipes))
		{
			Order.OrderTexutre = RecipeData->OrderTexutre;
			Order.RequireIngredients = RecipeData->RequireIngredients;
		}
	}

	return Order;
}

UTexture2D* UOC2GameInstance::GetChefTexture()
{
	FString ChefRowName = UOC2Const::ChefTextureName + FString::FromInt(UserIndex);
	FName RowName = FName(*ChefRowName);

	if (nullptr != ResourceTextureDataTable)
	{
		FResourceTextureDataRow* ResourceTextureData = ResourceTextureDataTable->FindRow<FResourceTextureDataRow>(RowName, nullptr);

		if (nullptr != ResourceTextureData)
		{
			return ResourceTextureData->TextureRes;
		}
	}

	return nullptr;
}

UTexture2D* UOC2GameInstance::GetChefTextureByIndex(int Index)
{
	FString ChefRowName = UOC2Const::ChefTextureName + FString::FromInt(Index);
	FName RowName = FName(*ChefRowName);

	if (nullptr != ResourceTextureDataTable)
	{
		FResourceTextureDataRow* ResourceTextureData = ResourceTextureDataTable->FindRow<FResourceTextureDataRow>(RowName, nullptr);

		if (nullptr != ResourceTextureData)
		{
			return ResourceTextureData->TextureRes;
		}
	}

	return nullptr;
}

bool UOC2GameInstance::CompareOrderWithRecipe(TArray<FRecipe>& Recipes, EOC2Stage OC2Stage)
{
	UDataTable* CheckOrderTable = OrderDataTableMap[OC2Stage];

	TArray<FName> RowNames = CheckOrderTable->GetRowNames();

	for (const FName& RowName : RowNames)
	{
		// 현재 행을 가져오기
		FOrderDataRow* OrderData = CheckOrderTable->FindRow<FOrderDataRow>(RowName, nullptr);

		if (OrderData->RequireIngredients.Num() == Recipes.Num()
			&& true == CompareOrder(OrderData, Recipes))
		{

			return true;
		}
	}

	return false;
}

bool UOC2GameInstance::CompareOrder(const FOrderDataRow* OrderData, TArray<FRecipe>& Recipes)
{
	if (nullptr == OrderData || 0 == OrderData->RequireIngredients.Num() || 0 == Recipes.Num())
	{
		return false;
	}

	for (int i = 0; i < OrderData->RequireIngredients.Num(); i++)
	{
		EIngredientType IngredientType = OrderData->RequireIngredients[i].IngredientType;
		EIngredientState IngredientState = OrderData->RequireIngredients[i].IngredientState;

		bool bFound = false;

		for (int j = 0; j < Recipes.Num(); j++)
		{
			if (Recipes[j].IngredientType == IngredientType && Recipes[j].IngredientState == IngredientState)
			{
				bFound = true;
				break;
			}
		}

		if (false == bFound)
		{
			return false;
		}
	}

	return true;
}

FString UOC2GameInstance::GetChefHeadName() const
{
	return ChefHeadName;
}

void UOC2GameInstance::SetChefHeadName(FString HeadName)
{
	ChefHeadName = HeadName;
}

EOC2Stage UOC2GameInstance::GetCurStage() const
{
	return CurStage;
}

void UOC2GameInstance::SetCurStage(EOC2Stage ChangeStage)
{
	CurStage = ChangeStage;
}

int UOC2GameInstance::GetUserIndex() const
{
	return UserIndex;
}

void UOC2GameInstance::SetUserIndex(int InUserIndex)
{
	UserIndex = InUserIndex;
}

int UOC2GameInstance::GetUserCount() const
{
	return CurUserCount;
}

void UOC2GameInstance::SetUserCount(int UserCount)
{
	CurUserCount = UserCount;
}
