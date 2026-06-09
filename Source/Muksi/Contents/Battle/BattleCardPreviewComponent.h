// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BattleCardPreviewComponent.generated.h"


class ABattleManager;
class ABattleGridManager;
class ABattleCharacterBase;
class UMuksiBattleCardDataAsset;
class UInstancedStaticMeshComponent;
class UMaterialInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MUKSI_API UBattleCardPreviewComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBattleCardPreviewComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void InitializePreviewComponent(
		ABattleManager* InBattleManager,
		ABattleGridManager* InGridManager
	);
	
	void InitializePathMeshComponent();

	void StartCardPreview(ABattleCharacterBase* InSourceCharacter);

	void UpdateHoverTile(const FIntPoint& HoveredCoord);

	void ClearPreview();
	
	
	UFUNCTION()
	void UpdateHoveredTile(FTransform StartTransform, FTransform EndTransform, bool bInRange);
	
	void ClearHoveredTile();

protected:
	UPROPERTY()
	TObjectPtr<ABattleManager> BattleManager;

	UPROPERTY()
	TObjectPtr<ABattleGridManager> GridManager;

	UPROPERTY()
	TObjectPtr<ABattleCharacterBase> SourceCharacter;

	UPROPERTY()
	TObjectPtr<UMuksiBattleCardDataAsset> CurrentCardData;
	
	UPROPERTY(EditAnywhere, Category="Preview")
	TObjectPtr<UStaticMesh> PathPointMesh;
	
	UPROPERTY()
	TObjectPtr<UInstancedStaticMeshComponent> PathInstancedMeshComponent;
	
	//Test
	UPROPERTY(EditAnywhere, Category = "Test")
	TObjectPtr<AActor> TestStartActor = nullptr;
	UPROPERTY(EditAnywhere, Category = "Test")
	TObjectPtr<AActor> TestEndActor = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
	TObjectPtr<UMaterialInterface> TruePreviewMaterial;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
	TObjectPtr<UMaterialInterface> FalsePreviewMaterial;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Preview")
	int32 OverlayWidth = 1.0f;
	
	
	void TestHover();
	void DrawCurvePath(const FVector& StartLocation, const FVector& CenterLocation, const FVector& EndLocation, FColor Color);
	void ShowCurvePathWithMesh(const FVector& StartLocation, const FVector& CenterLocation, const FVector& EndLocation, 
		UMaterialInterface* PathMaterial);
	
	UPROPERTY(EditAnywhere, Category = "Test")
	float UpZ = 1000.0f;
		
};
