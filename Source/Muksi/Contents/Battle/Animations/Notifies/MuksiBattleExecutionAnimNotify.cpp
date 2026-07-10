#include "Muksi/Contents/Battle/Animations/Notifies/MuksiBattleExecutionAnimNotify.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Actor.h"
#include "Muksi/Contents/Battle/Animations/MuksiBattleAnimationComponent.h"

void UMuksiBattleExecutionAnimNotify::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner)
	{
		return;
	}

	UMuksiBattleAnimationComponent* BattleAnimationComponent = Owner->FindComponentByClass<UMuksiBattleAnimationComponent>();

	if (!BattleAnimationComponent)
	{
		return;
	}

	BattleAnimationComponent->HandleBattleExecutionNotify(NotifyKey);
}

FString UMuksiBattleExecutionAnimNotify::GetNotifyName_Implementation() const
{
	return NotifyKey.IsNone()
		? TEXT("BattleExecution")
		: FString::Printf(TEXT("BattleExecution_%s"), *NotifyKey.ToString());
}