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
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Notify Begin Mesh=%s Owner=%s Animation=%s NotifyKey=%s Duration=%.3f"), *GetNameSafe(MeshComp), MeshComp ? *GetNameSafe(MeshComp->GetOwner()) : TEXT("None"), *GetNameSafe(Animation), *FXNotifyKey.ToString(), TotalDuration);

	if (!MeshComp)
		return;

	ABattleCharacterBase* BattleCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner());
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Notify Resolve Character=%s FXComponent=%s NotifyMesh=%s BattleMesh=%s SameMesh=%s"), *GetNameSafe(BattleCharacter), BattleCharacter ? *GetNameSafe(BattleCharacter->GetBattleFXComponent()) : TEXT("None"), *GetNameSafe(MeshComp), BattleCharacter ? *GetNameSafe(BattleCharacter->GetBattleSkeletalMesh()) : TEXT("None"), BattleCharacter && MeshComp == BattleCharacter->GetBattleSkeletalMesh() ? TEXT("true") : TEXT("false"));
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
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Notify End Mesh=%s Owner=%s Animation=%s NotifyKey=%s"), *GetNameSafe(MeshComp), MeshComp ? *GetNameSafe(MeshComp->GetOwner()) : TEXT("None"), *GetNameSafe(Animation), *FXNotifyKey.ToString());

	if (!MeshComp)
		return;

	ABattleCharacterBase* BattleCharacter = Cast<ABattleCharacterBase>(MeshComp->GetOwner());
	UE_LOG(LogTemp, Warning, TEXT("[VFX] Notify Resolve Character=%s FXComponent=%s NotifyMesh=%s BattleMesh=%s SameMesh=%s"), *GetNameSafe(BattleCharacter), BattleCharacter ? *GetNameSafe(BattleCharacter->GetBattleFXComponent()) : TEXT("None"), *GetNameSafe(MeshComp), BattleCharacter ? *GetNameSafe(BattleCharacter->GetBattleSkeletalMesh()) : TEXT("None"), BattleCharacter && MeshComp == BattleCharacter->GetBattleSkeletalMesh() ? TEXT("true") : TEXT("false"));
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
