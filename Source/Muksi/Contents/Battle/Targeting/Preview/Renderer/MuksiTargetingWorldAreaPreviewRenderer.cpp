#include "Muksi/Contents/Battle/Targeting/Preview/Renderer/MuksiTargetingWorldAreaPreviewRenderer.h"

#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingCircleWorldAreaPreviewStrategy.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Strategy/MuksiTargetingConeWorldAreaPreviewStrategy.h"
#include "Muksi/Contents/Battle/Targeting/Preview/Types/MuksiTargetingPreviewCommand.h"

void UMuksiTargetingWorldAreaPreviewRenderer::Initialize(UStaticMeshComponent* InCircleWorldAreaMesh, UStaticMeshComponent* InConeWorldAreaMesh)
{
	CircleStrategy = NewObject<UMuksiTargetingCircleWorldAreaPreviewStrategy>(this);
	ConeStrategy = NewObject<UMuksiTargetingConeWorldAreaPreviewStrategy>(this);

	if (CircleStrategy)
	{
		CircleStrategy->Initialize(InCircleWorldAreaMesh);
	}

	if (ConeStrategy)
	{
		ConeStrategy->Initialize(InConeWorldAreaMesh);
	}

	Hide();
}

void UMuksiTargetingWorldAreaPreviewRenderer::Update(const FMuksiTargetingPreviewCommand& Command, const FMuksiTargetingWorldAreaPreviewSettings& Settings)
{
	HideAllStrategies();

	switch (Command.WorldAreaStyle)
	{
	case EMuksiCardTargetingWorldAreaPreviewStyle::Circle:
		if (CircleStrategy)
		{
			CircleStrategy->Update(Command, Settings);
		}
		return;

	case EMuksiCardTargetingWorldAreaPreviewStyle::Cone:
		if (ConeStrategy)
		{
			ConeStrategy->Update(Command, Settings);
		}
		return;

	case EMuksiCardTargetingWorldAreaPreviewStyle::None:
	case EMuksiCardTargetingWorldAreaPreviewStyle::Rectangle:
	default:
		return;
	}
}

void UMuksiTargetingWorldAreaPreviewRenderer::Hide()
{
	HideAllStrategies();
}

void UMuksiTargetingWorldAreaPreviewRenderer::HideAllStrategies()
{
	if (CircleStrategy)
	{
		CircleStrategy->Hide();
	}

	if (ConeStrategy)
	{
		ConeStrategy->Hide();
	}
}