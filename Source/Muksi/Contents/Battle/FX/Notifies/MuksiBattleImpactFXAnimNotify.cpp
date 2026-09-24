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

	UE_LOG(LogTemp, Warning, TEXT("[BattleFX] Impact Notify Mesh=%s Owner=%s FXKey=%s"), *GetNameSafe(MeshComp), MeshComp ? *GetNameSafe(MeshComp->GetOwner()) : TEXT("None"), *FXKey.ToString());

	if (!MeshComp)
		return;

	ABattleCharacterBase* BattleCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner());
	if (!BattleCharacter)
		return;

	if (UMuksiBattleFXComponent* BattleFXComponent = BattleCharacter->GetBattleFXComponent())
		BattleFXComponent->PlayImpactFX(FXKey);
}

FString UMuksiBattleImpactFXAnimNotify::GetNotifyName_Implementation() const
{
	if (FXKey.IsNone())
		return TEXT("ImpactFX");

	return FString::Printf(TEXT("ImpactFX_%s"), *FXKey.ToString());
}
