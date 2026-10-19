#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BattleFloatingTextTypes.h"
#include "BattleFloatingTextManager.generated.h"

class UCanvasPanel;
class UWidget_BattleFloatingText;
class UBattleFloatingTextDataAsset;
class UBattleStatComponent;
class ABattleCharacterBase;

struct FBattleActiveFloatingText
{
	TWeakObjectPtr<ABattleCharacterBase> TargetCharacter;
	TWeakObjectPtr<UWidget_BattleFloatingText> Widget;
	float Height = 0.0f;
	FVector2D SpawnPosition = FVector2D::ZeroVector;
};

UCLASS()
class MUKSI_API UBattleFloatingTextManager : public UObject
{
	GENERATED_BODY()

public:
	void Initialize(UObject* WorldContext, UCanvasPanel* Canvas, UBattleFloatingTextDataAsset* DataAsset);
	void BindCharacters(ABattleCharacterBase* Player, ABattleCharacterBase* Enemy);
	void UnbindCharacters();

private:
	UFUNCTION()
	void HandleHPChangeApplied(float HPChange, FName StyleKey, ABattleCharacterBase* TargetCharacter);

	UPROPERTY(Transient)
	TObjectPtr<UObject> WorldContextObject;

	UPROPERTY(Transient)
	TObjectPtr<UCanvasPanel> FloatingTextCanvas;

	UPROPERTY(Transient)
	TObjectPtr<UBattleFloatingTextDataAsset> FloatingTextDataAsset;

	UPROPERTY(Transient)
	TObjectPtr<UBattleStatComponent> PlayerStats;

	UPROPERTY(Transient)
	TObjectPtr<UBattleStatComponent> EnemyStats;

	TArray<FBattleActiveFloatingText> ActiveFloatingTexts;
};
