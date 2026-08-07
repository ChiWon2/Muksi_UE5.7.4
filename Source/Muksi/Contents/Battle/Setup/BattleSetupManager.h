#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "BattleSetupManager.generated.h"

class ABattleCharacterBase;
class ABattleCharacter_Player;
class ABattleCharacter_Enemy;
class ABattleGridManager;
class ABattleManager;
class UMuksiCharacterDataAsset;

/**
 * Ready 단계의 전투 데이터 준비, 캐릭터 생성, 초기 배치와 사망 이벤트 연결을 담당한다.
 * Phase 순서는 결정하지 않으며 Setup 작업 완료만 BattleManager에 통지한다.
 */
UCLASS()
class MUKSI_API ABattleSetupManager : public AActor
{
    GENERATED_BODY()

public:
    ABattleSetupManager();

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    bool TryBindBattleFlow();
    void BindBattleFlowDeferred();

    UFUNCTION()
    void HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase);

    bool PrepareReadyData();
    bool PrepareReadyEnd();
    void ResolveEnemyCharacterData();
    bool CreateBattleCharacters();
    void BindBattleEndEvents();
    void UnbindBattleEndEvents();

    UFUNCTION()
    void HandleBattleCharacterDead(ABattleCharacterBase* DeadCharacter);

private:
    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    UPROPERTY(EditAnywhere, Category = "Battle|Setup|Grid")
    TObjectPtr<ABattleGridManager> BattleGridManager = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Setup|Character", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMuksiCharacterDataAsset> PlayerCharacterDataAsset = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Battle|Setup|Character", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UMuksiCharacterDataAsset> EnemyCharacterDataAsset = nullptr;

    UPROPERTY(EditAnywhere, Category = "Battle|Setup|Character")
    FHexOffsetCoord StartPlayerCoord = FHexOffsetCoord(1, 2);

    UPROPERTY(EditAnywhere, Category = "Battle|Setup|Character")
    FHexOffsetCoord StartEnemyCoord = FHexOffsetCoord(3, 2);

    FTimerHandle BattleFlowBindingTimerHandle;
};
