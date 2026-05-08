
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MuksiGameInstance.generated.h"


class UDialogueWidget;
class UDialogueSubsystem;
class UDialogueTriggerSubsystem;
class UEventHandleSubsystem;
class UTravelTimeSubsystem;
class UQuestSubsystem;
class AQuestObjectiveCreator;

UCLASS()
class MUKSI_API UMuksiGameInstance : public UGameInstance
{
	GENERATED_BODY()
public:
	virtual void Init() override;
private:
	void InitialzieDialogueSystem();

};
