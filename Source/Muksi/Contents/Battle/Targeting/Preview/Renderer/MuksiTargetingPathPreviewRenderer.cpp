#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingPathPreviewRenderer.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingArcPathPreviewStrategy.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingArrowPathPreviewStrategy.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingStraightPathPreviewStrategy.h"

void UMuksiTargetingPathPreviewRenderer::Initialize(AActor* InOwnerActor, USceneComponent* InSceneRoot, UStaticMeshComponent* InStraightPathMesh, UStaticMeshComponent* InArrowMesh, USplineComponent* InPathSpline)
{
	StraightStrategy = NewObject<UMuksiTargetingStraightPathPreviewStrategy>(this);
	ArrowStrategy = NewObject<UMuksiTargetingArrowPathPreviewStrategy>(this);
	ArcStrategy = NewObject<UMuksiTargetingArcPathPreviewStrategy>(this);

	if (StraightStrategy)
	{
		StraightStrategy->Initialize(InOwnerActor, InSceneRoot, InStraightPathMesh, InArrowMesh, InPathSpline);
	}

	if (ArrowStrategy)
	{
		ArrowStrategy->Initialize(InOwnerActor, InSceneRoot, InStraightPathMesh, InArrowMesh, InPathSpline);
	}

	if (ArcStrategy)
	{
		ArcStrategy->Initialize(InOwnerActor, InSceneRoot, InStraightPathMesh, InArrowMesh, InPathSpline);
	}

	Hide();
}

void UMuksiTargetingPathPreviewRenderer::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPathPreviewSettings& Settings)
{
	HideAllStrategies();

	switch (Command.PathStyle)
	{
	case EMuksiCardTargetingPathPreviewStyle::Straight:
		if (StraightStrategy)
		{
			StraightStrategy->Update(Command, Settings);
		}
		return;

	case EMuksiCardTargetingPathPreviewStyle::Arrow:
		if (ArrowStrategy)
		{
			ArrowStrategy->Update(Command, Settings);
		}
		return;

	case EMuksiCardTargetingPathPreviewStyle::Arc:
		if (ArcStrategy)
		{
			ArcStrategy->Update(Command, Settings);
		}
		return;

	case EMuksiCardTargetingPathPreviewStyle::None:
	default:
		return;
	}
}

void UMuksiTargetingPathPreviewRenderer::Hide()
{
	HideAllStrategies();
}

void UMuksiTargetingPathPreviewRenderer::HideAllStrategies()
{
	if (StraightStrategy)
	{
		StraightStrategy->Hide();
	}

	if (ArrowStrategy)
	{
		ArrowStrategy->Hide();
	}

	if (ArcStrategy)
	{
		ArcStrategy->Hide();
	}
}