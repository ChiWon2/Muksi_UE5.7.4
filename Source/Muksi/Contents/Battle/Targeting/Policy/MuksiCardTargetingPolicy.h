#pragma once

#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
#include "UObject/Object.h"

#include "Muksi/Contents/Battle/Targeting/Types/MuksiCardTargetingContext.h"
#include "Muksi/Contents/Battle/Targeting/Types/MuksiCardTargetingResult.h"

#include "MuksiCardTargetingPolicy.generated.h"

UCLASS(Abstract, Blueprintable, EditInlineNew, DefaultToInstanced)
class MUKSI_API UMuksiCardTargetingPolicy : public UObject
{
	GENERATED_BODY()

public:
	// 현재 입력을 바탕으로 카드 타기팅 결과를 계산한다.
	virtual void BuildTargetingResult(const FMuksiCardTargetingContext& Context, const FInstancedStruct& TargetingData, FMuksiCardTargetingResult& OutResult) const;

	// 이 Policy가 사용하는 TargetingData 구조체 타입을 반환한다.
	virtual const UScriptStruct* GetTargetingDataStruct() const;

protected:
	// 전달된 TargetingData가 이 Policy에서 요구하는 타입인지 확인한다.
	bool IsTargetingDataValid(const FInstancedStruct& TargetingData) const;

	// 타기팅 결과를 현재 Context의 기본 정보로 초기화한다.
	void InitializeResult(const FMuksiCardTargetingContext& Context, FMuksiCardTargetingResult& OutResult) const;
};