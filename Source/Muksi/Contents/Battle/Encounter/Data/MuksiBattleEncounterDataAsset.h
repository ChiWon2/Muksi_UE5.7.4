#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Muksi/Contents/Battle/Encounter/Data/MuksiBattleEncounterData.h"
#include "MuksiBattleEncounterDataAsset.generated.h"

// EncounterID를 Key로 사용하며 별도의 조회 캐시를 생성하지 않는다.
UCLASS(BlueprintType)
class MUKSI_API UMuksiBattleEncounterDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Encounter")
    TMap<FName, FMuksiBattleEncounterData> EncounterDatas;
};
