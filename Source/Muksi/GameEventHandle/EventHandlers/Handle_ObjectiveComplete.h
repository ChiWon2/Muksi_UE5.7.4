#pragma once

#include "CoreMinimal.h"

class UGameEventHandleSubsystem;
struct FEvent_ObjectiveComplete;

class MUKSI_API FHandle_ObjectiveComplete
{
private:
    static void Handle_ObjectiveComplete(const UObject* WorldContext, const FEvent_ObjectiveComplete& Event);
public:
    static void Register(UGameEventHandleSubsystem& Subsystem);
};