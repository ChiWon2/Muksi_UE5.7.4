#pragma once

#include "CoreMinimal.h"

class UGameEventHandleSubsystem;
struct FEvent_StartBattle;

class MUKSI_API FHandle_StartBattle
{
private:
    static void Handle_StartBattle(const UObject* WorldContext, const FEvent_StartBattle& Event);
public:
    static void Register(UGameEventHandleSubsystem& Subsystem);
};