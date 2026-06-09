#pragma once

#include "CoreMinimal.h"

class UGameEventHandleSubsystem;
struct FEvent_CompleteQuest;

class MUKSI_API FHandle_CompleteQuest
{
private:
    static void Handle_CompleteQuest(const UObject* WorldContext, const FEvent_CompleteQuest& Event);
public:
    static void Register(UGameEventHandleSubsystem& Subsystem);
};