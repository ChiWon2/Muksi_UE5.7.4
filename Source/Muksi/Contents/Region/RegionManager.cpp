#include "RegionManager.h"

#include "Engine/Texture2D.h"
#include "Engine/TextureDefines.h"

void ARegionManager::BeginPlay()
{
    Super::BeginPlay();
    //TODO :: ForTest Region CurrentRegionTexture
    CurrentRegionTexture = RegionTextures[0];

    ColorMap.Empty();


    ColorMap.Add( FColor::Red, ERegionID::RED);

    ColorMap.Add(FColor::Green, ERegionID::GREEN);

    ColorMap.Add(FColor::Blue, ERegionID::BLUE);

    ColorMap.Add( FColor::Yellow, ERegionID::YELLO);

    ColorMap.Add( FColor(255, 0, 255), ERegionID::TempRegionName_5);

    ColorMap.Add( FColor(0, 255, 255), ERegionID::TempRegionName_6);

    ColorMap.Add( FColor::White, ERegionID::TempRegionName_7);

    CacheLandscapeBounds();
    
    CreateLandscapeMID();

    UpdateLandscapeMaterialParameters();

    if (CurrentRegionTexture)
    {
        CreateRegionArrayByTexture();
    }
}

void ARegionManager::ChangeRegionTexture(int32 Index)
{
    if (!RegionTextures.IsValidIndex(Index))
    {
        UE_LOG(LogTemp, Error,TEXT("Invalid Texture Index"));

        return;
    }

    CurrentRegionTexture = RegionTextures[Index];

    if (LandscapeMID)
    {
        LandscapeMID->SetTextureParameterValue( TEXT("RegionTexture"), CurrentRegionTexture);
    }

    CreateRegionArrayByTexture();

    UE_LOG(LogTemp, Warning, TEXT("Region Texture Changed"));
}

uint8 ARegionManager::ConvertColorToRegionID(const FColor& Color)
{
    if (const ERegionID* Found = ColorMap.Find(Color))
    {
        return (uint8)(*Found);
    }

    return (uint8)ERegionID::None;
}

void ARegionManager::CacheLandscapeBounds()
{
    if (!TargetLandscape)
    {
        UE_LOG( LogTemp, Error, TEXT("TargetLandscape is NULL"));
        return;
    }

    CachedLandscapeBounds = TargetLandscape->GetComponentsBoundingBox();

    UE_LOG( LogTemp, Warning, TEXT("Landscape Bounds Cached --- Min = %s  ,   Max = %s" ), *CachedLandscapeBounds.Min.ToString(), *CachedLandscapeBounds.Max.ToString());
}

void ARegionManager::CreateRegionArrayByTexture()
{
    if (!CurrentRegionTexture)
    {
        UE_LOG(LogTemp, Error, TEXT("CurrentRegionTexture is NULL"));
        return;
    }

    FTextureSource& Source = CurrentRegionTexture->Source;

    Width = Source.GetSizeX();

    Height = Source.GetSizeY();

    RegionIDs.SetNum(Width * Height);

    const uint8* RawData = Source.LockMipReadOnly(0);

    const FColor* Pixels = reinterpret_cast<const FColor*>(RawData);

    for (int32 i = 0; i < Width * Height; i++)
    {
        RegionIDs[i] = ConvertColorToRegionID(Pixels[i]);
    }

    Source.UnlockMip(0);

    UE_LOG(LogTemp, Warning, TEXT("Loaded Region Texture %d x %d"), Width, Height);
}

ERegionID ARegionManager::GetRegionByPixel(int32 X, int32 Y) const
{
    if (X < 0 || X >= Width)
    {
        return ERegionID::None;
    }

    if (Y < 0 || Y >= Height)
    {
        return ERegionID::None;
    }

    int32 Index = Y * Width + X;

    return (ERegionID) RegionIDs[Index];
}

