#pragma once

#include "CoreMinimal.h"
#include "TargetingConfirmResult.generated.h"

UENUM(BlueprintType)
enum class ETargetingConfirmResult : uint8
{
	//현재 단계 결과를 확정할 수 없음
	Failed UMETA(DisplayName = "Failed"),
	// 현재 단계는 확정됐고 다음 단계로 진행함
	AdvancedToNextStep UMETA(DisplayName = "Advanced To Next Step"),
	//마지막 단계까지 확정되어 최종 타기팅이 완료됨
	Completed UMETA(DisplayName = "Completed")
};