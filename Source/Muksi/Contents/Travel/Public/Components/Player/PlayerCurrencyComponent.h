#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerCurrencyComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, NewGold);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MUKSI_API UPlayerCurrencyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerCurrencyComponent();

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintPure, Category = "Currency")
	int32 GetGold() const { return Gold; }

	UFUNCTION(BlueprintPure, Category = "Currency")
	bool HasEnoughGold(int32 Amount) const;

	UFUNCTION(BlueprintCallable, Category = "Currency")
	void AddGold(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Currency")
	bool SpendGold(int32 Amount);

	UFUNCTION(BlueprintCallable, Category = "Currency")
	void SetGold(int32 NewGold);

	UPROPERTY(BlueprintAssignable, Category = "Currency")
	FOnGoldChanged OnGoldChanged;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Currency", meta = (ClampMin = "0"))
	int32 Gold = 0;
};