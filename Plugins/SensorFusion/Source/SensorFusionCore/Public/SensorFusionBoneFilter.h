#pragma once

#include "Engine.h"

class USensorFusionBone;

#include "SensorFusionBoneFilter.generated.h"


//! @defgroup sffilters Sensor Fusion Data Filters

//! @addtogroup sffilters
//! @brief All kind of filters to smooth out the different data streams.



//! @brief A blueprint interface to apply filter algorithms to specific bones. 
//! @details All implemented filters are automatically detected by the system. A single filter will be
//!		applied on the same bone data stream to guarantee its functionality.
//! @ingroup sffilters
UCLASS(Abstract)
class SENSORFUSIONCORE_API USensorFusionBoneFilter : public UObject
{
	GENERATED_BODY()

public:
	//! @brief Applies the filter to a specific bone.
	//! @note The filter is intended to operate only on the same bones over time. So NEVER apply 
	//! it in one frame to bone A, while applying it to bone B in the next one. This may violate
	//! the correctness of the underlying algorithm, resulting in undefined behaviour.
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Filter")
	virtual USensorFusionBone* ApplyFilter(USensorFusionBone* Bone) { return Bone; }


	//! @brief Reset the underlying algorithm.
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Filter")
	virtual void Reset() {}
};