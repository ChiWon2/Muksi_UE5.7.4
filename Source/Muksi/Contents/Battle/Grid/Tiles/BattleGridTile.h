// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Hex/HexOffsetCoord.h"
#include "Muksi/Contents/Battle/Grid/SelectGridInterface.h"

#include "GameFramework/Actor.h"
#include "BattleGridTile.generated.h"

class ABattleGridManager;
class UStaticMeshComponent;
class UArrowComponent;

UCLASS()
class MUKSI_API ABattleGridTile : public AActor, public ISelectGridInterface
{
	GENERATED_BODY()
	
public:	
	ABattleGridTile();

protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION()
	void SetGridManager(ABattleGridManager* InGridManager){GridManager = InGridManager;};
	UFUNCTION()
	void SetGridCoord(FHexOffsetCoord Coord){GridCoord = Coord;};
	UFUNCTION()
	FHexOffsetCoord GetGridCoord()const{return GridCoord;};
	
	UFUNCTION()
	void SetTargetIndicatorVisible(bool bVisible);
	
	UFUNCTION()
	FVector GetGridCenterWorldLocation()const;
	
	UFUNCTION()
	FTransform GetCharacterSpawnTransform() const;
	
	
	UFUNCTION()
	void OnHoverBegin();
	UFUNCTION()
	void OnHoverEnd();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UArrowComponent> CenterPointComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> TargetIndicatorMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> ExtraIndicatorMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Indicator")
	TObjectPtr<UMaterialInterface> NormalIndicatorMaterial;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	FHexOffsetCoord GridCoord = FHexOffsetCoord();
	
	UPROPERTY()
	ABattleGridManager* GridManager;
public:
	virtual void OnGridSelected_Implementation() override;

};
