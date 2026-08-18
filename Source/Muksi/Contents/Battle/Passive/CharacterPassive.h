// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Data/BattlePhase.h"
#include "UObject/Object.h"
#include "CharacterPassive.generated.h"

class ABattleCharacterBase;
class UCharacterPassiveComponent;


USTRUCT(BlueprintType)
struct MUKSI_API FPassiveTextLine
{
	GENERATED_BODY()
	TArray<FText> Text;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPassiveActive, UTexture2D*, CharacterPortrait, FText, CharacterName);

UCLASS(Abstract, Blueprintable)
class MUKSI_API UCharacterPassive : public UObject
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;

	virtual void InitializePassive(ABattleCharacterBase* InOwner, UCharacterPassiveComponent* InOwnerComponent);
	void Execute(EBattlePhase OldPhase, EBattlePhase NewPhase, bool bAllowDeferredCompletion);

	UFUNCTION(BlueprintCallable, Category = "Passive|Execution")
	void CompleteExecution();

	ABattleCharacterBase* GetOwnerCharacter() const
	{
		return OwnerCharacter.Get();
	}


	int32 GetPriority() const
	{
		return Priority;
	}

	bool IsEnabled() const
	{
		return bEnabled;
	}

	FText GetPassiveName() const{return PassiveName;}
	TArray<FPassiveTextLine> GetPassiveDescription() const{return PassiveDescriptions;}

	UPROPERTY(BlueprintAssignable, Category = "Passive|Event")
	FOnPassiveActive OnPassiveActive;


protected:
	virtual void HandleExecution(EBattlePhase OldPhase, EBattlePhase NewPhase, bool bAllowDeferredCompletion);
	virtual void HandleBattlePhaseChanged(EBattlePhase OldPhase, EBattlePhase NewPhase);

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Passive")
	TObjectPtr<ABattleCharacterBase> OwnerCharacter = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UCharacterPassiveComponent> OwnerComponent = nullptr;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Passive")
	TObjectPtr<UTexture2D> PassiveImage;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Passive")
	FText PassiveName;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category = "Passive",meta = (MultiLine = true))
	TArray<FPassiveTextLine>PassiveDescriptions;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Passive" )
	int32 Priority = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Passive")
	bool bEnabled = true;

private:
	bool bExecutionActive = false;
};
