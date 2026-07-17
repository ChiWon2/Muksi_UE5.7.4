// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/Character/Enemy/AI/CardSelectStrategyBase/EnemyCardSelectStrategyBase.h"

#include "Muksi/Contents/Battle/Grid/CardRange/CardRangeDataAssetBase.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"


FEnemyCardSelectResult UEnemyCardSelectStrategyBase::SelectCardForExchange_Implementation(FCharacterData EnemyData,
                                                                                          ABattleGridManager* GridManager, const FIntPoint& EnemyCoord, const FIntPoint& PlayerCoord)
{
	FEnemyCardSelectResult BestResult;

	if (!EnemyData.IsValid())
	{
		return BestResult;
	}
	if (!GridManager)
	{
		return BestResult;
	}

	TArray<UMuksiBattleCardDataAsset*> Deck = EnemyData.BattleDeck;
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
	EnemyData.BattleDeck.Remove(Card);

	return BestResult;
}

TArray<FIntPoint> UEnemyCardSelectStrategyBase::GetCandidateCoords(FCharacterData EnemyData,
	UMuksiBattleCardDataAsset* Card, ABattleGridManager* GridManager, const FIntPoint& EnemyCoord, const FIntPoint& PlayerCoord)
{
	TArray<FIntPoint> Result;

	if (!EnemyData.IsValid() || !Card || !GridManager)
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
