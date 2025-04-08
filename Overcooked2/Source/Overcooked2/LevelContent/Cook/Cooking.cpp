// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Cook/Cooking.h"

// Sets default values
ACooking::ACooking()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	


	if (nullptr != StaticMeshComponent)
	{
		StaticMeshComponent->SetMobility(EComponentMobility::Movable);
		StaticMeshComponent->SetSimulatePhysics(true);
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
		StaticMeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	}

}

void ACooking::AttachToChef_Implementation(AActor* Player)
{
	StaticMeshComponent->SetSimulatePhysics(false);
	StaticMeshComponent->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttachToActor(Player, FAttachmentTransformRules::KeepRelativeTransform);

	ForwardAttachToChef(); // ĳ���Ͱ� ������ ���̺� ������ nullptr�� �����.
}

void ACooking::DetachFromChef_Implementation(AActor* Player)
{
	StaticMeshComponent->SetSimulatePhysics(true);
	StaticMeshComponent->SetCollisionProfileName(TEXT("Interactable"));
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	DetachFromActor(FDetachmentTransformRules::KeepRelativeTransform);

	// ���̸� Table���� SetCookingTable �Լ� ȣ���ؼ� ���̺� ������ �־��ش�.
	ForwardDetachToChef();
}

// Called when the game starts or when spawned
void ACooking::BeginPlay()
{
	AOC2Actor::BeginPlay();
	
	SetReplicateMovement(true);
}

// Called every frame
void ACooking::Tick(float DeltaTime)
{
	AOC2Actor::Tick(DeltaTime);

}

void ACooking::SetCookingTable_Implementation(ACookingTable* Table)
{
	ForwardCookingTable(Table);
}

