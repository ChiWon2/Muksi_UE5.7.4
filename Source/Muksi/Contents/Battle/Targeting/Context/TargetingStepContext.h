#pragma once

#include "CoreMinimal.h"
#include "TargetingStepContext.generated.h"

class ABattleCharacterBase;
/*
* 현재 한 단계에서 계산된 선택 상태를 저장한다.
*/

USTRUCT(BlueprintType)
struct FTargetingStepContext
{
	GENERATED_BODY()

	// 현재 단계 결과를 확정할 수 있는지 나타낸다.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	bool bCanConfirm = false;

	// 현재 단계의 시작 그리드 좌표.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FIntPoint OriginCoord = FIntPoint(INDEX_NONE, INDEX_NONE);

	// 현재 단계의 시작 월드 위치.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FVector OriginWorldLocation = FVector::ZeroVector;

	// 현재 단계에서 선택 가능한 모든 그리드 좌표.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FIntPoint> SelectableCoords;

	// 현재 단계에서 선택된 대표 그리드 좌표.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FIntPoint SelectedCoord = FIntPoint(INDEX_NONE, INDEX_NONE);

	// 현재 단계에서 선택된 대표 월드 위치.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FVector SelectedWorldLocation = FVector::ZeroVector;

	// 현재 단계에서 사용한 조준 월드 위치.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FVector AimWorldLocation = FVector::ZeroVector;

	// 현재 단계에서 계산된 육각 그리드 방향.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	int32 Direction = INDEX_NONE;

	// 현재 단계에서 선택된 캐릭터 목록.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<TObjectPtr<ABattleCharacterBase>> TargetCharacters;

	bool HasOriginCoord() const
	{
		return OriginCoord.X != INDEX_NONE && OriginCoord.Y != INDEX_NONE;
	}

	bool HasSelectedCoord() const
	{
		return SelectedCoord.X != INDEX_NONE && SelectedCoord.Y != INDEX_NONE;
	}

	bool HasDirection() const
	{
		return Direction != INDEX_NONE;
	}

	bool IsSelectableCoord(const FIntPoint& Coord) const
	{
		return SelectableCoords.Contains(Coord);
	}

	ABattleCharacterBase* GetPrimaryTargetCharacter() const
	{
		return TargetCharacters.IsValidIndex(0) ? TargetCharacters[0] : nullptr;
	}

	void Reset()
	{
		bCanConfirm = false;
		OriginCoord = FIntPoint(INDEX_NONE, INDEX_NONE);
		OriginWorldLocation = FVector::ZeroVector;
		SelectableCoords.Empty();
		SelectedCoord = FIntPoint(INDEX_NONE, INDEX_NONE);
		SelectedWorldLocation = FVector::ZeroVector;
		AimWorldLocation = FVector::ZeroVector;
		Direction = INDEX_NONE;
		TargetCharacters.Empty();
	}
};