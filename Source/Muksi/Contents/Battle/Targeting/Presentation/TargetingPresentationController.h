#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TargetingPresentationController.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UBattleTargetingSession;
struct FResolvedTargeting;
struct FTargetingCardData;
struct FTargetingPhasePresentationSettings;

/**
 * Indicator와 Preview Session의 생성/정리를 한 경로로 묶는다.
 * BattleManager는 표시 시점만 결정하고 실제 수명 정리는 이 객체에 위임한다.
 */
UCLASS()
class MUKSI_API UTargetingPresentationController : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(ABattleGridManager* InGridManager);
	bool AddResolvedStepPreview(
		ABattleCharacterBase* SourceCharacter,
		const FTargetingCardData& CardData,
		const FResolvedTargeting& ResolvedTargeting,
		int32 StepIndex,
		const FTargetingPhasePresentationSettings& PresentationSettings,
		bool bEnemyStyle);
	void ClearExecutionPreview();
	void ClearSession(TObjectPtr<UBattleTargetingSession>& Session);
	void ClearGridPresentation();
	void ClearAll(TObjectPtr<UBattleTargetingSession>& PlayerSession, TObjectPtr<UBattleTargetingSession>& EnemySession);

private:
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UBattleTargetingSession>> ExecutionPreviewSessions;
};
