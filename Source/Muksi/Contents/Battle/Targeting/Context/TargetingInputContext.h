#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "TargetingInputContext.generated.h"

class ABattleCharacterBase;

USTRUCT(BlueprintType)
struct FTargetingInputContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	FHexOffsetCoord HoveredCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	FVector AimWorldLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	TArray<TObjectPtr<ABattleCharacterBase>> CandidateCharacters;

	bool HasHoveredCoord() const
	{
		return HoveredCoord.X != INDEX_NONE && HoveredCoord.Y != INDEX_NONE;
	}

	ABattleCharacterBase* GetPrimaryCandidateCharacter() const
	{
		return CandidateCharacters.IsValidIndex(0) ? CandidateCharacters[0] : nullptr;
	}

	void Reset()
	{
		HoveredCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
		AimWorldLocation = FVector::ZeroVector;
		CandidateCharacters.Empty();
	}
};
