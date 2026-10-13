#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldBattleEncounterManager.generated.h"

class UMuksiBattleEncounterDataAsset;

// 유랑 레벨에 배치해 해당 월드의 Encounter DataAsset을 관리한다.
UCLASS()
class MUKSI_API AWorldBattleEncounterManager : public AActor
{
    GENERATED_BODY()

public:
    AWorldBattleEncounterManager();

    static AWorldBattleEncounterManager* Get(const UObject* WorldContextObject);

    UFUNCTION(BlueprintCallable, Category = "Encounter")
    bool StartEncounter(FName EncounterID);

private:
    // 이 액터가 존재하는 유랑 레벨의 Encounter 데이터만 소유한다.
    UPROPERTY(EditAnywhere, Category = "Encounter")
    TArray<TObjectPtr<UMuksiBattleEncounterDataAsset>> EncounterDataAssets;
};
