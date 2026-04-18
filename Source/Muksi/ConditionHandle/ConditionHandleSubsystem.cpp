// Fill out your copyright notice in the Description page of Project Settings.


#include "ConditionHandleSubsystem.h"
#include"Condition/Register/RegisterAllCondition.h"

void UConditionHandleSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	ConditionRegister::RegisterAllConditions();

	UE_LOG(LogTemp, Warning, TEXT("[ConditionHandleSubsystem]Condition System Initialized"));
}