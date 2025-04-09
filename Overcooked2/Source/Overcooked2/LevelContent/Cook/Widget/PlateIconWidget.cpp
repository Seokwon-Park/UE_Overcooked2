// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelContent/Cook/Widget/PlateIconWidget.h"
#include "Styling/SlateBrush.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UPlateIconWidget::Init()
{
	TArray<UImage*> Slots = { ImageSlot1, ImageSlot2, ImageSlot3 };
	for (int32 i = 0; i < Slots.Num(); i++)
	{
		Slots[i]->SetVisibility(ESlateVisibility::Collapsed); // ���� �� ����
	}
}

void UPlateIconWidget::SetIngredientTextures(const TArray<UTexture2D*>& InTextures)
{
    TArray<UImage*> Slots = { ImageSlot1, ImageSlot2, ImageSlot3 };

    for (int32 i = 0; i < Slots.Num(); ++i)
    {
        if (Slots[i])
        {
            if (i < InTextures.Num() && nullptr != InTextures[i])
            {
                Slots[i]->SetBrushFromTexture(InTextures[i]);
                Slots[i]->SetVisibility(ESlateVisibility::Visible);
            }
            else
            {
                Slots[i]->SetVisibility(ESlateVisibility::Collapsed); // ���� �� ����
            }
        }
    }

	if (1 == InTextures.Num())
	{
		Slots[0]->SetRenderTranslation(FVector2D(0.0f, 0.0f));
	}
	else if (2 == InTextures.Num())
	{
		for (int32 i = 0; i < Slots.Num(); ++i)
		{
			if (Slots[i] && Slots[i]->GetVisibility() == ESlateVisibility::Visible)
			{
				Slots[i]->SetRenderTranslation(FVector2D::ZeroVector);
				//Slots[i]->SetRenderTranslation(Offset);
			}
		}
	}
	else
	{
		const FVector2D Offset(0.0f, -250.0f); // Y�� �Ʒ���
		// 3�� �� ���� ��쿣 ��ġ �ʱ�ȭ (���� �ٽ� �ø�)
		for (int32 i = 0; i < Slots.Num(); ++i)
		{
			if (Slots[i])
			{
				Slots[i]->SetRenderTranslation(FVector2D::ZeroVector);
				Slots[i]->SetRenderTranslation(Offset);
			}
		}
	}

}

void UPlateIconWidget::Reset()
{
    TArray<UImage*> Slots = { ImageSlot1, ImageSlot2, ImageSlot3 };
    for (int32 i = 0; i < Slots.Num(); i++)
    {
        Slots[i]->SetBrushFromTexture(nullptr);
        Slots[i]->SetVisibility(ESlateVisibility::Collapsed);
		Slots[i]->SetRenderTranslation(FVector2D::ZeroVector);
    }
}
