// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/Enemy/AI/CardSelectStrategyBase/EnemyCardSelectStrategyBase.h"

#include "Muksi/Contents/Battle/Grid/CardRange/CardRangeDataAssetBase.h"
#include "Muksi/Contents/Battle/CharacterDataBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"


FEnemyCardSelectResult UEnemyCardSelectStrategyBase::SelectCardForExchange_Implementation(UCharacterDataBase* EnemyData,
                                                                                          ABattleGridManager* GridManager, const FIntPoint& EnemyCoord, const FIntPoint& PlayerCoord)
{
	FEnemyCardSelectResult BestResult;

	if (!EnemyData)
	{
		return BestResult;
	}
	if (!GridManager)
	{
		return BestResult;
	}

	TArray<UMuksiBattleCardDataAsset*> Deck = EnemyData->GetCharacterDeck();
	if (Deck.Num() <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Enemy deck is empty"));
		return BestResult;
	}
	//일단 첫번째 카드 사용
	UMuksiBattleCardDataAsset* Card = Deck[0];
	
	TArray<FIntPoint> CandidateCoords = GetCandidateCoords(
			EnemyData,
			Card,
			GridManager,
			EnemyCoord,
			PlayerCoord
		);
	//일단 첫번째 가능 위치 사용
	FIntPoint SelectedCoord = CandidateCoords[0];
	BestResult.SelectedCoordArray = Card->AttackType.RangeDataAsset->GetRangeCoords(GridManager, SelectedCoord, 0);
	BestResult.SelectedCard = Card;
	EnemyData->RemoveCard(Card);

	//점수 제 로 좋은 카드 구하는 코드 <- Base 단계에선 필요없음
	/*float BestScore = TNumericLimits<float>::Lowest();

	for (UMuksiBattleCardDataAsset* Card : Deck)
	{
		if (!Card)
		{
			continue;
		}

		
		for (const FIntPoint& CandidateCoord : CandidateCoords)
		{
			const float Score = EvaluateCardCoord(
				Card,
				CandidateCoord,
				PlayerCoord,
				GridManager
			);

			if (!BestResult.bValid || Score > BestScore)
			{
				BestScore = Score;
				BestResult.SelectedCard = Card;
				BestResult.SelectedCoord = CandidateCoord;
				BestResult.bValid = true;
			}
		}
	}*/
	//몰라
	/*if (BestResult.bValid && BestResult.SelectedCard)
	{
		EnemyData->RemoveCard(BestResult.SelectedCard);
	}*/

	return BestResult;
}

TArray<FIntPoint> UEnemyCardSelectStrategyBase::GetCandidateCoords(UCharacterDataBase* EnemyData,
	UMuksiBattleCardDataAsset* Card, ABattleGridManager* GridManager, const FIntPoint& EnemyCoord, const FIntPoint& PlayerCoord)
{
	TArray<FIntPoint> Result;

	if (!EnemyData || !Card || !GridManager)
	{
		return Result;
	}
	//일단 플레이어 위치 선택 -> Base에서 자세한 단계는 생략
	Result.Add(PlayerCoord);

	// 카드의 선택 가능 좌표 범위를 가져오는 부분
	// 실제 변수명은 네 카드 데이터 구조에 맞춰야 함.
	/*if (Card->CardRange)
	{
		Result = Card->CardRange->GetRangeCoords(
			GridManager,
			EnemyCoord,
			0
		);
	}*/

	return Result;
}

float UEnemyCardSelectStrategyBase::EvaluateCardCoord(UMuksiBattleCardDataAsset* Card, const FIntPoint& CandidateCoord,
	const FIntPoint& PlayerCoord, ABattleGridManager* GridManager)
{
	if (!Card || !GridManager)
	{
		return 0.f;
	}

	float Score = 0.f;

	// 플레이어 위치를 직접 선택할 수 있으면 높은 점수
	if (CandidateCoord == PlayerCoord)
	{
		Score += 100.f;
	}

	// 플레이어와 가까운 좌표일수록 선호
	/*const int32 Distance = GridManager->GetDistance(CandidateCoord, PlayerCoord);
	Score -= Distance * 10.f;*/

	return Score;
}
