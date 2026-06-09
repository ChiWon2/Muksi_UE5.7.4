#pragma once

#include "CoreMinimal.h"
#include "Widgets/Widget_ActivatableBase.h"
#include "Items/Types/MuksiItemTypes.h"
#include "Widget_InventoryEquipment.generated.h"

class UButton;
class UHorizontalBox;
class UPanelWidget;
class UUniformGridPanel;
class UWidget;
class UMuksiPlayerDataSubsystem;
class UInventoryComponent;
class UEquipmentComponent;
class UWidget_CharacterInfoPanel;
class UWidget_EquipmentSlot;
class UWidget_ItemSlot;

UENUM(BlueprintType)
enum class EInventoryCategoryFilter : uint8
{
	Equipment	UMETA(DisplayName = "Equipment"),
	Consumable	UMETA(DisplayName = "Consumable"),
	Special		UMETA(DisplayName = "Special")
};

UCLASS()
class MUKSI_API UWidget_InventoryEquipment : public UWidget_ActivatableBase
{
	GENERATED_BODY()

protected:
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UUniformGridPanel> InventoryGridPanel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> EquipmentSlotBox;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> EquipmentCategoryButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ConsumableCategoryButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> SpecialCategoryButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget_CharacterInfoPanel> CharacterInfoPanel;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> CharacterInfoButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UPanelWidget> WindowRoot;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UWidget> TitleBar;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UWidget_ItemSlot> ItemSlotClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	TSubclassOf<UWidget_EquipmentSlot> EquipmentSlotClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
	int32 InventoryColumnCount = 8;

private:
	UFUNCTION()
	void HandleBackClicked();

	UFUNCTION()
	void HandleEquipmentCategoryClicked();

	UFUNCTION()
	void HandleConsumableCategoryClicked();

	UFUNCTION()
	void HandleSpecialCategoryClicked();

	UFUNCTION()
	void HandleItemSlotClicked(FGuid InstanceId);

	UFUNCTION()
	void HandleItemSlotRightClicked(FGuid InstanceId);

	UFUNCTION()
	void HandleEquipmentSlotClicked(EMuksiEquipmentSlot EquipmentSlot);

	UFUNCTION()
	void HandleEquipmentSlotRightClicked(EMuksiEquipmentSlot EquipmentSlot);

	UFUNCTION()
	void HandleCharacterInfoButtonClicked();

	void SetCharacterInfoPanelVisible(bool bVisible);

	bool bCharacterInfoPanelVisible = false;

	void SetCategoryFilter(EInventoryCategoryFilter NewFilter);

	void RefreshAll();
	void RefreshInventoryGrid();
	void RefreshEquipmentSlots();
	void RefreshCharacterInfo();

	void AddEquipmentSlot(EMuksiEquipmentSlot EquipmentSlot);

	bool DoesItemPassCategoryFilter(const FMuksiInventoryEntry& Entry) const;

	UMuksiPlayerDataSubsystem* GetPlayerDataSubsystem() const;
	UInventoryComponent* GetInventoryComponent() const;
	UEquipmentComponent* GetEquipmentComponent() const;

	EInventoryCategoryFilter CurrentCategoryFilter = EInventoryCategoryFilter::Equipment;

	FGuid SelectedInstanceId;
	EMuksiEquipmentSlot SelectedEquipmentSlot = EMuksiEquipmentSlot::None;

	bool bDraggingWindow = false;
	FVector2D DragOffset = FVector2D::ZeroVector;
};