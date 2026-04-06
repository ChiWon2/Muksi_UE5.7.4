#include "TravelTimeSubsystem.h"

void UTravelTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitializeTravelTimeSubsystem();
}

void UTravelTimeSubsystem::InitializeTravelTimeSubsystem()
{
    CurrentDate = StartDate;
    SetTotalTravelTime(TotalTravelMinute);
}

void UTravelTimeSubsystem::Tick(float DeltaTime)
{
    if (!bIsRunning)
        return;

    AccumulatedRealSeconds += DeltaTime;

    while (AccumulatedRealSeconds >= SecondsPerTravelHour)
    {
        AccumulatedRealSeconds -= SecondsPerTravelHour;
        AdvanceOneHour();
    }
}

TStatId UTravelTimeSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(UTravelTimeSubsystem, STATGROUP_Tickables);
}

void UTravelTimeSubsystem::StartTravelTime()
{
    bIsRunning = true;
}

void UTravelTimeSubsystem::StopTravelTime()
{
    bIsRunning = false;
}

FTravelDate UTravelTimeSubsystem::GetCurrentDate() const
{
    return CurrentDate;
}

/////////FOR MOVING SUN//////////
float UTravelTimeSubsystem::GetCurrentHourFloat() const
{
    //0.00 ~ 23.99
    float Ratio = SecondsPerTravelHour > 0.f? AccumulatedRealSeconds / SecondsPerTravelHour: 0.f;

    return CurrentDate.Hour + Ratio;
}

float UTravelTimeSubsystem::GetNormalizedDayTime() const
{
    //0.00 ~ 23.99
    return GetCurrentHourFloat() / 24.f;
}
///////////////////////////////

FText UTravelTimeSubsystem::GetFormattedTime() const
{
    float HourFloat = GetCurrentHourFloat();
    int32 Minute = FMath::FloorToInt((HourFloat - CurrentDate.Hour) * 60.f);

    return FText::FromString(
        FString::Printf(
            TEXT("%04d-%02d-%02d  %02d:%02d"),
            CurrentDate.Year,
            CurrentDate.Month,
            CurrentDate.Day,
            CurrentDate.Hour,
            Minute
        )
    );
}

FText UTravelTimeSubsystem::GetFormattedTime(const FTravelDate& Date) const
{
    return FText::FromString(
        FString::Printf(
            TEXT("%04d-%02d-%02d  %02d:00"),
            Date.Year,
            Date.Month,
            Date.Day,
            Date.Hour
        )
    );
}


void UTravelTimeSubsystem::AdvanceOneHour()
{
    FTravelTimeUtils::AddOneHour(CurrentDate);
    OnHourPassed.Broadcast(CurrentDate);
}

void UTravelTimeSubsystem::SetTotalTravelTime(float RealTravelMinutes)
{
    if (RealTravelMinutes <= 0.f)
        return;

    int64 StartH = FTravelTimeUtils::ConvertToHours(StartDate);
    int64 EndH = FTravelTimeUtils::ConvertToHours(EndDate);

    int64 TotalGameHours = FMath::Abs(EndH - StartH);

    if (TotalGameHours <= 0)
        return;

    float TotalRealSeconds = RealTravelMinutes * 60.f;
    SecondsPerTravelHour = TotalRealSeconds / (float)TotalGameHours;

    UE_LOG(LogTemp, Warning,TEXT("[UTravelTimeSubsystem] TotalPlay: %.2f min ¡æ SecPerTravelHour = %.4f"), RealTravelMinutes,SecondsPerTravelHour);
}

FTravelDuration UTravelTimeSubsystem::GetTimeDiff(const FTravelDate& Early, const FTravelDate& Late)
{
    return Late - Early;
}