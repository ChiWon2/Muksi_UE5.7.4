#include "Muksi/Contents/Battle/Sequence/Executions/MuksiBattleMainEffectExecution.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardEffectData.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Sequence/Executions/MuksiBattleHitReactionExecution.h"

void UMuksiBattleMainEffectExecution::Execute(
	const FMuksiBattleExecutionContext& Context,
	FMuksiBattleExecutionFinished OnFinished
)
{
	if (!Context.IsValidContext())
	{
		FinishExecution(OnFinished);
		return;
	}

	const FMuksiBattleCardAttackTypeData& EffectData =
		Context.Card->AttackType;

	switch (EffectData.AttackType)
	{
	case EMuksiBattleCardAttackType::Rush:
	case EMuksiBattleCardAttackType::RangeAttack:
		ExecuteDamage(Context);
		break;

	case EMuksiBattleCardAttackType::Heal:
		ExecuteHeal(Context);
		break;

	case EMuksiBattleCardAttackType::Move:
		ExecuteMove(Context);
		break;

	case EMuksiBattleCardAttackType::Defense:
		/**
		 * TODO:
		 * 방어도 또는 Guard 시스템 연결 시 구현
		 */
		break;

	case EMuksiBattleCardAttackType::None:
	default:
		break;
	}

	/**
	 * ExecuteDamage() 안에서 HitReactionExecution 요청이 발생하면
	 * BattleSequenceManager의 Pending이 먼저 증가한다.
	 *
	 * 그 후 MainEffectExecution이 완료되면서
	 * MainEffectExecution 자신의 Pending만 감소한다.
	 */
	FinishExecution(OnFinished);
}

void UMuksiBattleMainEffectExecution::ExecuteDamage(
	const FMuksiBattleExecutionContext& Context
)
{
	if (!IsValid(Context.BattleGridManager))
	{
		return;
	}

	if (!IsValid(Context.Card))
	{
		return;
	}

	const int32 DamageValue =
		Context.Card->AttackType.Value;

	if (DamageValue <= 0)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"[MainEffectExecution] "
				"DamageValue must be greater than zero. Value=%d"
			),
			DamageValue
		);

		return;
	}

	for (const FIntPoint& TargetPoint : Context.TargetPoints)
	{
		const FBattleGridCell* Cell =
			Context.BattleGridManager->GetCell(TargetPoint);

		if (!Cell)
		{
			continue;
		}

		if (!IsValid(Cell->OccupyingActor))
		{
			continue;
		}

		ABattleCharacterBase* TargetCharacter =
			Cast<ABattleCharacterBase>(
				Cell->OccupyingActor
			);

		if (!IsValid(TargetCharacter))
		{
			continue;
		}

		if (TargetCharacter == Context.Attacker)
		{
			continue;
		}

		const int32 OldHP =
			TargetCharacter->GetCurrentHP();

		if (OldHP <= 0)
		{
			/**
			 * 이미 사망했거나 유효한 HP를 얻지 못한 대상에게는
			 * 추가 Damage 및 HitReaction을 실행하지 않는다.
			 */
			continue;
		}

		const int32 RequestedNewHP =
			OldHP - DamageValue;

		TargetCharacter->SetCurrentHP(
			RequestedNewHP
		);

		/**
		 * SetCurrentHP 내부에서 Clamp가 적용될 가능성을 고려해
		 * 실제 저장된 HP를 다시 가져온다.
		 */
		const int32 NewHP =
			TargetCharacter->GetCurrentHP();

		const int32 AppliedDamage =
			FMath::Max(
				0,
				OldHP - NewHP
			);

		UE_LOG(
			LogTemp,
			Log,
			TEXT(
				"[MainEffectExecution] "
				"Damage Requested=%d Applied=%d Target=%s "
				"OldHP=%d NewHP=%d"
			),
			DamageValue,
			AppliedDamage,
			*GetNameSafe(TargetCharacter),
			OldHP,
			NewHP
		);

		/**
		 * 실제로 HP가 감소한 경우에만 HitReaction을 요청한다.
		 *
		 * 방어, 회피, 무효화 등이 나중에 추가돼
		 * AppliedDamage가 0이 되면 HitReaction도 발생하지 않는다.
		 */
		if (AppliedDamage > 0)
		{
			RequestHitReaction(
				Context,
				TargetCharacter
			);
		}
	}
}

void UMuksiBattleMainEffectExecution::ExecuteHeal(
	const FMuksiBattleExecutionContext& Context
) const
{
	if (!IsValid(Context.Attacker))
	{
		return;
	}

	if (!IsValid(Context.Card))
	{
		return;
	}

	const int32 HealValue =
		Context.Card->AttackType.Value;

	if (HealValue <= 0)
	{
		return;
	}

	const int32 OldHP =
		Context.Attacker->GetCurrentHP();

	const int32 RequestedNewHP =
		OldHP + HealValue;

	Context.Attacker->SetCurrentHP(
		RequestedNewHP
	);

	const int32 NewHP =
		Context.Attacker->GetCurrentHP();

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"[MainEffectExecution] "
			"Heal Requested=%d Target=%s OldHP=%d NewHP=%d"
		),
		HealValue,
		*GetNameSafe(Context.Attacker),
		OldHP,
		NewHP
	);
}

void UMuksiBattleMainEffectExecution::ExecuteMove(
	const FMuksiBattleExecutionContext& Context
) const
{
	if (!IsValid(Context.Attacker))
	{
		return;
	}

	if (!IsValid(Context.BattleGridManager))
	{
		return;
	}

	const FIntPoint TargetPoint =
		Context.GetMainTargetPoint();

	if (
		TargetPoint.X == INDEX_NONE
		|| TargetPoint.Y == INDEX_NONE
		)
	{
		return;
	}

	Context.BattleGridManager->MoveActorOnGrid(
		Context.Attacker,
		Context.Attacker->GetCharacterPosition(),
		TargetPoint
	);

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"[MainEffectExecution] "
			"Move %s to (%d, %d)"
		),
		*GetNameSafe(Context.Attacker),
		TargetPoint.X,
		TargetPoint.Y
	);
}

void UMuksiBattleMainEffectExecution::RequestHitReaction(
	const FMuksiBattleExecutionContext& Context,
	ABattleCharacterBase* DamagedTarget
) const
{
	if (!IsValid(DamagedTarget))
	{
		return;
	}

	if (!Context.CanRequestSystemExecution())
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT(
				"[MainEffectExecution] "
				"RequestSystemExecution is not bound. Target=%s"
			),
			*GetNameSafe(DamagedTarget)
		);

		return;
	}

	/**
	 * 기존 Context를 복사해서 공격자, 카드, Grid, Notify 정보는 유지하고
	 * HitReaction의 직접 대상만 설정한다.
	 */
	FMuksiBattleExecutionContext HitReactionContext =
		Context;

	HitReactionContext.TargetCharacter =
		DamagedTarget;

	Context.RequestSystemExecution.Execute(
		UMuksiBattleHitReactionExecution::StaticClass(),
		HitReactionContext
	);
}