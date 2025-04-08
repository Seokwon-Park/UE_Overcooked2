// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <LevelContent/Cook/Cooking.h>
#include <Global/OC2Enum.h>
#include <Global/OC2Const.h>
#include "Plate.generated.h"

class APlateSpawner;

// ���� ~ ���ÿ� �ö� ������ �丮 ~ �丮���� ���� ~ �ϼ��� �丮
UCLASS()
class OVERCOOKED2_API APlate : public ACooking
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlate();

	// ���� ���� ��Ḧ �״� ��� ����
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void Add(class AIngredient* Ingredient);
	void Add_Implementation(class AIngredient* Ingredient);

	bool IsCombinationSuccessful() const
	{
		return bIsCombinationSuccessful;
	}


	UFUNCTION(BlueprintCallable)
	bool IsDirtyPlate();

	// ����(GameMode)���� ȣ���ؾ߸� ����Ǵ� �Լ���!!!
	// ������ ���� �� ȣ��
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void WashPlate();
	void WashPlate_Implementation();

	// ������ ���¸� Dirty�� �ٲ㼭 �����ϰ� ������ ȣ��
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void SetPlateState(EPlateState State);
	void SetPlateState_Implementation(EPlateState State);

	// ���� ���� �丮 ��Ḧ ������ ���ø� �ʱ� ���·� �ǵ���
	UFUNCTION(BlueprintCallable, NetMulticast, Reliable)
	void CleanPlate();
	void CleanPlate_Implementation();

	TArray<FRecipe> GetIngredients() const
	{
		return Ingredients;
	}

	FRecipe GetIngredient(int32 Index) const
	{
		return Ingredients[Index];
	}
	UFUNCTION(NetMulticast, Reliable)
	void SubmitPlate();

	void SpawnPlate();


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	bool CanPlaceOnPlate(AIngredient* Ingredient);
	void SetIngredinetMesh(FPlateInitData Data);
	void SetIngredinetTextures(FPlateInitData Data);

	void SetMesh();
	void SetMaterialTexture(UTexture* Texture);

	virtual void ForwardAttachToChef() override;
	virtual void ForwardDetachToChef() override;

	virtual void PostInitializeComponents() override;

private:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Cooking", meta = (AllowPrivateAccess = "true"))
	TArray<FRecipe> Ingredients;

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Cooking", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* IngredientMesh = nullptr; // ���

	// ���� ����
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Cooking", meta = (AllowPrivateAccess = "true"))
	EPlateState PlateState = EPlateState::EMPTY;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking", meta = (AllowPrivateAccess = "true"))
	UTexture* CleanTexture = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking", meta = (AllowPrivateAccess = "true"))
	UTexture* DirtyTexture = nullptr;

	UPROPERTY(Replicated)
	bool bIsCombinationSuccessful = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooking", meta = (AllowprivateAccess = "true"))
	class UWidgetComponent* WidgetComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooking", meta = (AllowprivateAccess = "true"))
	TSubclassOf<UUserWidget> SubclassWidget = nullptr; // �����Ϳ��� ������ WBP ����

	UPROPERTY()
	class UPlateIconWidget* IconWidget = nullptr; // ������ ���� ��ü ���� �� �Լ� ȣ���

	UPROPERTY()
	APlateSpawner* PlateSpawner = nullptr;

};
