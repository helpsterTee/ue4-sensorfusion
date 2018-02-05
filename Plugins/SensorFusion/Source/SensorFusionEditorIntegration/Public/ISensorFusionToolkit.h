#pragma once 

#include "AvateeringComponent.h"



//! @brief Public interface to access the mapping editor.
class ISensorFusionToolkit : public FAssetEditorToolkit
{
public:
	virtual const USkeletalMesh* GetTarget() const = 0;
	virtual UAvateeringProfile* GetMapping() const = 0;
};