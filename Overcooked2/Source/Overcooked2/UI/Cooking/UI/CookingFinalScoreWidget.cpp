// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Cooking/UI/CookingFinalScoreWidget.h"
#include "Components/CanvasPanel.h"
#include "Components/Image.h"
#include "UI/Cooking/CaptureComponent2D.h"
#include "Character/OC2Character.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Global/Data/OC2GlobalData.h"
#include "Kismet/GameplayStatics.h"

void UCookingFinalScoreWidget::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    //CurPlayerCount = 4;

    //RenderTargets = { RenderTarget0, RenderTarget1, RenderTarget2, RenderTarget3 };
    PlayerImgs = { PlayerImg_0, PlayerImg_1, PlayerImg_2, PlayerImg_3 };

}

void UCookingFinalScoreWidget::NativeTick(const FGeometry& MyGeometry, float DeltaTime)
{
    Super::NativeTick(MyGeometry, DeltaTime);


}

void UCookingFinalScoreWidget::ShowCapturePlayers()
{
    //PlayerCharacters.Empty();

    //// 플레이어 캐릭터 검색
    //TArray<AActor*> FoundCharacters;
    //UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOC2Character::StaticClass(), FoundCharacters);

    //for (AActor* Actor : FoundCharacters)
    //{
    //    if (AOC2Character* Character = Cast<AOC2Character>(Actor))
    //    {
    //        PlayerCharacters.Add(Character);
    //    }
    //}

    //CurPlayerCount = PlayerCharacters.Num();

    for (int i = 0; i < UOC2Global::GetOC2GameInstance(GetWorld())->GetUserCount(); i++)
    {
        ShowPlayers(i);
    }

    //for (int32 i = 0; i < PlayerCharacters.Num() && i < RenderTargets.Num() && i < PlayerImgs.Num(); ++i)
    //{
    //    AOC2Character* PlayerCharacter = PlayerCharacters[i];
    //    if (!IsValid(PlayerCharacter)) continue;

    //    UCaptureComponent2D* CaptureComponent = PlayerCharacter->FindComponentByClass<UCaptureComponent2D>();
    //    if (!IsValid(CaptureComponent)) continue;

    //    if (!IsValid(RenderTargets[i]))
    //    {
    //        UE_LOG(LogTemp, Warning, TEXT("RenderTarget[%d] is not valid!"), i);
    //        continue;
    //    }

    //    // 렌더 타겟 설정
    //    CaptureComponent->TextureTarget = RenderTargets[i];
    //    CaptureComponent->bAlwaysPersistRenderingState = true;
    //    CaptureComponent->MarkRenderStateDirty();
    //    CaptureComponent->CaptureScene();

    //    // 렌더 타겟이 유효한지 체크
    //    UTextureRenderTarget2D* RenderedTexture = CaptureComponent->TextureTarget;
    //    if (!IsValid(RenderedTexture)) continue;

    //    if (!IsValid(PlayerImgs[i])) continue;

    //    // 기존 머티리얼이 없으면 새로 생성
    //    UMaterialInstanceDynamic* DynamicMaterial = PlayerImgs[i]->GetDynamicMaterial();
    //    //if (!DynamicMaterial)
    //    //{
    //    //    UMaterialInterface* BaseMaterial = PlayerImgs[i]->GetBrush().GetResourceObject()
    //    //        ? Cast<UMaterialInterface>(PlayerImgs[i]->GetBrush().GetResourceObject())
    //    //        : nullptr;

    //    //    if (BaseMaterial)
    //    //    {
    //    //        DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, this);
    //    //    }
    //    //}

    //    if (!DynamicMaterial) continue;

    //    DynamicMaterial->SetTextureParameterValue(FName("TextureParam"), RenderedTexture);
    //    PlayerImgs[i]->SetBrushFromMaterial(DynamicMaterial);
    //    PlayerImgs[i]->SynchronizeProperties();
    //}
}

void UCookingFinalScoreWidget::ShowTexturePlayers()
{
    for (int i = 0; i < UOC2Global::GetOC2GameInstance(GetWorld())->GetUserCount(); i++)
    {
        PlayerImgs[i]->SetBrushFromTexture((UOC2GlobalData::GetChefTextureByIndex(GetWorld(), i)));
        ShowPlayers(i);
    }

}



void UCookingFinalScoreWidget::ShowPlayers(int Index)
{

    FString PlayerNum = "Player_" + FString::FromInt(Index);
    UCanvasPanel* Player = FindChildWidget<UCanvasPanel>(PlayerNum, PlayerCanvas);

    if (Player != nullptr)
    {
        static const TMap<int, TArray<float>> XOffsetsMap = {
            {1, {0.0f}},
            {2, {-1.0f, 1.0f}},
            {3, {-1.0f, 0.0f, 1.0f}},
            {4, {-1.5f, -0.5f, 0.5f, 1.5f}}
        };

        const float SizeX = 350.0f;

        float XOffset = 0.0f;

        if (XOffsetsMap.Contains(UOC2Global::GetOC2GameInstance(GetWorld())->GetUserCount()))
        {
            TArray<float> XOffsetArray = XOffsetsMap[UOC2Global::GetOC2GameInstance(GetWorld())->GetUserCount()];

            if (XOffsetArray.IsValidIndex(Index))
            {
                XOffset = XOffsetArray[Index] * SizeX;
            }
        }

        Player->SetRenderTranslation({ XOffset, 0.0f });
        Player->SetVisibility(ESlateVisibility::Visible);
    }
}


template <typename T>
T* UCookingFinalScoreWidget::FindChildWidget(const FString& Name, UCanvasPanel* Canvas)
{
    if (!Canvas) return nullptr;

    FString TargetPrefix = Name;
    TArray<UWidget*> Children = Canvas->GetAllChildren();

    for (UWidget* Child : Children)
    {
        if (T* Widget = Cast<T>(Child))
        {
            if (Widget->GetName().StartsWith(TargetPrefix))
            {
                return Widget;
            }
        }
    }

    return nullptr;
}

