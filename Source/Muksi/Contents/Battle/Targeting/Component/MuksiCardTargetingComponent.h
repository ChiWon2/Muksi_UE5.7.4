#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Muksi/Contents/Battle/Targeting/Types/MuksiCardTargetingContext.h"
#include "Muksi/Contents/Battle/Targeting/Types/MuksiCardTargetingResult.h"
#include "MuksiCardTargetingComponent.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;
class UMuksiBattleCardDataAsset;
class UMuksiCardAreaPattern;
class UMuksiCardTargetingPolicy;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMuksiCardTargetingResultUpdated, const FMuksiCardTargetingResult&, TargetingResult);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMuksiCardTargetingEnded);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUKSI_API UMuksiCardTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMuksiCardTargetingComponent();

public:
	// 타기팅 계산에 사용할 그리드 매니저를 설정한다.
	void InitializeTargetingComponent(ABattleGridManager* InGridManager);

	// 지정한 캐릭터와 카드로 타기팅을 시작한다.
	bool StartTargeting(ABattleCharacterBase* InSourceCharacter, UMuksiBattleCardDataAsset* InCard);

	// 현재 마우스 입력 Context를 기준으로 타기팅 결과를 갱신한다.
	void UpdateTargeting(const FMuksiCardTargetingContext& Context);

	// 현재 타기팅 결과를 확정하여 반환한다.
	bool ConfirmTargeting(FMuksiCardTargetingResult& OutResult);

	// 현재 진행 중인 타기팅을 취소하고 초기화한다.
	void CancelTargeting();

	// 현재 타기팅이 진행 중인지 반환한다.
	UFUNCTION(BlueprintPure, Category = "Battle|Targeting")
	bool IsTargeting() const { return bIsTargeting; }

	// 현재 계산된 타기팅 결과를 반환한다.
	UFUNCTION(BlueprintPure, Category = "Battle|Targeting")
	const FMuksiCardTargetingResult& GetCurrentResult() const { return CurrentResult; }

	// 현재 타기팅에 사용 중인 카드를 반환한다.
	UFUNCTION(BlueprintPure, Category = "Battle|Targeting")
	UMuksiBattleCardDataAsset* GetCurrentCard() const { return CurrentCard; }

	// 현재 카드를 사용하는 캐릭터를 반환한다.
	UFUNCTION(BlueprintPure, Category = "Battle|Targeting")
	ABattleCharacterBase* GetSourceCharacter() const { return SourceCharacter; }

public:
	// 타기팅 결과가 새로 계산되었을 때 호출된다.
	UPROPERTY(BlueprintAssignable, Category = "Battle|Targeting")
	FMuksiCardTargetingResultUpdated OnTargetingResultUpdated;

	// 타기팅이 확정되거나 취소되어 종료되었을 때 호출된다.
	UPROPERTY(BlueprintAssignable, Category = "Battle|Targeting")
	FMuksiCardTargetingEnded OnTargetingEnded;

private:
	// 현재 카드에 설정된 Policy와 Pattern 객체를 생성한다.
	bool CreateTargetingObjects();

	// 현재 입력 Context에 Component가 관리하는 공통 정보를 적용한다.
	FMuksiCardTargetingContext BuildRuntimeContext(const FMuksiCardTargetingContext& InputContext) const;

	// 현재 타기팅 관련 런타임 상태를 초기화한다.
	void ResetTargetingState();

private:
	// 타기팅 계산에 사용하는 그리드 매니저.
	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	// 현재 카드를 사용하는 캐릭터.
	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;

	// 현재 타기팅에 사용 중인 카드.
	UPROPERTY(Transient)
	TObjectPtr<UMuksiBattleCardDataAsset> CurrentCard = nullptr;

	// 현재 카드에 설정된 Targeting Policy 인스턴스.
	UPROPERTY(Transient)
	TObjectPtr<UMuksiCardTargetingPolicy> TargetingPolicy = nullptr;

	// 현재 카드에 설정된 Area Pattern 인스턴스.
	UPROPERTY(Transient)
	TObjectPtr<UMuksiCardAreaPattern> AreaPattern = nullptr;

	// 현재 계산된 타기팅 결과.
	UPROPERTY(Transient)
	FMuksiCardTargetingResult CurrentResult;

	// 현재 타기팅이 진행 중인지 나타낸다.
	UPROPERTY(Transient)
	bool bIsTargeting = false;
};