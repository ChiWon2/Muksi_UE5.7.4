#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ZoneActor.generated.h"

class UBoxComponent;
class UBillboardComponent;

UENUM(BlueprintType)
enum class EZoneType : uint8
{
	None        UMETA(DisplayName = "None"),
	Country     UMETA(DisplayName = "Country"),
	Border      UMETA(DisplayName = "Border"),
	Town        UMETA(DisplayName = "Town"),
	Dungeon     UMETA(DisplayName = "Dungeon")
};

USTRUCT(BlueprintType)
struct FZoneData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	FName ZoneID = NAME_None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	FText ZoneDisplayName = FText::FromString(TEXT("Unknown Zone"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	EZoneType ZoneType = EZoneType::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zone")
	bool bCanOpenTownUI = true;
};

UCLASS()
class MUKSI_API AZoneActor : public AActor
{
	GENERATED_BODY()

public:
	AZoneActor();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone")
	UBoxComponent* ZoneBounds;

#if WITH_EDITORONLY_DATA
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Zone")
	UBillboardComponent* Billboard;
#endif

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zone")
	FZoneData ZoneData;

	UFUNCTION(BlueprintCallable, Category = "Zone")
	const FZoneData& GetZoneData() const { return ZoneData; }

protected:
	UFUNCTION()
	void OnZoneBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION()
	void OnZoneEndOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};

