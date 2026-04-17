#pragma once

// Types that use TargetValue
#define COND_TARGET_TYPES \
"Type == CharacterLevel || " \
"Type == CharacterStat || " \
"Type == CharacterEquip || " \
"Type == HasItem"

// Types that use NameValue
#define COND_NAME_TYPES \
"Type == CharacterLevel || " \
"Type == CharacterStat || " \
"Type == CharacterEquip || " \
"Type == HasItem || " \
"Type == QuestCompleted || " \
"Type == DialogueChecked || "

// Types that use IntValue
#define COND_INT_TYPES \
"Type == CharacterLevel || " \
"Type == CharacterStat || " \
"Type == HasItem"

// Types that use BoolValue
#define COND_BOOL_TYPES \
"Type == QuestCompleted || " \
"Type == DialogueChecked"

// Types that use CompareOpr
#define COND_COMPARE_TYPES \
"Type == CharacterLevel || " \
"Type == CharacterStat ||" \
"Type == HasItem"

