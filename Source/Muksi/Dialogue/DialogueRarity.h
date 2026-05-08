// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DialogueRarity.generated.h"

//DialogueRarity should be sorted from lowest to highest.
UENUM(BlueprintType)
enum class EDialogueRarity : uint8
{
	Common,
	UnCommon,
	Rare,
	Epic,
	Legendary
};