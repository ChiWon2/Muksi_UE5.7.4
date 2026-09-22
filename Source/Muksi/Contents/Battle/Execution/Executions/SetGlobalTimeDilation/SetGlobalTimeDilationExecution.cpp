#include "Muksi/Contents/Battle/Execution/Executions/SetGlobalTimeDilation/SetGlobalTimeDilationExecution.h"

#include "Kismet/GameplayStatics.h"
#include "Muksi/Contents/Battle/Execution/Executions/SetGlobalTimeDilation/SetGlobalTimeDilationExecutionData.h"

USetGlobalTimeDilationExecution::USetGlobalTimeDilationExecution()
{
	bPresentationOnly = true;
}

void USetGlobalTimeDilationExecution::Execute(const FBattleExecutionContext& Context, FBattleExecutionFinished OnFinished)
{
	const FSetGlobalTimeDilationExecutionData* TimeDilationData = Context.GetExecutionData<FSetGlobalTimeDilationExecutionData>();
	const float TimeDilation = TimeDilationData ? FMath::Max(0.01f, TimeDilationData->TimeDilation) : 1.0f;

	if (UWorld* World = GetWorld())
		UGameplayStatics::SetGlobalTimeDilation(World, TimeDilation);

	FinishExecution(OnFinished);
}

const UScriptStruct* USetGlobalTimeDilationExecution::GetExecutionDataStruct() const
{
	return FSetGlobalTimeDilationExecutionData::StaticStruct();
}
