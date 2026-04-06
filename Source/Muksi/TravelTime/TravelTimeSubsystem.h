#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "TravelTimeTypes.h"
#include "TravelTimeSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
    FOnTravelHourPassed,
    FTravelDate,
    CurrentTime
);

UCLASS()
class MUKSI_API UTravelTimeSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
    GENERATED_BODY()

private:
    const int TotalTravelMinute = 180; 
    const FTravelDate StartDate = FTravelDate(1857, 3, 3, 12);
    const FTravelDate EndDate = FTravelDate(1859, 9, 9, 12);

private:
    FTravelDate CurrentDate;
    //int64 TotalHours = 0;

    float SecondsPerTravelHour = 1.f;
    float AccumulatedRealSeconds = 0.f;

    bool bIsRunning = false;

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void InitializeTravelTimeSubsystem();
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual bool IsTickable() const override { return true; }

public:
    UPROPERTY(BlueprintAssignable)
    FOnTravelHourPassed OnHourPassed;

    UFUNCTION(BlueprintCallable)
    void StartTravelTime();

    UFUNCTION(BlueprintCallable)
    void StopTravelTime();

    UFUNCTION(BlueprintCallable)
    FTravelDate GetCurrentDate() const;

    UFUNCTION(BlueprintCallable)
    float GetCurrentHourFloat() const;

    UFUNCTION(BlueprintCallable)
    float GetNormalizedDayTime() const;

    UFUNCTION(BlueprintCallable)
    FText GetFormattedTime() const;

    FText GetFormattedTime(const FTravelDate& Date) const;

    UFUNCTION(BlueprintCallable)
    FTravelDuration GetTimeDiff(const FTravelDate& Early, const FTravelDate& Late);

private:
    void AdvanceOneHour();
    void SetTotalTravelTime(float RealPlayMinutes);

};