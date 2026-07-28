#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingResult.h"
#include "BattleAction.generated.h"

class ABattleCharacterBase;
class UMuksiBattleCardDataAsset;

USTRUCT(BlueprintType)
struct FBattleAction
{
	GENERATED_BODY()

	// 몇 번째 합의 행동인지: 0, 1, 2
	UPROPERTY(BlueprintReadOnly)
	int32 ExchangeIndex = INDEX_NONE;

	// 행동하는 캐릭터
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> Attacker = nullptr;

	// 사용하는 카드
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> Card = nullptr;

	// 행동 생성 시점의 속도
	UPROPERTY(BlueprintReadOnly)
	int32 Speed = 0;

	// 플레이어 행동인지
	UPROPERTY(BlueprintReadOnly)
	bool bPlayerAction = false;

	// 카드 선택 과정에서 확정된 전체 Targeting 결과
	UPROPERTY(BlueprintReadOnly)
	FTargetingResult TargetingResult;
};