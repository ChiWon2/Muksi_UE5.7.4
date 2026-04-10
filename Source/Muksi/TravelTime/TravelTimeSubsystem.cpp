#include "TravelTimeSubsystem.h"

UTravelTimeSubsystem* UTravelTimeSubsystem::Get(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UGameInstance* GI = WorldContextObject->GetWorld()->GetGameInstance();
    return GI ? GI->GetSubsystem<UTravelTimeSubsystem>() : nullptr;
}

void UTravelTimeSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    InitializeTravelTimeSubsystem();
    StartTravelTime();
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

FText UTravelTimeSubsystem::GetCurrentFormattedTime(bool bIncludeMinute) const
{
    if (bIncludeMinute)
    {
        float HourFloat = GetCurrentHourFloat();
        int32 Minute = FMath::FloorToInt((HourFloat - FMath::FloorToInt(HourFloat)) * 60.f);
        return FText::FromString(
            FString::Printf(TEXT("%d/%d/%d %02d:%02d"),
                CurrentDate.Year,
                CurrentDate.Month,
                CurrentDate.Day,
                CurrentDate.Hour,
                Minute
            )
        );
    }

    return FText::FromString(
        FString::Printf(TEXT("%d/%d/%d %02d"),
            CurrentDate.Year,
            CurrentDate.Month,
            CurrentDate.Day,
            CurrentDate.Hour
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

    int64 StartH = FTravelTimeUtils::DateToHours(StartDate);
    int64 EndH = FTravelTimeUtils::DateToHours(EndDate);

    int64 TotalGameHours = FMath::Abs(EndH - StartH);
    FTravelDuration TotalTravelDuration = FTravelTimeUtils::HoursToDuration(TotalGameHours);

    if (TotalGameHours <= 0)
        return;

    float TotalRealSeconds = RealTravelMinutes * 60.f;
    SecondsPerTravelHour = TotalRealSeconds / (float)TotalGameHours;

    UE_LOG(LogTemp, Warning,TEXT("[UTravelTimeSubsystem] TotalTravelMinute: %.2f min / TotalGameDuration: %d D %d H = %d H -> SecPerTravelHour = %.4f"), RealTravelMinutes , TotalTravelDuration.Day, TotalTravelDuration.Hour, TotalGameHours,SecondsPerTravelHour);
}

FTravelDuration UTravelTimeSubsystem::GetTimeDiff(const FTravelDate& Early, const FTravelDate& Late)
{
    return Late - Early;
}