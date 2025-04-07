// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CookingFinalScoreWidget.generated.h"

/**
 * 
 */
UCLASS()
class OVERCOOKED2_API UCookingFinalScoreWidget : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "OC2UI")
	class UCanvasPanel* PlayerCanvas = nullptr;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "OC2UI")
	class UImage* PlayerImg_0 = nullptr;
	
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget), Category = "OC2UI")
	class UImage* PlayerImg_1 = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderTarget")
	UTextureRenderTarget2D* RenderTarget0;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RenderTarget")
	UTextureRenderTarget2D* RenderTarget1;

protected:
	virtual void NativeOnInitialized() override;

private:
	void ShowPlayers(int Index);

	int CurPlayerCount = 0;

	template <typename T>
	T* FindChildWidget(const FString& name, UCanvasPanel* canvas);
};
	