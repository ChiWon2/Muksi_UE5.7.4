#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "BattleSimulationCharacter.generated.h"

class UMaterialInterface;

UCLASS()
class MUKSI_API ABattleSimulationCharacter : public ABattleCharacterBase
{
	GENERATED_BODY()

public:
	ABattleSimulationCharacter();

	void InitializeFromCharacter(const ABattleCharacterBase* SourceCharacter, UMaterialInterface* MaterialOverride = nullptr);

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleCharacterBase* GetSourceCharacter() const { return SourceCharacter; }

	virtual FName GetTargetingCharacterKey() const override;

private:
	void ResetActiveState();
	void CopyActorState(const ABattleCharacterBase& InSourceCharacter);
	void CopyMeshPresentation(const ABattleCharacterBase& InSourceCharacter, UMaterialInterface* MaterialOverride);
	void CopyAnimationPresentation(const ABattleCharacterBase& InSourceCharacter);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Simulation")
	TObjectPtr<UMaterialInterface> SimulationMaterial;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;
};
