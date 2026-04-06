#pragma once

#include "CoreMinimal.h"
#include "TravelTimeTypes.generated.h"

//////////////////////////////////////////////////////////////
// Date
//////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTravelDate
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Year;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Month;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Day;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Hour;

    FTravelDate()
        : Year(1), Month(1), Day(1), Hour(0)
    {
    }

    FTravelDate(int32 InYear, int32 InMonth, int32 InDay, int32 InHour)
        : Year(InYear), Month(InMonth), Day(InDay), Hour(InHour)
    {
    }
};

//////////////////////////////////////////////////////////////
// Duration
//////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct FTravelDuration
{
    GENERATED_BODY();

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Day;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Hour;

    FTravelDuration()
        : Day(0), Hour(0)
    {
    }

    FTravelDuration(int32 InDay, int32 InHour)
        : Day(InDay), Hour(InHour)
    {
    }
};

//////////////////////////////////////////////////////////////
// Utils
//////////////////////////////////////////////////////////////

class FTravelTimeUtils
{
public:

    //////////////////////////////////////////////////////////
    //윤년
    //////////////////////////////////////////////////////////
    static bool IsLeapYear(int32 Year)
    {
        return (Year % 4 == 0 && Year % 100 != 0) || (Year % 400 == 0);
    }

    //////////////////////////////////////////////////////////
    // 월 일수
    //////////////////////////////////////////////////////////
    static int32 GetDaysInMonth(int32 Year, int32 Month)
    {
        switch (Month)
        {
        case 2: return IsLeapYear(Year) ? 29 : 28;
        case 4: case 6: case 9: case 11: return 30;
        default: return 31;
        }
    }

    //////////////////////////////////////////////////////////
    // Date → Hours
    //////////////////////////////////////////////////////////
    static int64 ConvertToHours(const FTravelDate& Time)
    {
        int32 Y = Time.Year - 1;

        int64 TotalDays = (int64)Y * 365
            + (Y / 4)
            - (Y / 100)
            + (Y / 400);

        static const int32 MonthDays[12] =
        { 31,28,31,30,31,30,31,31,30,31,30,31 };

        for (int32 M = 1; M < Time.Month; ++M)
        {
            TotalDays += MonthDays[M - 1];

            if (M == 2 && IsLeapYear(Time.Year))
            {
                TotalDays += 1;
            }
        }

        TotalDays += (Time.Day - 1);

        return TotalDays * 24 + Time.Hour;
    }

    //////////////////////////////////////////////////////////
    // Hours → Date
    //////////////////////////////////////////////////////////
    static FTravelDate ConvertToDate(int64 InHours)
    {
        FTravelDate Result;

        int64 TotalDays = InHours / 24;
        Result.Hour = InHours % 24;

        int64 Num400 = TotalDays / 146097;
        int32 DayOf400 = TotalDays % 146097;

        int32 Num100 = FMath::Min((int32)(DayOf400 / 36524), 3);
        int32 DayOf100 = DayOf400 - Num100 * 36524;

        int32 Num4 = DayOf100 / 1461;
        int32 DayOf4 = DayOf100 % 1461;

        int32 Num1 = FMath::Min(DayOf4 / 365, 3); //leap year overflow

        int32 Year = (int32)(Num400 * 400 + Num100 * 100 + Num4 * 4 + Num1 + 1);
        int32 DayOfYear = DayOf4 - Num1 * 365;

        Result.Year = Year;

        static const int32 MonthDays[12] =
        { 31,28,31,30,31,30,31,31,30,31,30,31 };

        int32 Month = 1;

        for (int32 i = 0; i < 12; ++i)
        {
            int32 Days = MonthDays[i];

            if (i == 1 && IsLeapYear(Year))
            {
                Days += 1;
            }

            if (DayOfYear < Days)
                break;

            DayOfYear -= Days;
            Month++;
        }

        Result.Month = Month;
        Result.Day = DayOfYear + 1;

        return Result;
    }

    //////////////////////////////////////////////////////////
    // Duration < - > Hour
    //////////////////////////////////////////////////////////
    static int64 DurationToHours(const FTravelDuration& D)
    {
        return (int64)D.Day * 24 + D.Hour;
    }

    static FTravelDuration HoursToDuration(int64 Hours)
    {
        FTravelDuration Result;
        Result.Day = Hours / 24;
        Result.Hour = Hours % 24;
        return Result;
    }
    //AddOneHour
    static FORCEINLINE void AddOneHour(FTravelDate& Date)
    {
        Date.Hour++;

        if (Date.Hour >= 24)
        {
            Date.Hour = 0;
            Date.Day++;
        }

        int32 MaxDays = GetDaysInMonth(Date.Year, Date.Month);
        if (Date.Day > MaxDays)
        {
            Date.Day = 1;
            Date.Month++;
        }

        if (Date.Month > 12)
        {
            Date.Month = 1;
            Date.Year++;
        }
    }

    static FTravelDate AddHours(const FTravelDate& Date, int64 Hours)
    {
        int64 Base = ConvertToHours(Date);
        return ConvertToDate(Base + Hours);
    }
};

//////////////////////////////////////////////////////////////
// Operator Overloading
//////////////////////////////////////////////////////////////

// Date - Date = Duration
FORCEINLINE FTravelDuration operator-(const FTravelDate& A, const FTravelDate& B)
{
    int64 AH = FTravelTimeUtils::ConvertToHours(A);
    int64 BH = FTravelTimeUtils::ConvertToHours(B);

    return FTravelTimeUtils::HoursToDuration(AH - BH);
}

// Date + Duration = Date
FORCEINLINE FTravelDate operator+(const FTravelDate& Date, const FTravelDuration& Duration)
{
    int64 Base = FTravelTimeUtils::ConvertToHours(Date);
    int64 Add = FTravelTimeUtils::DurationToHours(Duration);

    return FTravelTimeUtils::ConvertToDate(Base + Add);
}

// Date - Duration = Date
FORCEINLINE FTravelDate operator-(const FTravelDate& Date, const FTravelDuration& Duration)
{
    int64 Base = FTravelTimeUtils::ConvertToHours(Date);
    int64 Sub = FTravelTimeUtils::DurationToHours(Duration);

    return FTravelTimeUtils::ConvertToDate(Base - Sub);
}

// Duration + Duration = Duration
FORCEINLINE FTravelDuration operator+(const FTravelDuration& A, const FTravelDuration& B)
{
    return FTravelTimeUtils::HoursToDuration(
        FTravelTimeUtils::DurationToHours(A) +
        FTravelTimeUtils::DurationToHours(B)
    );
}

// 비교
FORCEINLINE bool operator>(const FTravelDate& A, const FTravelDate& B)
{
    return FTravelTimeUtils::ConvertToHours(A) > FTravelTimeUtils::ConvertToHours(B);
}

FORCEINLINE bool operator<(const FTravelDate& A, const FTravelDate& B)
{
    return FTravelTimeUtils::ConvertToHours(A) < FTravelTimeUtils::ConvertToHours(B);
}