#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepResult.h"

class ABattleCharacterBase;
class ABattleGridManager;
struct FTargetingPhasePresentationSettings;
struct FTargetingStepCardData;

struct FTargetingPreviewContext
{
    ABattleCharacterBase* SourceCharacter = nullptr;
    ABattleGridManager* GridManager = nullptr;
    const FTargetingStepCardData* StepData = nullptr;
    const FTargetingStepResult* TargetingStep = nullptr;
    const FTargetingPhasePresentationSettings* PresentationSettings = nullptr;
    const TArray<FHexOffsetCoord>* PreviewAffectedCoords = nullptr;
    const TArray<FHexOffsetCoord>* PreviewPathCoords = nullptr;

    bool IsValid() const { return SourceCharacter && GridManager && StepData && PresentationSettings; }
    bool IsStepValid() const { return TargetingStep != nullptr; }

    bool HasOriginCoord() const { return TargetingStep && TargetingStep->Step.HasOriginCoord(); }
    FHexOffsetCoord GetOriginCoord() const { return TargetingStep ? TargetingStep->Step.OriginCoord : FHexOffsetCoord::Invalid(); }

    bool HasTargetCoord() const { return TargetingStep && TargetingStep->Step.HasTargetCoord(); }
    FHexOffsetCoord GetTargetCoord() const { return TargetingStep ? TargetingStep->Step.TargetCoord : FHexOffsetCoord::Invalid(); }

    bool HasDirection() const { return TargetingStep && TargetingStep->Step.HasDirection(); }
    int32 GetDirection() const { return TargetingStep ? TargetingStep->Step.Direction : INDEX_NONE; }

    const TArray<FHexOffsetCoord>* GetAffectedCoords() const
    {
        if (TargetingStep) return &TargetingStep->AffectedCoords;
        return PreviewAffectedCoords;
    }

    const TArray<FHexOffsetCoord>* GetPathCoords() const
    {
        if (TargetingStep) return &TargetingStep->PathCoords;
        return PreviewPathCoords;
    }
};
