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

	UE_LOG(LogTemp, Warning, TEXT("[BattleFX] Trail NotifyBegin Mesh=%s Owner=%s FXKey=%s"), *GetNameSafe(MeshComp), MeshComp ? *GetNameSafe(MeshComp->GetOwner()) : TEXT("None"), *FXKey.ToString());

	if (!MeshComp)
		return;

	ABattleCharacterBase* BattleCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner());
	if (!BattleCharacter)
		return;

	if (UMuksiBattleFXComponent* BattleFXComponent = BattleCharacter->GetBattleFXComponent())
		BattleFXComponent->StartTrailFX(FXKey);
}

void UMuksiBattleTrailFXAnimNotifyState::NotifyEnd(
	USkeletalMeshComponent* MeshComp,
	UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	UE_LOG(LogTemp, Warning, TEXT("[BattleFX] Trail NotifyEnd Mesh=%s Owner=%s FXKey=%s"), *GetNameSafe(MeshComp), MeshComp ? *GetNameSafe(MeshComp->GetOwner()) : TEXT("None"), *FXKey.ToString());

	if (!MeshComp)
		return;

	ABattleCharacterBase* BattleCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner());
	if (!BattleCharacter)
		return;

	if (UMuksiBattleFXComponent* BattleFXComponent = BattleCharacter->GetBattleFXComponent())
		BattleFXComponent->StopTrailFX(FXKey);
}

FString UMuksiBattleTrailFXAnimNotifyState::GetNotifyName_Implementation() const
{
	if (FXKey.IsNone())
		return TEXT("TrailFX");

	return FString::Printf(TEXT("TrailFX_%s"), *FXKey.ToString());
}
