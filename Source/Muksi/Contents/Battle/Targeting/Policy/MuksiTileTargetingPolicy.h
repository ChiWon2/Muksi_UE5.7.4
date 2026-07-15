#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Policy/MuksiCardTargetingPolicy.h"
#include "MuksiTileTargetingPolicy.generated.h"

UCLASS()
class MUKSI_API UMuksiTileTargetingPolicy : public UMuksiCardTargetingPolicy
{
	GENERATED_BODY()

public:
	// 현재 호버 타일을 기준으로 타기팅 결과를 계산한다.
	virtual void BuildTargetingResult(const FMuksiCardTargetingContext& Context, const FInstancedStruct& TargetingData, FMuksiCardTargetingResult& OutResult) const override;

	// Tile Targeting Policy가 사용하는 데이터 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetTargetingDataStruct() const override;

private:
	// 두 육각 그리드 좌표 사이의 거리를 계산한다.
	int32 GetHexDistance(const ABattleGridManager* GridManager, const FIntPoint& StartCoord, const FIntPoint& EndCoord) const;
};