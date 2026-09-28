#include "MuksiBattleTrailFXAnimNotifyState.h"

#include "Components/SkeletalMeshComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/FX/MuksiBattleFXComponent.h"

void UMuksiBattleTrailFXAnimNotifyState::NotifyBegin(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	float TotalDuration,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp)
		return;

	ABattleCharacterBase* BattleCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner());
	if (!BattleCharacter)
		return;

	if (UMuksiBattleFXComponent* BattleFXComponent = BattleCharacter->GetBattleFXComponent())
		BattleFXComponent->StartTrailFX(FXNotifyKey);
}

void UMuksiBattleTrailFXAnimNotifyState::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (!MeshComp)
		return;

	ABattleCharacterBase* BattleCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner());
	if (!BattleCharacter)
		return;

	if (UMuksiBattleFXComponent* BattleFXComponent = BattleCharacter->GetBattleFXComponent())
		BattleFXComponent->StopTrailFX(FXNotifyKey);
}

FString UMuksiBattleTrailFXAnimNotifyState::GetNotifyName_Implementation() const
{
	if (FXNotifyKey.IsNone())
		return TEXT("TrailFX");

	return FString::Printf(TEXT("TrailFX_%s"), *FXNotifyKey.ToString());
}
