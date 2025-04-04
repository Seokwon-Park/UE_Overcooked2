// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LevelContent/Table/CookingTable.h"
#include "SinkTable.generated.h"

/**
 * 
 */
UCLASS()
class OVERCOOKED2_API ASinkTable : public ACookingTable
{
	GENERATED_BODY()

public:
	ASinkTable();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	virtual void PlaceItem(ACooking* ReceivedCooking) override;

	void DoTheDishes();
};
