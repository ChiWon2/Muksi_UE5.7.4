// Fill out your copyright notice in the Description page of Project Settings.


#include "Muksi/Contents/Battle/BattleCardPreviewComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/GameplayStaticsTypes.h"
#include "Muksi/Contents/Battle/Character/BattleCharacterBase.h"
#include "Muksi/Contents/Battle/Data/MuksiBattleCardDataAsset.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Muksi/Contents/Battle/Grid/BattleGridManager.h"
#include "Muksi/Contents/Battle/BattleManager.h"

#include "DrawDebugHelpers.h"


// Sets default values for this component's properties
UBattleCardPreviewComponent::UBattleCardPreviewComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UBattleCardPreviewComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	InitializePathMeshComponent();
}


// Called every frame
void UBattleCardPreviewComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	TestHover();
	// ...
}

void UBattleCardPreviewComponent::InitializePreviewComponent(ABattleManager* InBattleManager,
	ABattleGridManager* InGridManager)
{
	BattleManager = InBattleManager;
	GridManager = InGridManager;
	
}

void UBattleCardPreviewComponent::InitializePathMeshComponent()
{
	if (PathInstancedMeshComponent)
	{
		return;
	}

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	PathInstancedMeshComponent = NewObject<UInstancedStaticMeshComponent>(
		OwnerActor,
		TEXT("PathInstancedMeshComponent")
	);

	if (!PathInstancedMeshComponent)
	{
		return;
	}

	PathInstancedMeshComponent->RegisterComponent();

	if (USceneComponent* RootComponent = OwnerActor->GetRootComponent())
	{
		PathInstancedMeshComponent->AttachToComponent(
			RootComponent,
			FAttachmentTransformRules::KeepWorldTransform
		);
	}

	if (PathPointMesh)
	{
		PathInstancedMeshComponent->SetStaticMesh(PathPointMesh);
	}
}

void UBattleCardPreviewComponent::StartCardPreview(ABattleCharacterBase* InSourceCharacter
	)
{
	//if ()
}

void UBattleCardPreviewComponent::UpdateHoverTile(const FIntPoint& HoveredCoord)
{
	
}

void UBattleCardPreviewComponent::ClearPreview()
{
	
}

void UBattleCardPreviewComponent::UpdateHoveredTile(FTransform StartTransform, FTransform EndTransform, bool InRange)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	// Z 차이가 이 값 이상이면 거의 높은 쪽으로 붙게 됨
	const float MaxZDiffForBias = 500.0f;
	
	const FVector StartLocation = StartTransform.GetLocation();
	const FVector EndLocation = EndTransform.GetLocation();

	const float ZDiff = FMath::Abs(StartLocation.Z - EndLocation.Z);
	const float BiasAlpha = FMath::Clamp(ZDiff / MaxZDiffForBias, 0.0f, 1.0f);

	const FVector MidLocation = (StartLocation + EndLocation) * 0.5f;

	const FVector HigherLocation = StartLocation.Z >= EndLocation.Z
		? StartLocation
		: EndLocation;

	FVector CenterLocation = FMath::Lerp(MidLocation, HigherLocation, BiasAlpha);
	CenterLocation.Z = FMath::Max(StartLocation.Z, EndLocation.Z) + UpZ;
	
	if (InRange)ShowCurvePathWithMesh(StartLocation, CenterLocation, EndLocation, TruePreviewMaterial);
	else ShowCurvePathWithMesh(StartLocation, CenterLocation, EndLocation, FalsePreviewMaterial);
	/*if (InRange)
	DrawCurvePath(StartLocation, CenterLocation, EndLocation, FColor::Green);
	else DrawCurvePath(StartLocation, CenterLocation, EndLocation, FColor::Red);*/
}

void UBattleCardPreviewComponent::ClearHoveredTile()
{
	if (PathInstancedMeshComponent)PathInstancedMeshComponent->ClearInstances();
}

