// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NiagaraSystem.h"

#include "Global/OC2Struct.h"
#include "Global/OC2Enum.h"

#include "Global/Data/IngredientDataTable.h"
#include "Global/Data/OrderDataTable.h"
#include "Global/Data/ResourceTextureTable.h"
#include "Global/Data/ResourceNiagaraTable.h"
#include "Global/Data/ResourceStaticMeshTable.h"
#include "Global/Data/ResourceMaterialTable.h"

#include "OC2GlobalData.generated.h"

class AIngredient;
class ACooking;

/**
 * 
 */
UCLASS()
class OVERCOOKED2_API UOC2GlobalData : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * ����� ������ �����ɴϴ�.
	 *
	 * @param RowName ������ ���̺��� ã�� ���� �̸��Դϴ�.
	 * @return �ش� ����� ����(EIngredientType)�Դϴ�.
	 */
	UFUNCTION(BlueprintCallable)
	static EIngredientType GetIngredientType(UWorld* World, const FName& RowName);
	/**
	 * Ư�� ����� ���� �����͸� �����ɴϴ�.
	 *
	 * @param RowName ������ ���̺��� ã�� ���� �̸��Դϴ�.
	 * @return �ش� ����� ���� ������ �迭(TArray<FIngredientCookDataRow>)�Դϴ�.
	 */
	UFUNCTION(BlueprintCallable)
	static const TArray<FIngredientCookDataRow>& GetIngredientCookDataRows(UWorld* World, const FName& RowName);
	/**
	 * Ư�� ����� ����ƽ �޽ø� �����ɴϴ�.
	 *
	 * @param RowName ������ ���̺��� ã�� ���� �̸��Դϴ�.
	 * @return �ش� ����� ����ƽ �޽�(UStaticMesh*)�Դϴ�.
	 */
	UFUNCTION(BlueprintCallable)
	static UStaticMesh* GetIngredientStaticMesh(UWorld* World, const FName& RowName);
	/**
	 * �־��� ��� Ÿ��(EIngredientType)�� �ش��ϴ� ������ ���̺� �� �̸��� ��ȯ�մϴ�.
	 *
	 * @param IngredientType ��ȸ�� ����� Ÿ���Դϴ�.
	 * @return FName �ش� ����� ������ ���̺� �� �̸��� ��ȯ�մϴ�. ��ȿ���� ���� Ÿ���� ��� �� FName("")�� ��ȯ�մϴ�.
	 */
	UFUNCTION(BlueprintCallable, Category = "Global|Data")
	static FName GetIngredientDataTableRowName(UWorld* World, EIngredientType IngredientType);
	/**
	 * Ư�� ����� ������ ���̺� �� �������� �ּҰ��� �����ɴϴ�.
	 *
	 * @param World ���� ���� ��ü�Դϴ�.
	 * @return FIngredientDataRow �ش� ����� ������ ���̺� �� �������Դϴ�.
	 */
	UFUNCTION(BlueprintCallable, Category = "Global|Data")
	static const FIngredientDataRow& GetIngredientDataRow(UWorld* World, const FName& RowName);
	/**
	 * Ư�� ��� Ÿ���� ������ ���̺� �� �������� �ּҰ��� �����ɴϴ�.
	 *
	 * @param World ���� ���� ��ü�Դϴ�.
	 * @param IngredientType ��ȸ�� ����� Ÿ���Դϴ�.
	 * @return FIngredientDataRow �ش� ��� Ÿ���� ������ ���̺� �� �������Դϴ�.
	 */
	static const FIngredientDataRow& GetIngredientDataRow(UWorld* World, EIngredientType IngredientType);

	/**
	 * �־��� ������ ����� ������� ������ �ʱ�ȭ �����͸� �����ɴϴ�.
	 *
	 * @param World ���� ���� ��ü�Դϴ�.
	 * @param Recipes ���ÿ� ��ġ�� ������ ����Դϴ�.
	 * @return FPlateInitData ������ �ʱ�ȭ �������Դϴ�.
	 */
	static FPlateInitData GetPlateMesh(UWorld* World, TArray<FRecipe>& Recipes);

	/**
	 * �־��� ��� ����� ������� ������ �ʱ�ȭ �����͸� �����ɴϴ�.
	 *
	 * @param World ���� ���� ��ü�Դϴ�.
	 * @param Ingredients ���ÿ� ��ġ�� ��� ��ü ����Դϴ�.
	 * @return FPlateInitData ������ �ʱ�ȭ �������Դϴ�.
	 */
	static FPlateInitData GetPlateMesh(UWorld* World, const TArray<class AIngredient*>& Ingredients);

	/**
	 * Ư�� ���������� �ε����� �ش��ϴ� �ֹ� �����͸� �����ɴϴ�.
	 *
	 * @param World ���� ���� ��ü�Դϴ�.
	 * @param CurStage ���� ���� ���� ���������Դϴ�.
	 * @param Index �ش� �������� ������ �ֹ��� �ε����Դϴ�.
	 * @return FOrder �ش��ϴ� �ֹ� �������Դϴ�.
	 */
	static FOrder GetOrderByStageAndIndex(UWorld* World, EOC2Stage CurStage, int Index);

	/**
	 * Ư�� ���ҽ� �ؽ�ó�� ������ ���̺� �� �������� �ּҰ��� �����ɴϴ�.
	 *
	 * @param World ���� ���� ��ü�Դϴ�.
	 * @param RowName ��ȸ�� ���ҽ� �ؽ�ó�� ������ ���̺� �� �̸��Դϴ�.
	 * @return FResourceTextureDataRow �ش� ���ҽ� �ؽ�ó�� ������ ���̺� �� �������Դϴ�.
	 */
	static const FResourceTextureDataRow& GetResourceTextureDataRow(UWorld* World, const FName& RowName);

	/**
	 * Ư�� ���ҽ� �ؽ�ó�� �����ɴϴ�.
	 *
	 * @param World ���� ���� ��ü�Դϴ�.
	 * @param RowName ��ȸ�� ���ҽ� �ؽ�ó�� ������ ���̺� �� �̸��Դϴ�.
	 * @return UTexture* �ش��ϴ� ���ҽ� �ؽ�ó�Դϴ�.
	 */
	static UTexture* GetResourceTexture(UWorld* World, const FName& RowName);

public:
	static const FResourceNiagaraDataRow& GetResourceNiagaraDataRow(UWorld* World, const FName& RowName);
	static UNiagaraSystem* GetResourceNiagaraSystem(UWorld* World, const FName& RowName);
	static FVector GetResourceNiagaraLocation(UWorld* World, const FName& RowName);
	static FRotator GetResourceNiagaraRotator(UWorld* World, const FName& RowName);
	static FVector GetResourceNiagaraScale(UWorld* World, const FName& RowName);

public:
	static UStaticMesh* GetResourceStaticMesh(UWorld* World, const FName& RowName);

public:
	static UMaterial* GetResourceMaterial(UWorld* World, const FName& RowName);

public:
	static FOrder GetOrderByIngredients(UWorld* World, ACooking* Cooking);



};
