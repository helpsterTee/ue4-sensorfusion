#pragma once

#include "Sensor.h"

#include "DragAndDrop.h"


//!
struct FSensorDragDropOperation
	: public FDragDropOperation
{
	DRAG_DROP_OPERATOR_TYPE(FSensorDragDropOperation, FDragDropOperation);


	//!
	FSensorDragDropOperation(USensor* InSensor)
		: GrabbedSensor(InSensor)
	{}


	//!
	TSharedPtr<SWidget> GetDefaultDecorator() const override
	{
		TSharedRef<SVerticalBox> VerticalBox = SNew(SVerticalBox);

		VerticalBox->AddSlot()
			[
				SNew(SBorder)
				.BorderImage(FEditorStyle::GetBrush("Graph.ConnectorFeedback.Border"))
			.Content()
			[
				SNew(SHorizontalBox)
				+ SHorizontalBox::Slot()
			.AutoWidth()
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(FText::FromName(GrabbedSensor->Name))
			]
			]
			];

		return VerticalBox;
	}


	using FDragDropOperation::Construct;


	USensor* GrabbedSensor;
};