void UBattleCardPreviewComponent::TestHover()
{
	FTransform StartTransform = TestStartActor->GetTransform();
	FTransform EndTransform = TestEndActor->GetTransform();
	// Z 차이가 이 값 이상이면 거의 높은 쪽으로 붙게 됨
	const float MaxZDiffForBias = 500.0f;
	
	const FVector StartLocation = StartTransform.GetLocation();
	const FVector EndLocation = EndTransform.GetLocation();

	const float ZDiff = FMath::Abs(StartLocation.Z - EndLocation.Z);
	const float BiasAlpha = FMath::Clamp(ZDiff / MaxZDiffForBias, 0.0f, 1.0f);

	const FVector MidLocation = (StartLocation + EndLocation) * 0.5f;

	const FVector HigherLocation = StartLocation.Z >= EndLocation.Z
		? StartLocation
		: EndLocation;

	FVector CenterLocation = FMath::Lerp(MidLocation, HigherLocation, BiasAlpha);
	CenterLocation.Z = FMath::Max(StartLocation.Z, EndLocation.Z) + UpZ;
	
	//DrawCurvePath(StartLocation, CenterLocation, EndLocation);
}

void UBattleCardPreviewComponent::DrawCurvePath(const FVector& StartLocation, const FVector& CenterLocation,
	const FVector& EndLocation, FColor Color)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const int32 SegmentCount = 30;
	const float LifeTime = 0.05f;
	const float Thickness = 5.0f;

	FVector PrevPoint = StartLocation;

	for (int32 i = 1; i <= SegmentCount; ++i)
	{
		const float T = static_cast<float>(i) / static_cast<float>(SegmentCount);

		const FVector CurvePoint =
			FMath::Pow(1.0f - T, 2.0f) * StartLocation +
			2.0f * (1.0f - T) * T * CenterLocation +
			FMath::Pow(T, 2.0f) * EndLocation;

		DrawDebugLine(
			World,
			PrevPoint,
			CurvePoint,
			Color,
			false,
			LifeTime,
			0,
			Thickness
		);

		PrevPoint = CurvePoint;
	}

	DrawDebugSphere(World, StartLocation, 10.0f, 12, FColor::Blue, false, LifeTime);
	DrawDebugSphere(World, CenterLocation, 10.0f, 12, FColor::Yellow, false, LifeTime);
	DrawDebugSphere(World, EndLocation, 10.0f, 12, FColor::Red, false, LifeTime);
}

void UBattleCardPreviewComponent::ShowCurvePathWithMesh(const FVector& StartLocation, const FVector& CenterLocation,
	const FVector& EndLocation, UMaterialInterface* PathMaterial)
{
	if (!PathInstancedMeshComponent || !PathPointMesh)
	{
		return;
	}

	PathInstancedMeshComponent->SetStaticMesh(PathPointMesh);
	PathInstancedMeshComponent->ClearInstances();
	
	if (PathMaterial)
	{
		PathInstancedMeshComponent->SetMaterial(0, PathMaterial);
	}

	const float Distance = FVector::Dist(StartLocation, EndLocation);

	const int32 MinSegmentCount = 5;
	const int32 MaxSegmentCount = 40;
	const float DistancePerSegment = 100.0f;

	const int32 SegmentCount = FMath::Clamp(
		FMath::CeilToInt(Distance / DistancePerSegment),
		MinSegmentCount,
		MaxSegmentCount
	);

	const float MeshScale = 0.2f;

	for (int32 i = 0; i <= SegmentCount; ++i)
	{
		const float T = static_cast<float>(i) / static_cast<float>(SegmentCount);

		const FVector CurvePoint =
			FMath::Pow(1.0f - T, 2.0f) * StartLocation +
			2.0f * (1.0f - T) * T * CenterLocation +
			FMath::Pow(T, 2.0f) * EndLocation;

		const float NextT = FMath::Clamp(
			static_cast<float>(i + 1) / static_cast<float>(SegmentCount),
			0.0f,
			1.0f
		);

		const FVector NextCurvePoint =
			FMath::Pow(1.0f - NextT, 2.0f) * StartLocation +
			2.0f * (1.0f - NextT) * NextT * CenterLocation +
			FMath::Pow(NextT, 2.0f) * EndLocation;

		FVector Direction = NextCurvePoint - CurvePoint;

		if (Direction.IsNearlyZero())
		{
			Direction = EndLocation - CenterLocation;
		}

		const FRotator Rotation = Direction.GetSafeNormal().Rotation();

		FTransform InstanceTransform;
		InstanceTransform.SetLocation(CurvePoint);
		InstanceTransform.SetRotation(Rotation.Quaternion());
		InstanceTransform.SetScale3D(FVector(MeshScale));

		PathInstancedMeshComponent->AddInstance(InstanceTransform);
	}
}

