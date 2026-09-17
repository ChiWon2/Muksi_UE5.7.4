#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Execution/Data/BattleExecutionTypes.h"
#include "Muksi/Contents/Battle/Targeting/Context/TargetingIntent.h"
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

	// 행동 생성 시점의 카드 속도
	UPROPERTY(BlueprintReadOnly)
	float CardSpeed = 0.0f;

	// 행동 생성 시점의 캐릭터 속도
	UPROPERTY(BlueprintReadOnly)
	float CharacterSpeed = 0.0f;

	// 플레이어 행동인지
	UPROPERTY(BlueprintReadOnly)
	bool bPlayerAction = false;

	// 카드 선택 과정에서 확정된 Targeting 의도
	UPROPERTY(BlueprintReadOnly)
	FTargetingIntent TargetingIntent;

	// 이번 BattleAction에서 실제로 실행할 Main Execution 목록.
	UPROPERTY(BlueprintReadOnly)
	TArray<FBattleExecutionEntry> ExecutionEntries;

	bool IsHigherPriorityThan(const FBattleAction& Other) const
	{
		if (CardSpeed != Other.CardSpeed)
			return CardSpeed > Other.CardSpeed;

		if (CharacterSpeed != Other.CharacterSpeed)
			return CharacterSpeed > Other.CharacterSpeed;

		if (bPlayerAction != Other.bPlayerAction)
			return bPlayerAction;

		return false;
	}
};
