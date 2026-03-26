#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include"../EventParameter.h"
#include "BaseEvent.generated.h"


UCLASS(Abstract, BlueprintType, Blueprintable)
class MUKSI_API UBaseEvent : public UObject
{
    GENERATED_BODY()

public:
    // 이벤트 식별자
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName EventID;

    // 이벤트 타입
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    EEventType EventType;
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText EventDescribe;

    // 단일 구조체 기반 실행
    virtual void Execute(const FEventParameter& Param) PURE_VIRTUAL(UBaseEvent::Execute, );
};
