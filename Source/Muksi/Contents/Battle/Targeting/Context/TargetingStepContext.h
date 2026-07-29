#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "TargetingStepContext.generated.h"

class ABattleCharacterBase;

USTRUCT(BlueprintType)
struct FTargetingStepContext
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	bool bCanConfirm = false;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexOffsetCoord OriginCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FVector OriginWorldLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<FHexOffsetCoord> SelectableCoords;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FHexOffsetCoord SelectedCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FVector SelectedWorldLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	FVector AimWorldLocation = FVector::ZeroVector;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	int32 Direction = INDEX_NONE;

	UPROPERTY(BlueprintReadOnly, Category = "Targeting")
	TArray<TObjectPtr<ABattleCharacterBase>> TargetCharacters;

	bool HasOriginCoord() const
	{
		return OriginCoord.X != INDEX_NONE && OriginCoord.Y != INDEX_NONE;
	}

	bool HasSelectedCoord() const
	{
		return SelectedCoord.X != INDEX_NONE && SelectedCoord.Y != INDEX_NONE;
	}

	bool HasDirection() const
	{
		return Direction != INDEX_NONE;
	}

	bool IsSelectableCoord(const FHexOffsetCoord& Coord) const
	{
		return SelectableCoords.Contains(Coord);
	}

	ABattleCharacterBase* GetPrimaryTargetCharacter() const
	{
		return TargetCharacters.IsValidIndex(0) ? TargetCharacters[0] : nullptr;
	}

	void Reset()
	{
		bCanConfirm = false;
		OriginCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
		OriginWorldLocation = FVector::ZeroVector;
		SelectableCoords.Empty();
		SelectedCoord = FHexOffsetCoord(INDEX_NONE, INDEX_NONE);
		SelectedWorldLocation = FVector::ZeroVector;
		AimWorldLocation = FVector::ZeroVector;
		Direction = INDEX_NONE;
		TargetCharacters.Empty();
	}
};
