#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Setup/AssetPreload/BattleAssetPreloadManager.h"
#include "BattleSetupManager.generated.h"

class ABattleCharacterBase;
class ABattleCharacter_Player;
class ABattleCharacter_Enemy;
class ABattleGridManager;
class ABattleManager;
class UMuksiCharacterDataAsset;
class UBattlePhaseTask;
class UBattlePhaseTaskContext;
class UBattleAssetPreloadManager;

/**
 * Ready 단계의 전투 데이터 준비, 캐릭터 생성, 초기 배치와 사망 이벤트 연결을 담당한다.
 * Phase 순서는 결정하지 않으며 Entry 단계에서 등록한 Task만 완료한다.
 */
UCLASS()
class MUKSI_API ABattleSetupManager : public AActor
{
    GENERATED_BODY()

public:
    ABattleSetupManager();
    bool InitializeBattleFlow(ABattleManager* InBattleManager, ABattleGridManager* InBattleGridManager);

    UFUNCTION(BlueprintPure, Category = "Battle|Preload")
    UBattleAssetPreloadManager* GetAssetPreloadManager() const { return AssetPreloadManager; }

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
    UFUNCTION()
    void HandlePhaseEntryRequested(EBattlePhase OldPhase, EBattlePhase NewPhase, UBattlePhaseTaskContext* TaskContext);

    bool PrepareReadyData(UBattlePhaseTask* Task);
    bool PrepareReadyEnd();
    bool ShouldHandlePhaseEntry(EBattlePhase Phase) const;
    void LoadEncounterEnemyCharacterData();
    bool CreateBattleCharacters();
    void FaceCharactersTowardEachOther(ABattleCharacterBase* PlayerCharacter, ABattleCharacterBase* EnemyCharacter) const;
    void BindBattleEndEvents();
    void UnbindBattleEndEvents();

    UFUNCTION()
    void HandleBattleCharacterDead(ABattleCharacterBase* DeadCharacter);

private:
    UPROPERTY(Transient)
    TObjectPtr<ABattleManager> BattleManager = nullptr;

    UPROPERTY(Transient)
    TObjectPtr<UBattleAssetPreloadManager> AssetPreloadManager = nullptr;

    UPROPERTY(EditAnywhere, Category = "Battle|Setup|Preload", meta = (ClampMin = "1.0"))
    float RenderPreparationTimeoutSeconds = 60.0f;

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
};
