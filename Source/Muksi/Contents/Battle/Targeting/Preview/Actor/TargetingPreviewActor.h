#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "GameFramework/Actor.h"

#include "Muksi/Contents/Battle/Targeting/Types/TargetingGridPreviewMode.h"

#include "TargetingPreviewActor.generated.h"

class ABattleGridManager;
class UInstancedStaticMeshComponent;
class USceneComponent;
class USplineComponent;
class USplineMeshComponent;
class UStaticMeshComponent;

UCLASS()
class MUKSI_API ATargetingPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ATargetingPreviewActor();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	void Initialize(ABattleGridManager* InGridManager);
	void SetSelectionGridCoords(const TArray<FHexOffsetCoord>& InCoords);
	void SetPathGridCoords(const TArray<FHexOffsetCoord>& InCoords);
	void SetAreaGridCoords(const TArray<FHexOffsetCoord>& InCoords);
	void SetGridPreviewMode(ETargetingGridPreviewMode InGridPreviewMode);
	void ClearSelectionPreview();
	void ClearPathPreview();
	void ClearAreaPreview();
	void ClearAllPreview();
	void SetEnemyStyle(bool bInEnemyStyle);

	UStaticMeshComponent* GetSelectionPreviewMesh() const { return SelectionPreviewMesh; }
	UStaticMeshComponent* GetAreaPreviewMesh() const { return AreaPreviewMesh; }
	UStaticMeshComponent* GetArrowPreviewMesh() const { return ArrowPreviewMesh; }
	USplineComponent* GetPathSpline() const { return PathSpline; }
	ABattleGridManager* GetGridManager() const { return GridManager; }

	USplineMeshComponent* CreatePathMeshComponent();
	void ClearPathMeshComponents();

private:
	void LoadPreviewAssets();
	void RebuildGridPreview();
	void AddGridCoords(TSet<FHexOffsetCoord>& InOutCoords, const TArray<FHexOffsetCoord>& InCoords) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SelectionPreviewMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> AreaPreviewMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ArrowPreviewMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInstancedStaticMeshComponent> GridPreviewMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> PathSpline = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineMeshComponent>> PathMeshComponents;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	TArray<FHexOffsetCoord> SelectionGridCoords;
	TArray<FHexOffsetCoord> PathGridCoords;
	TArray<FHexOffsetCoord> AreaGridCoords;
	ETargetingGridPreviewMode GridPreviewMode = ETargetingGridPreviewMode::AffectedTiles;
	float PreviewHeightOffset = 5.0f;
	float OccupiedGridPreviewScale = 1.2f;
	bool bEnemyStyle = false;
	void ApplyPreviewStyle();
};
