#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingArcPathPreviewStrategy.h"

#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Actor.h"

void UMuksiTargetingArcPathPreviewStrategy::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingPathPreviewSettings& Settings)
{
	EnsureArcPathMeshes(FMath::Max(2, Settings.ArcSegmentCount));

	if (ArcPathMeshes.IsEmpty())
	{
		return;
	}

	FVector StartWorldLocation = Command.SourceWorldLocation;
	FVector EndWorldLocation = Command.AimWorldLocation;

	ApplyHeightOffset(StartWorldLocation, EndWorldLocation, Settings.PathPreviewHeightOffset);

	if (FVector::DistSquared(StartWorldLocation, EndWorldLocation) <= KINDA_SMALL_NUMBER)
	{
		Hide();
		return;
	}

	const FVector MidWorldLocation = (StartWorldLocation + EndWorldLocation) * 0.5f;
	const FVector ControlWorldLocation = MidWorldLocation + FVector(0.0f, 0.0f, FMath::Max(0.0f, Command.ArcHeight));

	if (StraightPathMesh)
	{
		StraightPathMesh->SetVisibility(false);
	}

	if (ArrowMesh)
	{
		ArrowMesh->SetVisibility(false);
	}

	if (PathSpline)
	{
		PathSpline->ClearSplinePoints(false);
		PathSpline->AddSplinePoint(StartWorldLocation, ESplineCoordinateSpace::World, false);
		PathSpline->AddSplinePoint(ControlWorldLocation, ESplineCoordinateSpace::World, false);
		PathSpline->AddSplinePoint(EndWorldLocation, ESplineCoordinateSpace::World, false);
		PathSpline->UpdateSpline();
		PathSpline->SetVisibility(false);
	}

	for (int32 SegmentIndex = 0; SegmentIndex < ArcPathMeshes.Num(); ++SegmentIndex)
	{
		const float StartAlpha = static_cast<float>(SegmentIndex) / static_cast<float>(ArcPathMeshes.Num());
		const float EndAlpha = static_cast<float>(SegmentIndex + 1) / static_cast<float>(ArcPathMeshes.Num());
		const FVector SegmentStart = GetQuadraticBezierPoint(StartWorldLocation, ControlWorldLocation, EndWorldLocation, StartAlpha);
		const FVector SegmentEnd = GetQuadraticBezierPoint(StartWorldLocation, ControlWorldLocation, EndWorldLocation, EndAlpha);

		UpdatePathSegment(ArcPathMeshes[SegmentIndex], SegmentStart, SegmentEnd, Settings.PathWidth, Settings.ArcSegmentLengthMultiplier);
	}
}

void UMuksiTargetingArcPathPreviewStrategy::Hide()
{
	Super::Hide();
	HideArcPathMeshes();
}

void UMuksiTargetingArcPathPreviewStrategy::EnsureArcPathMeshes(int32 RequiredSegmentCount)
{
	if (!OwnerActor || !SceneRoot || !StraightPathMesh)
	{
		return;
	}

	if (ArcPathMeshes.Num() == RequiredSegmentCount)
	{
		return;
	}

	for (UStaticMeshComponent* ArcPathMesh : ArcPathMeshes)
	{
		if (ArcPathMesh)
		{
			ArcPathMesh->DestroyComponent();
		}
	}

	ArcPathMeshes.Empty();

	for (int32 SegmentIndex = 0; SegmentIndex < RequiredSegmentCount; ++SegmentIndex)
	{
		const FName BaseComponentName = *FString::Printf(TEXT("ArcPathMesh_%02d"), SegmentIndex);
		const FName ComponentName = MakeUniqueObjectName(OwnerActor, UStaticMeshComponent::StaticClass(), BaseComponentName);
		UStaticMeshComponent* ArcPathMesh = NewObject<UStaticMeshComponent>(OwnerActor, ComponentName);

		if (!ArcPathMesh)
		{
			continue;
		}

		OwnerActor->AddInstanceComponent(ArcPathMesh);
		ArcPathMesh->SetupAttachment(SceneRoot);
		ArcPathMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ArcPathMesh->SetGenerateOverlapEvents(false);
		ArcPathMesh->SetCastShadow(false);
		ArcPathMesh->SetStaticMesh(StraightPathMesh->GetStaticMesh());
		ArcPathMesh->SetMaterial(0, StraightPathMesh->GetMaterial(0));
		ArcPathMesh->RegisterComponent();
		ArcPathMesh->SetVisibility(false);
		ArcPathMeshes.Add(ArcPathMesh);
	}
}

void UMuksiTargetingArcPathPreviewStrategy::HideArcPathMeshes()
{
	for (UStaticMeshComponent* ArcPathMesh : ArcPathMeshes)
	{
		if (ArcPathMesh)
		{
			ArcPathMesh->SetVisibility(false);
		}
	}
}

FVector UMuksiTargetingArcPathPreviewStrategy::GetQuadraticBezierPoint(const FVector& StartPoint, const FVector& ControlPoint, const FVector& EndPoint, float Alpha) const
{
	const float OneMinusAlpha = 1.0f - Alpha;

	return OneMinusAlpha * OneMinusAlpha * StartPoint + 2.0f * OneMinusAlpha * Alpha * ControlPoint + Alpha * Alpha * EndPoint;
}