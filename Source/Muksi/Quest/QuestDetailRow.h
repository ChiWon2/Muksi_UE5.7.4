#pragma once
#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include"QuestReward.h"
#include"ObjectiveDetails.h"
#include "QuestDetailRow.generated.h"


USTRUCT(BlueprintType)
struct FQuestDetailRow : public FTableRowBase
{
	GENERATED_BODY()
	//QuestID = DataTable RowName
	 
	//UI Dislplayed QusetName 
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic", meta = (DisplayName = "Quest Name"))
	FText QuestName;
	//UI Dislplayed Quest Descrption
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Basic", meta = (MultiLine = "true"))
	FText Description;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite,Category = "Objectives",meta = (TitleProperty = "Description"))
	TArray<FObjectiveDetails> Objectives;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Reward")
	FQuestReward Reward;

	//UPROPERTY(EditAnywhere,BlueprintReadWrite)
	//int32 QuestTravelHour; // 퀘스트 지속 기간(시간Hour 단위).. 이 기간 끝나면 실패처리됨
};