#pragma once

#include "CoreMinimal.h"
#include "QuestKey.generated.h"

USTRUCT(BlueprintType)
struct FQuestKey
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName TableID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName RowID;

    bool IsValid() const
    {
        return !TableID.IsNone() && !RowID.IsNone();
    }

    FString ToString() const
    {
        return FString::Printf(TEXT("[%s : %s]"), *TableID.ToString(), *RowID.ToString());
    }

    bool operator==(const FQuestKey& Other) const
    {
        return TableID == Other.TableID && RowID == Other.RowID;
    }
};

FORCEINLINE uint32 GetTypeHash(const FQuestKey& Key)
{
    return HashCombine(GetTypeHash(Key.TableID), GetTypeHash(Key.RowID));
}