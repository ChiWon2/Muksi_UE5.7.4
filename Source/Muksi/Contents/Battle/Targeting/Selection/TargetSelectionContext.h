#pragma once

#include "CoreMinimal.h"

#include "Muksi/Contents/Battle/Targeting/Context/TargetingInputContext.h"

#include "TargetSelectionContext.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;

USTRUCT(BlueprintType)
struct FTargetSelectionContext
{
	GENERATED_BODY()

	// 현재 카드를 사용하는 캐릭터.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	// 타기팅 계산에 사용하는 그리드 매니저.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	// 현재 프레임의 마우스 또는 AI 입력.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FTargetingInputContext InputContext;

	// 현재 처리 중인 Targeting Step 인덱스.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	int32 StepIndex = INDEX_NONE;

	// 현재 Step이 시작되는 그리드 좌표.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FIntPoint OriginCoord = FIntPoint(INDEX_NONE, INDEX_NONE);

	// 현재 Step이 시작되는 월드 위치.
	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FVector OriginWorldLocation = FVector::ZeroVector;

	bool HasOriginCoord() const
	{
		return OriginCoord.X != INDEX_NONE && OriginCoord.Y != INDEX_NONE;
	}

	void Reset()
	{
		SourceCharacter = nullptr;
		GridManager = nullptr;
		InputContext.Reset();
		StepIndex = INDEX_NONE;
		OriginCoord = FIntPoint(INDEX_NONE, INDEX_NONE);
		OriginWorldLocation = FVector::ZeroVector;
	}
};