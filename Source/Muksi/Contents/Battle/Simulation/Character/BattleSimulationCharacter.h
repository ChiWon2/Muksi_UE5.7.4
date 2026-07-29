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

	void InitializeFromCharacter(const ABattleCharacterBase* SourceCharacter);

	UFUNCTION(BlueprintPure, Category = "Battle|Simulation")
	ABattleCharacterBase* GetSourceCharacter() const { return SourceCharacter; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Battle|Simulation")
	TObjectPtr<UMaterialInterface> SimulationMaterial;

	UPROPERTY(Transient)
	TObjectPtr<ABattleCharacterBase> SourceCharacter = nullptr;
};