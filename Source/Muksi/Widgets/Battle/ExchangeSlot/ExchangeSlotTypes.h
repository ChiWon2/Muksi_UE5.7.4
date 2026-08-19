#pragma once

#include "CoreMinimal.h"
#include "ExchangeSlotTypes.generated.h"

class UWidget_BattleCardBase;
class UMuksiBattleCardDataAsset;
class ABattleCharacterBase;

//카드 위치 담은 구조체
struct FReleasedExchangeCard
{
	UWidget_BattleCardBase* CardWidget = nullptr;
	FVector2D AbsoluteBottomCenter = FVector2D::ZeroVector;
};

USTRUCT(BlueprintType)
struct FCardEquipSlotData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	int32 SlotIndex = -1;

	UPROPERTY(BlueprintReadOnly)
	int32 ExchangeNumber = 0;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> CardData = nullptr;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;


	UPROPERTY(BlueprintReadOnly)
	bool bConfirmed = false;

	bool IsValidCard() const
	{
		return CardData != nullptr && SourceCharacter != nullptr;
	}
};