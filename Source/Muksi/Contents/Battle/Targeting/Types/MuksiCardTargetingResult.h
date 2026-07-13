#pragma once

#include "CoreMinimal.h"
#include "MuksiCardTargetingResult.generated.h"

class ABattleCharacterBase;

USTRUCT(BlueprintType)
struct FMuksiCardTargetingResult
{
	GENERATED_BODY()

	// 현재 타기팅 결과를 확정할 수 있는지 나타낸다.
	UPROPERTY(BlueprintReadOnly)
	bool bCanConfirm = false;

	// 사용자가 최종적으로 선택한 대표 그리드 좌표.
	UPROPERTY(BlueprintReadOnly)
	FIntPoint SelectedCoord = FIntPoint(INDEX_NONE, INDEX_NONE);

	// 시각적 조준 연출이 사용하는 월드 위치.
	UPROPERTY(BlueprintReadOnly)
	FVector AimWorldLocation = FVector::ZeroVector;

	// 육각 그리드 기준으로 계산된 방향.
	UPROPERTY(BlueprintReadOnly)
	int32 Direction = INDEX_NONE;

	// 실제 카드 효과가 적용될 그리드 좌표 목록.
	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> AffectedCoords;

	// 투사체 또는 이동에 사용하는 경로 좌표 목록.
	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> PathCoords;

	// 최종적으로 확정된 대상 캐릭터 목록.
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<ABattleCharacterBase>> TargetCharacters;

	// 최종 선택된 그리드 좌표가 있는지 반환한다.
	bool HasSelectedCoord() const
	{
		return SelectedCoord.X != INDEX_NONE && SelectedCoord.Y != INDEX_NONE;
	}

	// 첫 번째 최종 대상 캐릭터를 반환한다.
	ABattleCharacterBase* GetPrimaryTargetCharacter() const
	{
		return TargetCharacters.IsValidIndex(0) ? TargetCharacters[0] : nullptr;
	}

	// 타기팅 결과를 기본 상태로 초기화한다.
	void Reset()
	{
		bCanConfirm = false;
		SelectedCoord = FIntPoint(INDEX_NONE, INDEX_NONE);
		AimWorldLocation = FVector::ZeroVector;
		Direction = INDEX_NONE;
		AffectedCoords.Empty();
		PathCoords.Empty();
		TargetCharacters.Empty();
	}
};