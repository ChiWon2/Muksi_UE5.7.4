// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Muksi/Contents/Battle/CharacterDataBase.h"
#include "SelectableCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class USelectableCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class MUKSI_API ISelectableCharacterInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual void OnSelected() = 0;
	virtual void OnDeselected() = 0;
};
