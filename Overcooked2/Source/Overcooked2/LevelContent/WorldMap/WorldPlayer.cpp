// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/WorldMap/WorldPlayer.h"
#include "LevelContent/WorldMap/WorldMapData.h"

#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "Camera/CameraActor.h"
#include "Camera/CameraComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

#include "Global/OC2GameInstance.h"
#include "Global/OC2Global.h"
#include "Global/State/GameState/WorldGameState.h"
#include "Global/Data/OC2GlobalData.h"

#include "UI/WorldMap/WorldMapHUD.h"
#include "UI/WorldMap/UI/WorldMapUserWidget.h"
#include "UI/Loading/LoadingWidget.h"

FVector AWorldPlayer::WorldPlayerLocation = UWorldMapData::START_LOC;

// Sets default values
AWorldPlayer::AWorldPlayer()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	AudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
	AudioComponent->SetupAttachment(GetCapsuleComponent());
	AudioComponent->bAutoActivate = false; // �ڵ���� ����

	DefaultGravity = GetCharacterMovement()->GravityScale;
}

void AWorldPlayer::BeginPlay()
{
	Super::BeginPlay();

	Hide();

	if (HasAuthority())
	{
		if (Controller != nullptr)	// Server side Server
		{
			Show_Client();
		}
	}
	else
	{
		if (Controller == nullptr)	// Client Side Server
		{
			Show_Client();
		}
	}

	FString BackgroundSound = TEXT("VanEngine");
	USoundBase* EngineSound = UOC2GlobalData::GetWorldBaseSound(GetWorld(), *BackgroundSound);

	if (nullptr != EngineSound && nullptr != AudioComponent)
	{
		AudioComponent->SetSound(EngineSound); // ���� ����
		AudioComponent->bAutoActivate = true;
		AudioComponent->bIsUISound = false; // 3D ����� ����
		AudioComponent->SetVolumeMultiplier(0.2f); // ������ ���� ����
	}

	SetActorLocation(UWorldMapData::START_LOC);
	AudioComponent->Play();

	WorldGameState = Cast<AWorldGameState>(UGameplayStatics::GetGameState(this));
}

void AWorldPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Controller && HasAuthority() == false)	// Client Side Client
	{
		SetActorLocation(WorldPlayerLocation);
	}

	if (IsLocallyControlled() && AudioComponent)
	{
		FVector Velocity = GetVelocity();
		float Speed = Velocity.Size();
		float MaxSpeed = 600.f;

		// ����: �ӵ� ��� 0.3 ~ 1.0
		float Volume = 0.2f + FMath::Clamp(Speed / MaxSpeed, 0.f, 1.f) * 0.7f;
		AudioComponent->SetVolumeMultiplier(Volume);

		// ��ġ�� ���� ���� (����)
		float Pitch = 0.8f + FMath::Clamp(Speed / MaxSpeed, 0.f, 1.f) * 0.4f;
		AudioComponent->SetPitchMultiplier(Pitch);

		// �׻� ����ǵ��� ����
		if (!AudioComponent->IsPlaying())
		{
			AudioComponent->Play();
		}
	}
}

void AWorldPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AWorldPlayer::OnMove(const FVector2D& _Vec)
{
	if (Controller && HasAuthority())
	{
		FVector MoveDir = FVector(_Vec.Y * 0.5f, _Vec.X * -0.5f, 0.f);
		AddMovementInput(MoveDir);
		UpdateBusLocation(GetActorLocation());
	}
}

void AWorldPlayer::OnSelectMap()
{
	if (Controller && HasAuthority())
	{
		if (nullptr != WorldGameState)
		{
			WorldGameState->Multicast_PlayZoomInAnmationUI();
		}
	}
}

void AWorldPlayer::ToggleController(bool _IsOn)
{
	if (Controller == nullptr)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(Controller);
	if (PC == nullptr)
	{
		return;
	}

	if (_IsOn)
	{
		EnableInput(PC);
	}
	else
	{
		DisableInput(PC);
	}
}

void AWorldPlayer::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AWorldPlayer, WorldPlayerLocation);
}

void AWorldPlayer::Show_Client_Implementation()
{
	Show();
}

void AWorldPlayer::Hide_Client_Implementation()
{
	Hide();
}

void AWorldPlayer::Show()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	//SetReplicates(true);
	GetCharacterMovement()->GravityScale = DefaultGravity;
}

void AWorldPlayer::Hide()
{
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	//SetReplicates(false);
	GetCharacterMovement()->GravityScale = 0.0f;
}

void AWorldPlayer::UpdateBusLocation_Implementation(FVector _Loc)
{
	WorldPlayerLocation = _Loc;
}