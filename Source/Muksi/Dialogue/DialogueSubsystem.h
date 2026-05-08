#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DialogueRow.h"
#include "DialogueKey.h"
#include "DialogueSubsystem.generated.h"

class UDialogueWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueTextUpdated, const FText&, Text);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueOptionsUpdated, const TArray<FDialogueOption>&, Options);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueImageUpdated, const TSoftObjectPtr<UTexture2D>&,Image);

UCLASS()
class MUKSI_API UDialogueSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    static UDialogueSubsystem* Get(const UObject* WorldContextObject);
    void InitializeSubsystem();
public:
    UPROPERTY(BlueprintAssignable)
    FOnDialogueOptionsUpdated OnDialogueOptionsUpdated;

    UPROPERTY(BlueprintAssignable)
    FOnDialogueTextUpdated OnDialogueTextUpdated;

    UPROPERTY(BlueprintAssignable)
    FOnDialogueImageUpdated OnDialogueImageUpdated;

    UPROPERTY(BlueprintAssignable)
    FOnDialogueEnded OnDialogueEnded;



public:
    UFUNCTION(BlueprintCallable)
    void StartDialogueByKey(const FDialogueKey& Key);

    UFUNCTION(BlueprintCallable)
    void SelectOption(int32 OptionIndex);

    bool IsDialogueActive() const;

private:
    void LoadDialogueByKey(const FDialogueKey& Key);
    void EndDialogue();
    void ExecuteEvents(const TArray<FInstancedStruct>& Events);

public:
    UDataTable* GetTableByID(FName TableID) const;
    FDialogueRow* GetDialogueRow(const FDialogueKey& Key);
private:
    UPROPERTY()
    TMap<FName, UDataTable*> LoadedTables;

    FDialogueKey CurrentDialogueKey;
    FName CurrentTableID;

    UPROPERTY()
    UDataTable* CurrentDialogueTable = nullptr;
};