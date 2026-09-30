#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecution.h"

#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Execution/Executions/HitReaction/HitReactionExecutionData.h"
#include "Muksi/Contents/Battle/FX/MuksiBattleFXComponent.h"


UHitReactionExecution::UHitReactionExecution()
{
	bPresentationOnly = true;
}

void UHitReactionExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	CachedOnFinished = OnFinished;
	bAnimationFinished = false;
	bFXFinished = true;

	ABattleCharacterBase* TargetCharacter = Context.ExecutionTarget.Get();
	if (!TargetCharacter)
		TargetCharacter = Context.Attacker.Get();

	if (!TargetCharacter)
	{
		FinishHitReaction();
		return;
	}

	const FHitReactionExecutionData* HitReactionData = Context.GetExecutionData<FHitReactionExecutionData>();
	const FName AnimKey = HitReactionData && !HitReactionData->AnimKey.IsNone() ? HitReactionData->AnimKey : TEXT("HitReaction");
	const float PlayRate = HitReactionData ? HitReactionData->PlayRate : 1.0f;

	if (HitReactionData && !HitReactionData->FXDataAssetKey.IsNone())
	{
		if (UMuksiBattleFXComponent* BattleFXComponent = TargetCharacter->GetBattleFXComponent())
		{
			if (HitReactionData->bWaitForFX)
			{
				bFXFinished = false;

				FSimpleDelegate CompletionDelegate;
				CompletionDelegate.BindUObject(this, &UHitReactionExecution::HandleFXFinished);
				BattleFXComponent->PlayOneShotFXByDataAssetKey(HitReactionData->FXDataAssetKey, MoveTemp(CompletionDelegate));
			}
			else
			{
				BattleFXComponent->PlayImpactFXByDataAssetKey(HitReactionData->FXDataAssetKey);
			}
		}
	}

	TargetAnimationComponent = TargetCharacter->FindComponentByClass<UMuksiBattleAnimationComponent>();
	if (!TargetAnimationComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("[HitReactionExecution] AnimationComponent not found. Target=%s"), *GetNameSafe(TargetCharacter));
		bAnimationFinished = true;
		TryFinishHitReaction();
		return;
	}

	PlayingMontage = TargetAnimationComponent->FindMontage(AnimKey);
	if (!PlayingMontage)
	{
		bAnimationFinished = true;
		TryFinishHitReaction();
		return;
	}

	TargetAnimationComponent->OnBattleAnimationFinished.AddUniqueDynamic(this, &UHitReactionExecution::HandleHitReactionFinished);

	if (!TargetAnimationComponent->PlayBattleAnimation(AnimKey, PlayRate))
	{
		TargetAnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UHitReactionExecution::HandleHitReactionFinished);
		PlayingMontage = nullptr;
		bAnimationFinished = true;
		TryFinishHitReaction();
	}
}

void UHitReactionExecution::HandleHitReactionFinished(UAnimMontage* Montage, bool bInterrupted)
{
	static_cast<void>(bInterrupted);

	if (Montage != PlayingMontage)
		return;

	bAnimationFinished = true;
	TryFinishHitReaction();
}

void UHitReactionExecution::HandleFXFinished()
{
	bFXFinished = true;
	TryFinishHitReaction();
}

void UHitReactionExecution::TryFinishHitReaction()
{
	if (!bAnimationFinished || !bFXFinished)
		return;

	FinishHitReaction();
}

void UHitReactionExecution::FinishHitReaction()
{
	if (IsExecutionFinished())
		return;

	if (TargetAnimationComponent)
		TargetAnimationComponent->OnBattleAnimationFinished.RemoveDynamic(this, &UHitReactionExecution::HandleHitReactionFinished);

	TargetAnimationComponent = nullptr;
	PlayingMontage = nullptr;
	FinishExecution(CachedOnFinished);
}

const UScriptStruct* UHitReactionExecution::GetExecutionDataStruct() const
{
	return FHitReactionExecutionData::StaticStruct();
}
