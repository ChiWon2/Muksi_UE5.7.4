// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SelectGridInterface.h"
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
	// Sets default values for this actor's properties
	ABattleGridTile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
public:
	UFUNCTION()
	void SetGridManager(ABattleGridManager* InGridManager){GridManager = InGridManager;};
	UFUNCTION()
	void SetGridCoord(FIntPoint Coord){GridCoord = Coord;};
	UFUNCTION()
	FIntPoint GetGridCoord()const{return GridCoord;};
	
	UFUNCTION()
	void SetTargetIndicatorVisible(bool bVisible);
	
	UFUNCTION()
	FVector GetGridCenterWorldLocation()const;
	
	UFUNCTION()
	FTransform GetCharacterSpawnTransform() const;
	
	UFUNCTION()
	void SetExchangeIndicator(int32 IndicatorType);
	
	UFUNCTION()
	void ClearExchangeIndicator();
	
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
	TObjectPtr<UStaticMeshComponent> ExchangeIndicatorMesh;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Indicator")
	TObjectPtr<UMaterialInterface> NormalIndicatorMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Indicator")
	TObjectPtr<UMaterialInterface> AttackableIndicatorMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Indicator")
	TObjectPtr<UMaterialInterface> BlockedIndicatorMaterial;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Indicator")
	TObjectPtr<UMaterialInterface> MoveIndicatorMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Battle|Grid")
	FIntPoint GridCoord = FIntPoint::ZeroValue;
	
	UPROPERTY()
	ABattleGridManager* GridManager;
	
	
	
	
public:
	virtual void OnGridSelected_Implementation() override;

};
