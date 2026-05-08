#pragma once

#include "CoreMinimal.h"

class UGameEventHandleSubsystem;
struct FEvent_AcceptQuest;

class MUKSI_API FHandle_AcceptQuest
{
private:
    static void Handle_AcceptQuest(const UObject* WorldContext, const FEvent_AcceptQuest& Event);
public:
    static void Register(UGameEventHandleSubsystem& Subsystem);
};