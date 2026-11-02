#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WorldMapMarkerWidget.generated.h"

class UHorizontalBox;
class UImage;
class USizeBox;
class UTextBlock;
class UWorldMapMarkerComponent;

UCLASS(Abstract, Blueprintable, meta = (DisableNativeTick))
class MUKSI_API UWorldMapMarkerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	bool HasValidMarkerLayout() const;
	bool GetMarkerLayout(bool bNameOnLeft, FVector2D& OutSize, FVector2D& OutIconCenter) const;
	void UpdateMarkerAppearance(const UWorldMapMarkerComponent* Marker);
	void SetNameOnLeft(bool bLeft);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UHorizontalBox> MarkerContent;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<USizeBox> IconAnchor;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> IconImage;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UWidget> NameContainer;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> NameText;
};
