// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleCardComponent.generated.h"

class UMuksiBattleCardDataAsset;

USTRUCT(BlueprintType)
struct FBattleCardInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid InstanceId;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UMuksiBattleCardDataAsset> CardData = nullptr;

public:
	FBattleCardInstance()
		: InstanceId(FGuid::NewGuid())
	{
	}

	explicit FBattleCardInstance(UMuksiBattleCardDataAsset* InCardData)
		: InstanceId(FGuid::NewGuid())
		, CardData(InCardData)
	{
	}

	bool IsValid() const
	{
		return CardData != nullptr;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUKSI_API UBattleCardComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBattleCardComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
public:
	void Initialize(const TArray<UMuksiBattleCardDataAsset*>& InDeck);
	
	const TArray<FBattleCardInstance>& GetCurrentHand() const
	{
		return CurrentHand;
	}

	const TArray<FBattleCardInstance>& GetCommittedCards() const
	{
		return CommittedCards;
	}

	int32 GetCurrentHandCount() const
	{
		return CurrentHand.Num();
	}
	
	const TArray<TObjectPtr<UMuksiBattleCardDataAsset>>& GetFullDeck() const
	{
		return FullDeck;
	}
	
	const FBattleCardInstance* FindHandCardById(const FGuid& InstanceId) const;
	
	
	bool CommitCard(const FGuid& InstanceId);
	bool ReturnCommittedCard(const FGuid& InstanceId);
	
	bool ReplaceHandCard(const FGuid& InstanceId, UMuksiBattleCardDataAsset* NewCardData);
	void ConsumeCommittedCards();
	bool RefillHandIfEmpty();
private:
	
	
	UPROPERTY(Transient)
	TArray<TObjectPtr<UMuksiBattleCardDataAsset>> FullDeck;//전체 덱

	UPROPERTY(Transient)
	TArray<FBattleCardInstance> CurrentHand;//손패 인스턴스

	UPROPERTY(Transient)
	TArray<FBattleCardInstance> CommittedCards;//ExchangeSlot에 넣은 카드 인스턴스
		
};
