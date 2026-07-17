#pragma once

#include "CoreMinimal.h"
#include "TargetingInputContext.generated.h"

class ABattleCharacterBase;
/*
* 현재 프레임의 마우스 또는 AI 입력만 담는다.
*/
USTRUCT(BlueprintType)
struct FTargetingInputContext
{
	GENERATED_BODY()

	// 현재 마우스 또는 AI가 가리키는 그리드 좌표.
	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	FIntPoint HoveredCoord = FIntPoint(INDEX_NONE, INDEX_NONE);

	// 현재 마우스 또는 AI가 조준하는 월드 위치.
	UPROPERTY(BlueprintReadWrite, Category = "Targeting")
	FVector AimWorldLocation = FVector::ZeroVector;

	// 현재 입력 위치에서 선택 후보가 되는 캐릭터 목록.
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
		HoveredCoord = FIntPoint(INDEX_NONE, INDEX_NONE);
		AimWorldLocation = FVector::ZeroVector;
		CandidateCharacters.Empty();
	}
};