#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Character/BattleStatComponent.h"
#include "BattleCharacterInitData.generated.h"

struct FCharacterPanicData;
class UMuksiCharacterDataAsset;
class UMuksiBattleCardDataAsset;
class UCharacterPassive;

class UTexture2D;

// 전투 캐릭터의 표시 정보.
USTRUCT(BlueprintType)
struct MUKSI_API FBattleCharacterInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName CharacterID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText CharacterName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> CharacterIllustration = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> CharacterHUD = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText FactionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (MultiLine = true))
	FText FactionDescription;
};

// 시간 초과 시 사용할 패널티 설정.
USTRUCT(BlueprintType)
struct MUKSI_API FCharacterPanicData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> PenaltyCard = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (MultiLine = true))
	FText CharacterDescription;

	bool IsValid() const
	{
		return PenaltyCard != nullptr;
	}
};


USTRUCT(BlueprintType)
struct MUKSI_API FBattleCharacterInitData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Init")
	TSubclassOf<ABattleCharacterBase> CharacterClass = nullptr;

	// 이름, 초상화, 소속 등 표시 정보.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Init")
	FBattleCharacterInfo CharacterInfo;
	
	// 전투 시작 능력치.
	// FBattleStats의 HP는 최대 HP를 의미함.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Init")
	FBattleStats Stats;

	// 전투에 진입할 때의 현재 HP.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Init")
	float CurrentHP = 0.f;

	// 전투에 사용할 스킬 목록.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Init")
	TArray<UMuksiBattleCardDataAsset*> Skills;

	// 최대 스킬 코스트.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Init")
	int32 MaxSkillCost = 10;

	// 전투에 적용할 패시브 클래스 목록.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Init")
	TArray<TSubclassOf<UCharacterPassive>> PassiveClasses;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Battle|Init")
	TArray<FCharacterPanicData> TimeoutPenalties;
};