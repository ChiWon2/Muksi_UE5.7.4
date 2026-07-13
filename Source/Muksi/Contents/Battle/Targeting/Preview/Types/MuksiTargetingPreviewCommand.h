#pragma once

#include "CoreMinimal.h"
#include "Muksi/Contents/Battle/Targeting/Types/MuksiCardTargetingPreviewTypes.h"
#include "MuksiTargetingPreviewCommand.generated.h"

USTRUCT(BlueprintType)
struct FMuksiTargetingPreviewCommand
{
	GENERATED_BODY()

	// 전체 Preview를 표시할지 결정한다.
	UPROPERTY(BlueprintReadOnly)
	bool bVisible = false;

	// 현재 조준 결과를 확정할 수 있는지 나타낸다.
	UPROPERTY(BlueprintReadOnly)
	bool bCanConfirm = false;

	// 시전자 주변 선택 사거리를 표시할지 결정한다.
	UPROPERTY(BlueprintReadOnly)
	bool bShowSelectionRange = false;

	// 카드가 선택할 수 있는 최대 그리드 거리.
	UPROPERTY(BlueprintReadOnly)
	int32 SelectionRange = 0;

	// 시전자에서 조준 위치까지의 경로 표시 방식.
	UPROPERTY(BlueprintReadOnly)
	EMuksiCardTargetingPathPreviewStyle PathStyle = EMuksiCardTargetingPathPreviewStyle::None;

	// 조준 위치에 표시되는 월드 범위 방식.
	UPROPERTY(BlueprintReadOnly)
	EMuksiCardTargetingWorldAreaPreviewStyle WorldAreaStyle = EMuksiCardTargetingWorldAreaPreviewStyle::None;

	// 실제 그리드 표시 방식.
	UPROPERTY(BlueprintReadOnly)
	EMuksiCardTargetingGridPreviewStyle GridStyle = EMuksiCardTargetingGridPreviewStyle::None;

	// 시전자의 월드 위치.
	UPROPERTY(BlueprintReadOnly)
	FVector SourceWorldLocation = FVector::ZeroVector;

	// 현재 조준 중인 월드 위치.
	UPROPERTY(BlueprintReadOnly)
	FVector AimWorldLocation = FVector::ZeroVector;

	// 현재 선택된 대표 그리드 좌표.
	UPROPERTY(BlueprintReadOnly)
	FIntPoint SelectedCoord = FIntPoint(INDEX_NONE, INDEX_NONE);

	// 곡선 경로의 최대 높이.
	UPROPERTY(BlueprintReadOnly)
	float ArcHeight = 0.0f;

	// 조준 위치에 표시되는 월드 범위 반지름.
	UPROPERTY(BlueprintReadOnly)
	float WorldAreaRadius = 0.0f;

	// 원뿔형 월드 범위 각도.
	UPROPERTY(BlueprintReadOnly)
	float WorldAreaAngle = 0.0f;

	// 사각형 월드 범위 크기.
	UPROPERTY(BlueprintReadOnly)
	FVector2D WorldAreaSize = FVector2D::ZeroVector;

	// 실제 효과가 적용되는 좌표 목록.
	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> AffectedCoords;

	// 경로에 포함되는 좌표 목록.
	UPROPERTY(BlueprintReadOnly)
	TArray<FIntPoint> PathCoords;

	void Reset()
	{
		bVisible = false;
		bCanConfirm = false;
		bShowSelectionRange = false;
		SelectionRange = 0;
		PathStyle = EMuksiCardTargetingPathPreviewStyle::None;
		WorldAreaStyle = EMuksiCardTargetingWorldAreaPreviewStyle::None;
		GridStyle = EMuksiCardTargetingGridPreviewStyle::None;
		SourceWorldLocation = FVector::ZeroVector;
		AimWorldLocation = FVector::ZeroVector;
		SelectedCoord = FIntPoint(INDEX_NONE, INDEX_NONE);
		ArcHeight = 0.0f;
		WorldAreaRadius = 0.0f;
		WorldAreaAngle = 0.0f;
		WorldAreaSize = FVector2D::ZeroVector;
		AffectedCoords.Empty();
		PathCoords.Empty();
	}
};