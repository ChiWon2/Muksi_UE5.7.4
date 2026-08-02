#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Targeting/Identity/TargetingIdentityProvider.h"
#include "BattleSimulationCharacter.generated.h"

class UMaterialInterface;

UCLASS()
class MUKSI_API ABattleSimulationCharacter : public ABattleCharacterBase, public ITargetingIdentityProvider
{
	GENERATED_BODY()

public:
	ABattleSimulationCharacter();

	void InitializeFromCharacter(const ABattleCharacterBase* SourceCharacter, UMaterialInterface* MaterialOverride = nullptr);

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleCharacterBase* GetSourceCharacter() const { return SourceCharacter; }

	virtual FName GetTargetingCharacterKey_Implementation() const override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Simulation")
	TObjectPtr<UMaterialInterface> SimulationMaterial;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;
};
