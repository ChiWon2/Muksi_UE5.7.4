// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "MuksiSaveGame.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FPlayerWorldSaveData
{
	GENERATED_BODY()

public:
	/**
	 * 월드 복귀 시 플레이어를 다시 배치할 위치와 회전
	 */
	UPROPERTY(
		SaveGame,
		BlueprintReadOnly,
		Category = "Muksi|Save|Player"
	)
	FTransform PlayerTransform =
		FTransform::Identity;

	/**
	 * 전투 전/후 현재 체력
	 */
	UPROPERTY(
		SaveGame,
		BlueprintReadOnly,
		Category = "Muksi|Save|Player"
	)
	int32 CurrentHP = 0;

	/**
	 * 플레이어 최대 체력
	 */
	UPROPERTY(
		SaveGame,
		BlueprintReadOnly,
		Category = "Muksi|Save|Player"
	)
	int32 MaxHP = 0;
};

UCLASS()
class MUKSI_API UMuksiSaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UMuksiSaveGame();

public:

	//플레이어가 마지막으로 있던 월드 이름
	UPROPERTY(
		SaveGame,
		BlueprintReadOnly,
		Category = "Muksi|Save|World"
	)
	FName SavedWorldName = NAME_None;


	//월드 플레이어 관련 저장 데이터
	UPROPERTY(
		SaveGame,
		BlueprintReadOnly,
		Category = "Muksi|Save|Player"
	)
	FPlayerWorldSaveData PlayerWorldData;


	//테스트 용
	UPROPERTY(
		SaveGame,
		BlueprintReadWrite,
		Category = "Muksi|Save|Test"
	)
	int32 TestValue = 0;
};
