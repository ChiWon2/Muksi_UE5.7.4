#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "RegionID.h"

#include "Engine/Texture2D.h"
#include "Landscape.h"


#include "RegionManager.generated.h"

UCLASS()
class MUKSI_API ARegionManager : public AActor
{
    GENERATED_BODY()

public:

    virtual void BeginPlay() override;
    UPROPERTY()
    UMaterialInstanceDynamic* LandscapeMID;

    // 현재 사용중인 Region Texture
    UPROPERTY(BlueprintReadOnly)
    UTexture2D* CurrentRegionTexture;

    // 챕터별 Region Texture - Region이 챕터별로 변할 수도 있다.
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Region")
    TArray<UTexture2D*> RegionTextures;

    UPROPERTY(EditAnywhere, Category = "Region")
    ALandscape* TargetLandscape;

    UFUNCTION(BlueprintCallable)
    void ChangeRegionTexture(int32 Index);

    UFUNCTION(BlueprintCallable)
    ERegionID GetRegionByPixel(int32 X,int32 Y) const;

    UFUNCTION(BlueprintCallable)
    ERegionID GetRegionByWorldLocation(const FVector& WorldLocation) const;

protected:
    // 1024x1024 Region 데이터
    UPROPERTY(VisibleAnywhere)
    TArray<uint8> RegionIDs;

    int32 Width = 0;
    int32 Height = 0;

    TMap<FColor, ERegionID> ColorMap;
    FBox CachedLandscapeBounds;


protected:

    void CreateRegionArrayByTexture();

    uint8 ConvertColorToRegionID(const FColor& Color);

    void CacheLandscapeBounds();

    void CreateLandscapeMID();

    void UpdateLandscapeMaterialParameters();
public:
    //TODO :: Delete under code // FORTEST
    UFUNCTION(BlueprintCallable)
    void DebugWorldLocationToRegion(const FVector& WorldLocation);

    UFUNCTION(BlueprintCallable)
    void DebugPixelAtWorldLocation( const FVector& WorldLocation);

    UFUNCTION(BlueprintCallable)
    void SetOverlayVisible(bool bVisible);
};