ERegionID ARegionManager::GetRegionByWorldLocation(const FVector& WorldLocation) const
{
    if (RegionIDs.Num() == 0)
    {
        return ERegionID::None;
    }

    const float LandscapeWidth = CachedLandscapeBounds.Max.X - CachedLandscapeBounds.Min.X;

    const float LandscapeHeight = CachedLandscapeBounds.Max.Y - CachedLandscapeBounds.Min.Y;

    const float U = (WorldLocation.X - CachedLandscapeBounds.Min.X) / LandscapeWidth;

    const float V = (WorldLocation.Y - CachedLandscapeBounds.Min.Y) / LandscapeHeight;

    const int32 PixelX = FMath::Clamp( FMath::FloorToInt(U * Width), 0, Width - 1);

    const int32 PixelY = FMath::Clamp( FMath::FloorToInt(V * Height), 0, Height - 1);

    return GetRegionByPixel( PixelX, PixelY);
}

void ARegionManager::CreateLandscapeMID()
{
    if (!TargetLandscape)
    {
        return;
    }

    UMaterialInterface* Material = TargetLandscape->GetLandscapeMaterial();

    if (!Material)
    {
        UE_LOG(LogTemp, Error, TEXT("Landscape Material is NULL"));
        return;
    }

    LandscapeMID = UMaterialInstanceDynamic::Create( Material, this);

    TargetLandscape->LandscapeMaterial = LandscapeMID;
}

void ARegionManager::UpdateLandscapeMaterialParameters()
{
    if (!LandscapeMID)
    {
        return;
    }

    const FVector Min = CachedLandscapeBounds.Min;

    const FVector Size( CachedLandscapeBounds.Max.X - CachedLandscapeBounds.Min.X, CachedLandscapeBounds.Max.Y - CachedLandscapeBounds.Min.Y,1.f);

    LandscapeMID->SetVectorParameterValue(TEXT("LandscapeMin"),FLinearColor( Min.X, Min.Y, 0.f, 0.f));

    LandscapeMID->SetVectorParameterValue( TEXT("LandscapeSize"), FLinearColor(Size.X, Size.Y, 1.f, 0.f));

    LandscapeMID->SetTextureParameterValue( TEXT("RegionTexture"), CurrentRegionTexture);
}

void ARegionManager::DebugWorldLocationToRegion(const FVector& WorldLocation)
{
    const ERegionID Region = GetRegionByWorldLocation( WorldLocation);

UE_LOG(
    LogTemp,
    Warning,
    TEXT("Region of (%.0f, %.0f, %.0f) = %s"),
    WorldLocation.X,
    WorldLocation.Y,
    WorldLocation.Z,
    *UEnum::GetValueAsString(Region));}

void ARegionManager::DebugPixelAtWorldLocation(const FVector& WorldLocation)
{
    const float LandscapeWidth =CachedLandscapeBounds.Max.X - CachedLandscapeBounds.Min.X;

    const float LandscapeHeight = CachedLandscapeBounds.Max.Y - CachedLandscapeBounds.Min.Y;

    const float U =(WorldLocation.X -CachedLandscapeBounds.Min.X) / LandscapeWidth;

    const float V = (WorldLocation.Y - CachedLandscapeBounds.Min.Y)/ LandscapeHeight;

    const int32 PixelX = FMath::Clamp( FMath::FloorToInt(U * Width), 0, Width - 1);

    const int32 PixelY = FMath::Clamp( FMath::FloorToInt(V * Height), 0, Height - 1);

    const ERegionID Region = GetRegionByPixel( PixelX, PixelY);

    UE_LOG( LogTemp, Warning, TEXT("World=%s U=%f V=%f Pixel=(%d,%d) Region=%d"), *WorldLocation.ToString(), U, V, PixelX, PixelY, (uint8)Region);
}

void ARegionManager::SetOverlayVisible(bool bVisible)
{
    if (!LandscapeMID)
    {
        return;
    }

    LandscapeMID->SetScalarParameterValue(TEXT("ShowRegionOverlay"),bVisible ? 0.5f : 0.f);
}
