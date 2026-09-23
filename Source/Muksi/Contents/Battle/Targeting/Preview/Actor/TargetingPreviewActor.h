#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "GameFramework/Actor.h"

#include "TargetingPreviewActor.generated.h"

class ABattleGridManager;
class USceneComponent;
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
	void ClearPathPreview();
	void ClearAllPreview();

	UStaticMeshComponent* GetSelectionPreviewMesh() const { return SelectionPreviewMesh; }
	ABattleGridManager* GetGridManager() const { return GridManager; }

	USplineMeshComponent* CreatePathMeshComponent();
	UStaticMeshComponent* CreateArrowMeshComponent();
	void ClearPathMeshComponents();
	void ClearArrowMeshComponents();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneRoot = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Targeting Preview", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> SelectionPreviewMesh = nullptr;




	UPROPERTY(Transient)
	TArray<TObjectPtr<USplineMeshComponent>> PathMeshComponents;

	UPROPERTY(Transient)
	TArray<TObjectPtr<UStaticMeshComponent>> ArrowMeshComponents;

	UPROPERTY(Transient)
	TObjectPtr<ABattleGridManager> GridManager = nullptr;

	void ApplyPreviewStyle();
};
