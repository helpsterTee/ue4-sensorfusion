#pragma once 

#include "AvateeringComponent.h"

#include "WorkflowCentricApplication.h"


//! @brief Public interface to access the mapping editor.
class ISensorFusionToolkit 
: public FWorkflowCentricApplication
{
public:
	virtual UAvateeringProfile* GetProfile() const = 0;
};