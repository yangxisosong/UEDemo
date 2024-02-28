// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "PropertyCustomizationHelpers.h"
#include "Layout/Visibility.h"
#include "Input/Reply.h"
#include "Widgets/SWidget.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

class SDLDebugWidget
	: public SCompoundWidget
{
	SLATE_BEGIN_ARGS(SDLDebugWidget)
		{}

	SLATE_EVENT(FOnClicked, OnSClicked)

	SLATE_EVENT(FOnSetClass, OnClassClicked)

	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	SDLDebugWidget();
	~SDLDebugWidget();


	FOnClicked OnSClicked;

	FOnSetClass OnClassClicked;
private:
	void OnBrowseWorld();

};
