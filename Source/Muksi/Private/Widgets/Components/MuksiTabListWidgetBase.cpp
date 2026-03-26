// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/Components/MuksiTabListWidgetBase.h"

#include "Widgets/Components/MuksiCommonButtonBase.h"
#include "Editor/WidgetCompilerLog.h"

void UMuksiTabListWidgetBase::RequestRegisterTab(const FName& InTabID, const FText& InTabDisplayName)
{
	RegisterTab(InTabID, TabButtonEntryWidgetClass, nullptr);
	
	if (UMuksiCommonButtonBase* FoundButton = Cast<UMuksiCommonButtonBase>(GetTabButtonBaseByID(InTabID)))
	{
		FoundButton->SetButtonText(InTabDisplayName);
	}
}

#if WITH_EDITOR
void UMuksiTabListWidgetBase::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);
	
	if (!TabButtonEntryWidgetClass)
	{
		CompileLog.Error(FText::FromString(TEXT("The variable TabButtonEntryWidgetClass has no valid entry specified. ") +
			GetClass()->GetName() +
			TEXT(" needs a valid entry widget class to function properly")));
	}
}
#endif
