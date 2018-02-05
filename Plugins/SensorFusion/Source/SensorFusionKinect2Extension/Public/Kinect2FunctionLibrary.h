#pragma once

#include "Engine.h"

#include "AvateeringComponent.h"
#include "SensorFusionBone.h"

#include "Kinect2FunctionLibrary.generated.h"


//! @brief Some special paths
//! @ingroup kinect2extbp
UENUM(BlueprintType)
enum class EKinect2SpecialBonePath
{
	Nearest = 0,
	Centered
};



//! @ingroup
//! @{
static EKinect2SpecialBonePath operator++(EKinect2SpecialBonePath& path) { return path = static_cast<EKinect2SpecialBonePath>(static_cast<int>(path) + 1); }
static EKinect2SpecialBonePath begin(EKinect2SpecialBonePath path) { return EKinect2SpecialBonePath::Nearest; }
static EKinect2SpecialBonePath end(EKinect2SpecialBonePath path) { return static_cast<EKinect2SpecialBonePath>(2); }
static EKinect2SpecialBonePath operator*(EKinect2SpecialBonePath path) { return path; }
//! @}



//! @brief Enumeration to map between kinect 2 and blueprint
UENUM(BlueprintType)
enum class EKinect2JointType : uint8
{
	SpineBase = 0,
	SpineMid = 1,
	Neck = 2,
	Head = 3,
	ShoulderLeft = 4,
	ElbowLeft = 5,
	WristLeft = 6,
	HandLeft = 7,
	ShoulderRight = 8,
	ElbowRight = 9,
	WristRight = 10,
	HandRight = 11,
	HipLeft = 12,
	KneeLeft = 13,
	AnkleLeft = 14,
	FootLeft = 15,
	HipRight = 16,
	KneeRight = 17,
	AnkleRight = 18,
	FootRight = 19,
	SpineShoulder = 20,
	HandTipLeft = 21,
	ThumbLeft = 22,
	HandTipRight = 23,
	ThumbRight = 24,
};



// Iteration helper
static EKinect2JointType operator++(EKinect2JointType& joint) { return joint = static_cast<EKinect2JointType>(static_cast<uint8>(joint) + 1); }
static EKinect2JointType begin(EKinect2JointType joint) { return EKinect2JointType::SpineBase; }
static EKinect2JointType end(EKinect2JointType joint) { return static_cast<EKinect2JointType>(25); }
static EKinect2JointType operator*(EKinect2JointType joint) { return joint; }


//! @brief Main utilities to handle kinect 2 sensors.
//! @ingroup kinect2extbp
UCLASS(ClassGroup = "Sensor Fusion|Kinect 2")
class UKinect2FunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()

private:
	static UEnum* BoneEnumeratorReflection;

public:
	//!
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Sensor Fusion|Kinect 2")
	static EKinect2JointType GetParentJointType(const EKinect2JointType joint);


	//!
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Kinect 2")
	static FName GetBoneName(const EKinect2JointType joint);


	//! 
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Kinect 2")
	static URegistryPath* BuildBonePath(const int body, const EKinect2JointType joint);


	//! @brief Construct a path to specialized bodies.
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Kinect 2")
	static URegistryPath* BuildSpecialBonePath(const EKinect2JointType joint, const EKinect2SpecialBonePath which);


	//! @brief Get the matching bone type in the standard model.
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Kinect 2")
	static ESensorFusionBone GetAccordingBone(const EKinect2JointType joint);


	//! @brief Get the body count for the default sensor.
	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Kinect 2")
	static int GetMaxBodyCount( );


	//!
	static FName BuildBonePathInternal(const int body, const EKinect2JointType joint);
	static FName BuildSpecialBonePathInternal(const EKinect2JointType joint, const EKinect2SpecialBonePath which);
};