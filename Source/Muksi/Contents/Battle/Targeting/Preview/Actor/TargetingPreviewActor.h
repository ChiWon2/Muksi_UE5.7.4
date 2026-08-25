#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "GameFramework/Actor.h"

#include "Muksi/Contents/Battle/Simulation/Data/BattleSimulationTypes.h"

#include "TargetingPreviewActor.generated.h"

class ABattleGridManager;
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
	void Initialize(ABattleGridManager* InGridManager, EBattleSimulationWorldType InGridWorldType);
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
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SelectionPreviewMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> AreaPreviewMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> ArrowPreviewMesh = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USplineComponent> PathSpline = nullptr;

	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineMeshComponent>> PathMeshComponents;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;
	EBattleSimulationWorldType GridWorldType = EBattleSimulationWorldType::PlayerActualEnemyActual;

	bool bEnemyStyle = false;
	void ApplyPreviewStyle();
};
