#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingStepContext.h"
#include "TargetingResult.generated.h"

class ABattleCharacterBase;
/*
* 모든 단계와 Pattern 계산이 끝난 뒤 BattleManager 등 외부 시스템에 전달하는 최종 결과.
*/
USTRUCT(BlueprintType)
struct FTargetingResult
{
	GENERATED_BODY()

	// 순서대로 확정된 모든 Targeting Step 상태.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FTargetingStepContext> StepContexts;

	// 최종 AreaPattern이 계산한 실제 효과 적용 좌표 목록.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FIntPoint> AffectedCoords;

	// 이동, 투사체 또는 경로 효과가 사용하는 좌표 목록.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FIntPoint> PathCoords;

	// 카드 전체 타기팅 결과로 확정된 캐릭터 목록.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<TObjectPtr<ABattleCharacterBase>> TargetCharacters;

	const FTargetingStepContext* GetStepContext(int32 StepIndex) const
	{
		return StepContexts.IsValidIndex(StepIndex) ? &StepContexts[StepIndex] : nullptr;
	}

	const FTargetingStepContext* GetLastStepContext() const
	{
		return StepContexts.IsEmpty() ? nullptr : &StepContexts.Last();
	}

	bool HasSelectedCoord() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext && StepContext->HasSelectedCoord();
	}

	FIntPoint GetOriginCoord() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->OriginCoord : FIntPoint(INDEX_NONE, INDEX_NONE);
	}

	FVector GetOriginWorldLocation() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->OriginWorldLocation : FVector::ZeroVector;
	}

	FIntPoint GetSelectedCoord() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->SelectedCoord : FIntPoint(INDEX_NONE, INDEX_NONE);
	}

	FVector GetSelectedWorldLocation() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->SelectedWorldLocation : FVector::ZeroVector;
	}

	FVector GetAimWorldLocation() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->AimWorldLocation : FVector::ZeroVector;
	}

	int32 GetDirection() const
	{
		const FTargetingStepContext* StepContext = GetLastStepContext();
		return StepContext ? StepContext->Direction : INDEX_NONE;
	}

	ABattleCharacterBase* GetPrimaryTargetCharacter() const
	{
		return TargetCharacters.IsValidIndex(0) ? TargetCharacters[0] : nullptr;
	}

	void AddAffectedCoord(const FIntPoint& Coord)
	{
		AffectedCoords.AddUnique(Coord);
	}

	void AddPathCoord(const FIntPoint& Coord)
	{
		PathCoords.AddUnique(Coord);
	}

	void Reset()
	{
		StepContexts.Empty();
		AffectedCoords.Empty();
		PathCoords.Empty();
		TargetCharacters.Empty();
	}
};