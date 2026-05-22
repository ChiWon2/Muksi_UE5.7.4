#pragma once

#include "CoreMinimal.h"
#include "DialogueEffectTypes.generated.h"

//DialogueEffectTypes.h

class UTexture2D;
class UMaterialInterface;
class USoundBase;

USTRUCT(BlueprintType)
struct FDialogueEffectPreset
{
    GENERATED_BODY()

    // 화면 페이드 색상
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Config)
    FLinearColor FadeColor = FLinearColor::Black;

    // 페이드 시간
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Config)
    float FadeInTime = 0.25f;

    // 레어도 장식 이미지 (보석 등)
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Config)
    TSoftObjectPtr<UTexture2D> RarityGemTexture = nullptr;

    // Overlay Material
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Config)
    TSoftObjectPtr<UMaterialInterface> OverlayMaterial = nullptr;

    // 시작 사운드
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Config)
    TSoftObjectPtr<USoundBase> StartSFX = nullptr;
};