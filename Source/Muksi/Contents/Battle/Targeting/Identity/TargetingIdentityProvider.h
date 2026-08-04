#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TargetingIdentityProvider.generated.h"

class ABattleCharacterBase;
class ABattleGridManager;

UINTERFACE(BlueprintType)
class MUKSI_API UTargetingIdentityProvider : public UInterface
{
	GENERATED_BODY()
};

class MUKSI_API ITargetingIdentityProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Battle|Targeting")
	FName GetTargetingCharacterKey() const;
};

class MUKSI_API FTargetingCharacterIdentity
{
public:
	static FName GetCharacterKey(const ABattleCharacterBase* Character);
	static ABattleCharacterBase* FindCharacterByKey(ABattleGridManager* GridManager, FName CharacterKey);
};
