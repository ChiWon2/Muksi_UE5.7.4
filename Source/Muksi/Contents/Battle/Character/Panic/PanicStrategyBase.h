// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"
#include "UObject/Object.h"
#include "PanicStrategyBase.generated.h"

class ABattleManager;
class ABattleCharacterBase;
class UMuksiBattleCardDataAsset;


USTRUCT(BlueprintType)
struct MUKSI_API FPanicStrategyContext
{
	GENERATED_BODY()

	// Panic 행동을 수행하는 캐릭터
	UPROPERTY(BlueprintReadOnly, Category = "Panic")
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	// 필요한 상대 캐릭터, GridManager, RuntimeContext 등을
	// 특정 Strategy가 조회할 수 있도록 전달
	UPROPERTY(BlueprintReadOnly, Category = "Panic")
	TObjectPtr<ABattleManager> BattleManager = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Panic")
	TObjectPtr<UMuksiBattleCardDataAsset> PanicCard = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Panic")
	EBattleSimulationWorldType WorldType = EBattleSimulationWorldType::PlayerActualEnemyDeceived;
};

USTRUCT(BlueprintType)
struct MUKSI_API FPanicStrategyResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Panic")
	FHexOffsetCoord TargetCoord = FHexOffsetCoord::Invalid();

	UPROPERTY(BlueprintReadOnly, Category = "Panic")
	int32 Direction = INDEX_NONE;

	bool IsValid() const
	{
		return TargetCoord.IsValid();
	}
};

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class MUKSI_API UPanicStrategyBase : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Battle|Panic")
	FPanicStrategyResult SelectTarget(const FPanicStrategyContext& Context);

	virtual FPanicStrategyResult SelectTarget_Implementation(const FPanicStrategyContext& Context);
};
