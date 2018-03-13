#pragma once

#include "Engine.h"

#include "SensorFusionBoneFilter.h"

#include "SensorFusionBone.generated.h"


//! @enum ESensorFusionBone
//! @brief Most important bones for skeletal tracking
//! @details These bones are the most impotant bones to capture human movement. They have mostly 
//!		anatomic names to minimize ambiguity. Since there is no way to group enumerations 
//!		together, the individual enum values specify in the individuals bone group. On top of
//!		these strong enumeration values is a helper function #USensorFusionFunctionLibrary::GetParentBoneType, which 
//!		contains the hierarchical information. In this framework the human skeleton is 
//!		interpreted as a mathematical tree, such that every bone has a unique parent. The root
//!		is arbitrarily set at the pelvis, since this simplified the kinect integration.
UENUM(BlueprintType)
enum class ESensorFusionBone : uint8
{
	//! @{
	//! Spine column bones
	SpineLumbar,
	SpineThoracic,
	SpineCervical,
	Cranium,
	//! @}


	//! @{
	//! Left arm bones
	ClavicleLeft,
	HumerusLeft,
	UlnaLeft, //!< Equally to the radius bone
	//! @}


	//! @{
	//! Left hand bones
	WristLeft,
	ThumbMetacarpalLeft,
	ThumbProximalLeft,
	ThumbMedialLeft,
	ThumbDistalLeft,
	IndexFingerMetacarpalLeft,
	IndexFingerProximalLeft,
	IndexFingerMedialLeft,
	IndexFingerDistalLeft,
	MiddleFingerMetacarpalLeft,
	MiddleFingerProximalLeft,
	MiddleFingerMedialLeft,
	MiddleFingerDistalLeft,
	RingFingerMetacarpalLeft,
	RingFingerProximalLeft,
	RingFingerMedialLeft,
	RingFingerDistalLeft,
	PinkyFingerMetacarpalLeft,
	PinkyFingerProximalLeft,
	PinkyFingerMedialLeft,
	PinkyFingerDistalLeft,
	//! @}


	//! @{
	//! Right arm bones
	ClavicleRight,
	HumerusRight,
	UlnaRight, //!< Equally to the radius bone
	//! @}


	//! @{
	//! Right hand bones
	WristRight,
	ThumbMetacarpalRight,
	ThumbProximalRight,
	ThumbMedialRight,
	ThumbDistalRight,
	IndexFingerMetacarpalRight,
	IndexFingerProximalRight,
	IndexFingerMedialRight,
	IndexFingerDistalRight,
	MiddleFingerMetacarpalRight,
	MiddleFingerProximalRight,
	MiddleFingerMedialRight,
	MiddleFingerDistalRight,
	RingFingerMetacarpalRight,
	RingFingerProximalRight,
	RingFingerMedialRight,
	RingFingerDistalRight,
	PinkyFingerMetacarpalRight,
	PinkyFingerProximalRight,
	PinkyFingerMedialRight,
	PinkyFingerDistalRight,
	//! @}


	//! @{
	//! Left leg bones
	FemurLeft,
	TibiaLeft,
	FootLeft,
	//! @}

	
	//! @{
	//! Right leg bones
	FemurRight,
	TibiaRight,
	FootRight,
	//! @}


	//! @{
	Pelvis, //!< The pelvis bone is our skeletons root
	//! @}


	//! @{
	Invalid, //!< In the standard workflow all invalid bones should fall into this category
	//! @}
};



//! @defgroup enum_interation_helpers Iteration helper functions
//! @details Global helper functions to allow direct iteration over enumerations.
//! @{
static ESensorFusionBone operator++(ESensorFusionBone& bone) { return bone = static_cast<ESensorFusionBone>(static_cast<uint8>(bone) + 1); }
static ESensorFusionBone begin(ESensorFusionBone bone) { return ESensorFusionBone::SpineLumbar; }
static ESensorFusionBone end(ESensorFusionBone bone) { return static_cast<ESensorFusionBone>(static_cast<uint8>(ESensorFusionBone::Invalid)); }
static ESensorFusionBone operator*(ESensorFusionBone bone) { return bone; }
//! @}



//! @class USensorFusionBone
//! @brief A bone in basis of the engines default skeleton.
UCLASS(NotPlaceable, NotBlueprintable, BlueprintType, ClassGroup = "Sensor Fusion|Avateering")
class SENSORFUSIONCORE_API USensorFusionBone : public UObject
{
	GENERATED_BODY()

public:
	//! Unreal space bone position.
	UPROPERTY(BlueprintReadOnly, Category = "Sensor Fusion|Avateering")
	FVector Position;


	UPROPERTY(BlueprintReadOnly, Category = "Sensor Fusion|Avateering")
	FVector PositionRaw;


	//! Unreal space bone orientation.
	UPROPERTY(BlueprintReadOnly, Category = "Sensor Fusion|Avateering")
	FRotator Orientation;


	//! Sensor space orientation.
	UPROPERTY(BlueprintReadOnly, Category = "Sensor Fusion|Avateering")
	FRotator OrientationRaw;


	//! Bone length in unreal units.
	UPROPERTY(BlueprintReadOnly, Category = "Sensor Fusion|Avateering")
	float Length;


	//! Is true whenever this bone was successully tracked.
	UPROPERTY(BlueprintReadOnly, Category = "Sensor Fusion|Avateering")
	bool IsTracked;


	//! The tracking system's confidence in detecting the right orientation and position.
	UPROPERTY(BlueprintReadOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"), Category = "Sensor Fusion|Avateering")
	float Confidence;


	//! Which bone type.
	UPROPERTY(BlueprintReadOnly, Category = "Sensor Fusion|Avateering")
	ESensorFusionBone Which;
};