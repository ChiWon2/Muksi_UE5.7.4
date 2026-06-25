// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EQuestState : uint8
{
    Inactive     UMETA(DisplayName = "Inactive"),
    Active       UMETA(DisplayName = "Active"),
    ReadyToComplete UMETA(DisplayName = "ReadyToComplete"),
    Completed    UMETA(DisplayName = "Completed"),
    Failed       UMETA(DisplayName = "Failed")
};