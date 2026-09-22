#pragma once

#include "CoreMinimal.h"
#include "BattleSkillTypes.generated.h"

class UMuksiBattleCardDataAsset;

USTRUCT(BlueprintType)
struct FBattleSkillInstance
{
	GENERATED_BODY()

public:
	// 같은 스킬이 여러 슬롯에 들어갈 수도 있으므로
	// 각 스킬 인스턴스를 구분하기 위한 ID
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (IgnoreForMemberInitializationTest))
	FGuid InstanceId;

	// 실제 스킬 데이터
	// 일단 기존 CardDataAsset을 그대로 사용
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> SkillData = nullptr;

	// 스킬바에서 몇 번째 슬롯인지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 SlotIndex = INDEX_NONE;

	// 현재 남아있는 쿨다운
	// 0이면 사용 가능
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 RemainingCooldown = 0;


public:
	FBattleSkillInstance()
		: InstanceId(FGuid::NewGuid())
	{
	}

	explicit FBattleSkillInstance(
		UMuksiBattleCardDataAsset* InSkillData,
		int32 InSlotIndex)
		: InstanceId(FGuid::NewGuid())
		, SkillData(InSkillData)
		, SlotIndex(InSlotIndex)
		, RemainingCooldown(0)
	{
	}

	bool IsValid() const
	{
		return SkillData != nullptr;
	}

	bool IsReady() const
	{
		return IsValid() && RemainingCooldown <= 0;
	}
};