#include "MuksiBattleImpactFXAnimNotify.h"

#include "Components/SkeletalMeshComponent.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/FX/MuksiBattleFXComponent.h"

void UMuksiBattleImpactFXAnimNotify::Notify(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (!MeshComp)
		return;

	ABattleCharacterBase* BattleCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner());
	if (!BattleCharacter)
		return;

	if (UMuksiBattleFXComponent* BattleFXComponent = BattleCharacter->GetBattleFXComponent())
		BattleFXComponent->PlayImpactFX(FXNotifyKey);
}

FString UMuksiBattleImpactFXAnimNotify::GetNotifyName_Implementation() const
{
	if (FXNotifyKey.IsNone())
		return TEXT("ImpactFX");

	return FString::Printf(TEXT("ImpactFX_%s"), *FXNotifyKey.ToString());
}
