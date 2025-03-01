// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Cook/Cooking.h"
#include <Global/Component/TimeEventComponent.h>

// Sets default values
ACooking::ACooking()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	TimeEvent = CreateDefaultSubobject<UTimeEventComponent>(TEXT("TimeEvent"));
}

//void ACooking::Interact(AActor* ChefActor)
//{
//	if (false == bIsInteracting)
//	{
//		AttachToActor(ChefActor, FAttachmentTransformRules::SnapToTargetIncludingScale);
//		FVector Offset = FVector(100.0f, 0.0f, 0.0f);
//		SetActorRelativeLocation(Offset);
//	}
//	else
//	{
//		DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);
//	}
//}

// Called when the game starts or when spawned
void ACooking::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ACooking::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

