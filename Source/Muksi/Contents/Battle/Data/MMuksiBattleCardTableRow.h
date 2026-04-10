// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "MMuksiBattleCardTableRow.generated.h"

/**
 * 
 */
class UTexture2D;

USTRUCT(BlueprintType)
struct FMMuksiBattleCardTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	FName CardID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	FText CardName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card", meta = (MultiLine = true))
	FText CardDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Card")
	TObjectPtr<UTexture2D> CardTexture = nullptr;
};
