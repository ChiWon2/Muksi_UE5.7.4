#include "Muksi/Contents/Battle/Character/Simulation/BattleSimulationCharacter.h"

#include "Components/BoxComponent.h"

ABattleSimulationCharacter::ABattleSimulationCharacter()
{
	ClickCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABattleSimulationCharacter::InitializeFromCharacter(const ABattleCharacterBase* SourceCharacter)
{
	if (!IsValid(SourceCharacter))
	{
		return;
	}

	CopyBattleStateFrom(*SourceCharacter);
	SetActorTransform(SourceCharacter->GetActorTransform());
}
