#pragma once

#include "CoreMinimal.h"
#include "GameConditionType.generated.h"

UENUM(BlueprintType)
enum class EGameConditionType : uint8
{
	None UMETA(DisplayName = "None"),

	// Character
	CharacterLevel UMETA(DisplayName = "Character Level"),
	CharacterStat UMETA(DisplayName = "Character Stat"),
	CharacterEquip UMETA(DisplayName = "Character Equip"),

	// Item
	HasItem UMETA(DisplayName = "Has Item"),

	//Date
	TravelTime UMETA(DisplayName = "Travel Time"),

	// Quest
	QuestCompleted UMETA(DisplayName = "Quest Completed"),

	//Dialogue
	DialogueChecked UMETA(DisplayName = "Dialogue Checked"),

};