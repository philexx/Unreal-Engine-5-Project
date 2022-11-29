// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SlateBasics.h"
#include "SlateExtras.h"

/**
 * 
 */
class SMouseBoundarie : public SCompoundWidget
{
public:
	SLATE_BEGIN_ARGS(SMouseBoundarie) {}

	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);

	virtual bool SupportsKeyboardFocus() const override;
};
