#include "Muksi/Contents/Battle/Targeting/Component/MuksiCardTargetingComponent.h"

#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/Targeting/Pattern/MuksiCardAreaPattern.h"
#include "Muksi/Contents/Battle/Targeting/Policy/MuksiCardTargetingPolicy.h"

UMuksiCardTargetingComponent::UMuksiCardTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMuksiCardTargetingComponent::InitializeTargetingComponent(ABattleGridManager* InGridManager)
{
	GridManager = InGridManager;
}

bool UMuksiCardTargetingComponent::StartTargeting(ABattleCharacterBase* InSourceCharacter, UMuksiBattleCardDataAsset* InCard)
{
	ResetTargetingState();

	if (!InSourceCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] SourceCharacter is null."));
		return false;
	}

	if (!InCard)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] Card is null."));
		return false;
	}

	if (!GridManager)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] GridManager is null."));
		return false;
	}

	SourceCharacter = InSourceCharacter;
	CurrentCard = InCard;

	if (!CreateTargetingObjects())
	{
		ResetTargetingState();
		return false;
	}

	bIsTargeting = true;
	CurrentResult.Reset();

	return true;
}

void UMuksiCardTargetingComponent::UpdateTargeting(const FMuksiCardTargetingContext& Context)
{
	if (!bIsTargeting)
	{
		return;
	}

	if (!TargetingPolicy)
	{
		return;
	}

	if (!AreaPattern)
	{
		return;
	}

	if (!CurrentCard)
	{
		return;
	}

	const FMuksiCardTargetingContext RuntimeContext = BuildRuntimeContext(Context);

	TargetingPolicy->BuildTargetingResult(RuntimeContext, CurrentCard->TargetingData, CurrentResult);

	if (CurrentResult.bCanConfirm)
	{
		AreaPattern->ApplyAreaPattern(RuntimeContext, CurrentCard->AreaPatternData, CurrentResult);
	}

	OnTargetingResultUpdated.Broadcast(CurrentResult);
}

bool UMuksiCardTargetingComponent::ConfirmTargeting(FMuksiCardTargetingResult& OutResult)
{
	if (!bIsTargeting)
	{
		return false;
	}

	if (!CurrentResult.bCanConfirm)
	{
		return false;
	}

	OutResult = CurrentResult;

	ResetTargetingState();
	OnTargetingEnded.Broadcast();

	return true;
}

void UMuksiCardTargetingComponent::CancelTargeting()
{
	if (!bIsTargeting)
	{
		return;
	}

	ResetTargetingState();
	OnTargetingEnded.Broadcast();
}

bool UMuksiCardTargetingComponent::CreateTargetingObjects()
{
	if (!CurrentCard)
	{
		return false;
	}

	if (!CurrentCard->TargetingPolicyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] TargetingPolicyClass is null. Card=%s"), *GetNameSafe(CurrentCard));
		return false;
	}

	if (!CurrentCard->AreaPatternClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] AreaPatternClass is null. Card=%s"), *GetNameSafe(CurrentCard));
		return false;
	}

	TargetingPolicy = NewObject<UMuksiCardTargetingPolicy>(this, CurrentCard->TargetingPolicyClass);
	AreaPattern = NewObject<UMuksiCardAreaPattern>(this, CurrentCard->AreaPatternClass);

	if (!TargetingPolicy)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] Failed to create TargetingPolicy. Card=%s"), *GetNameSafe(CurrentCard));
		return false;
	}

	if (!AreaPattern)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] Failed to create AreaPattern. Card=%s"), *GetNameSafe(CurrentCard));
		return false;
	}

	const UScriptStruct* ExpectedTargetingDataStruct = TargetingPolicy->GetTargetingDataStruct();
	const UScriptStruct* ExpectedAreaPatternDataStruct = AreaPattern->GetAreaPatternDataStruct();

	if (!ExpectedTargetingDataStruct)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] TargetingPolicy data struct is null. Policy=%s"), *GetNameSafe(TargetingPolicy));
		return false;
	}

	if (!ExpectedAreaPatternDataStruct)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] AreaPattern data struct is null. Pattern=%s"), *GetNameSafe(AreaPattern));
		return false;
	}

	if (CurrentCard->TargetingData.GetScriptStruct() != ExpectedTargetingDataStruct)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] TargetingData type mismatch. Card=%s"), *GetNameSafe(CurrentCard));
		return false;
	}

	if (CurrentCard->AreaPatternData.GetScriptStruct() != ExpectedAreaPatternDataStruct)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MuksiCardTargetingComponent] AreaPatternData type mismatch. Card=%s"), *GetNameSafe(CurrentCard));
		return false;
	}

	return true;
}

FMuksiCardTargetingContext UMuksiCardTargetingComponent::BuildRuntimeContext(const FMuksiCardTargetingContext& InputContext) const
{
	FMuksiCardTargetingContext RuntimeContext = InputContext;

	RuntimeContext.SourceCharacter = SourceCharacter;
	RuntimeContext.Card = CurrentCard;
	RuntimeContext.GridManager = GridManager;

	return RuntimeContext;
}

void UMuksiCardTargetingComponent::ResetTargetingState()
{
	bIsTargeting = false;
	SourceCharacter = nullptr;
	CurrentCard = nullptr;
	TargetingPolicy = nullptr;
	AreaPattern = nullptr;
	CurrentResult.Reset();
}