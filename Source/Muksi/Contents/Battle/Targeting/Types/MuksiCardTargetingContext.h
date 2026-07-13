#pragma once

#include "CoreMinimal.h"
#include "MuksiCardTargetingContext.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UMuksiBattleCardDataAsset;

USTRUCT(BlueprintType)
struct FMuksiCardTargetingContext
{
	GENERATED_BODY()

	// 현재 카드를 사용하는 캐릭터.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	// 현재 선택된 카드.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> Card = nullptr;

	// 타기팅 계산에 사용하는 그리드 매니저.
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	// 현재 마우스로 가리키는 그리드 좌표.
	UPROPERTY(BlueprintReadOnly)
	FIntPoint HoveredCoord = FIntPoint(INDEX_NONE, INDEX_NONE);

	// 현재 마우스가 가리키는 월드 위치.
	UPROPERTY(BlueprintReadOnly)
	FVector AimWorldLocation = FVector::ZeroVector;

	// 현재 타기팅 후보로 계산된 캐릭터 목록.
	UPROPERTY(BlueprintReadOnly)
	TArray<TObjectPtr<ABattleCharacterBase>> CandidateCharacters;

	// 현재 유효한 호버 좌표가 있는지 반환한다.
	bool HasHoveredCoord() const
	{
		return HoveredCoord.X != INDEX_NONE && HoveredCoord.Y != INDEX_NONE;
	}

	// 첫 번째 타기팅 후보 캐릭터를 반환한다.
	ABattleCharacterBase* GetPrimaryCandidateCharacter() const
	{
		return CandidateCharacters.IsValidIndex(0) ? CandidateCharacters[0] : nullptr;
	}
};