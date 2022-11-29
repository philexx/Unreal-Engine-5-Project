// Fill out your copyright notice in the Description page of Project Settings.


#include "SMouseBoundarie.h"

#include "Widgets/Layout/SConstraintCanvas.h"


void SMouseBoundarie::Construct(const FArguments& InArgs)
{
	ChildSlot
	[
		SNew(SConstraintCanvas)
		+ SConstraintCanvas::Slot()
		.Alignment(FVector2D(200,300))
		[
			SNew(SBox)
			.WidthOverride(100)
			.HeightOverride(100)
			[
				SNew(SImage)
				.ColorAndOpacity(FColor::Black)
			]
		]
	];
}

bool SMouseBoundarie::SupportsKeyboardFocus() const
{
	return SCompoundWidget::SupportsKeyboardFocus();
}
