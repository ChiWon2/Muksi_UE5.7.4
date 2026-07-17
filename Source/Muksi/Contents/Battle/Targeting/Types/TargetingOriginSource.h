#pragma once

#include "CoreMinimal.h"
#include "TargetingOriginSource.generated.h"

UENUM(BlueprintType)
enum class ETargetingOriginSource : uint8
{
	//카드를 사용하는 캐릭터의 위치를 시작점으로 사용
	SourceCharacter UMETA(DisplayName = "Source Character"),
	//바로 이전 단계에서 선택한 위치를 시작점으로 사용
	PreviousStep UMETA(DisplayName = "Previous Step"),
	//지정한 단계에서 선택한 위치를 시작점으로 사용
	SpecificStep UMETA(DisplayName = "Specific Step")
};