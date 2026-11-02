#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DestinationMarkerWidget.generated.h"

class UImage;
class UWidgetAnimation;

UCLASS(Abstract, Blueprintable)
class MUKSI_API UDestinationMarkerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	bool IsConfigured() const;
	void SetMarkerVisible(bool bVisible);
	int32 GetMarkerZOrder() const { return MarkerZOrder; }

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> OuterImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> CenterImage;

	UPROPERTY(Transient, meta = (BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> OuterPulseAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "World Map Marker", meta = (DisplayName = "Z Order"))
	int32 MarkerZOrder = 50;

private:
	bool bMarkerVisible = false;
};
