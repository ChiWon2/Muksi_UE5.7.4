#pragma once

#include "CoreMinimal.h"
#include "DialogueKey.generated.h"

USTRUCT(BlueprintType)
struct FDialogueKey
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TableID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RowID;

    bool IsValid() const
    {
        return !RowID.IsNone();
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("[%s : %s]"), *TableID.ToString(), *RowID.ToString());
    }

    bool operator==(const FDialogueKey& Other) const
    {
        return TableID == Other.TableID && RowID == Other.RowID;
    }
};

FORCEINLINE uint32 GetTypeHash(const FDialogueKey& Key)
{
    return HashCombine(GetTypeHash(Key.TableID), GetTypeHash(Key.RowID));
}