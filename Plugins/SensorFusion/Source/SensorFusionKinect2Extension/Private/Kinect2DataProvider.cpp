#include "Kinect2ExtensionPCH.h"
#include "Kinect2DataProvider.h"
#include "Kinect2FunctionLibrary.h"

#include "SensorFusionCore.h"
#include "SensorFusionFunctionLibrary.h"

#include <limits>

#include "AllowWindowsPlatformTypes.h"



FKinect2DataProvider::FKinect2DataProvider(IKinectSensor* pSensor)
{
	this->Registry = FSensorFusionCore::Get().GetSensorDataRegistry();

	if (FAILED(pSensor->get_BodyFrameSource(&this->BFS)))
	{
		UE_LOG(SensorFusionLog, Error, TEXT("Unable to get kinect body frame source."));
	}
	else
	{
		UE_LOG(SensorFusionLog, Log, TEXT("Kinect body frame source ready."));
		if (FAILED(this->BFS->get_BodyCount(&MaxBodyCount))) 
		{
			UE_LOG(SensorFusionLog, Error, TEXT("Failed to acquire maximum body count."));
		}
		else 
		{
			UE_LOG(SensorFusionLog, Log, TEXT("Registering kinect 2 body data."));

			if (auto KinectSensor = this->Registry->RegisterSensor(TEXT("Kinect2")))
			{
				// Register all bodies
				for (int body = 0; body < this->GetMaxBodyCount(); body++)
				{
					for (auto joint : EKinect2JointType())
					{
						auto BonePath = UKinect2FunctionLibrary::BuildBonePath(body, joint);
						if (auto Bone = this->Registry->RegisterBone(BonePath->AsFName()))
						{
							Bone.GetValue()->Which = UKinect2FunctionLibrary::GetAccordingBone(joint);
							KinectSensor.GetValue()->ProvidedBones.Add(BonePath);
							UE_LOG(SensorFusionLog, Log, TEXT("Registered bone '%s'."), *BonePath->AsFName().ToString());
						}
						else
						{
							UE_LOG(SensorFusionLog, Error, TEXT("Failed to register bone '%s'."), *BonePath->AsFName().ToString());
						}
					}
				}

				// Register all references
				for (auto joint : EKinect2JointType())
				{
					auto BonePath = UKinect2FunctionLibrary::BuildBonePath(0, joint);
					for (auto spath : EKinect2SpecialBonePath())
					{
						auto ReferencePath = UKinect2FunctionLibrary::BuildSpecialBonePath(joint, spath);
						if (!this->Registry->RegisterBoneReference(ReferencePath->AsFName(), BonePath->AsFName()))
						{
							UE_LOG(SensorFusionLog, Error, TEXT("Failed to register bone reference '%s'."), *ReferencePath->AsFName().ToString());
						}
						else
						{
							KinectSensor.GetValue()->ProvidedBones.Add(ReferencePath);
						}
					}
				}

				// Launch worker thread
				if (FAILED(this->BFS->OpenReader(&this->BFR)))
				{
					UE_LOG(SensorFusionLog, Error, TEXT("Unable to open kinect body frame reader."));
				}
				else
				{
					UE_LOG(SensorFusionLog, Log, TEXT("Kinect body frame reader ready."));
					this->Thread = FRunnableThread::Create(this, TEXT("FKinect2DataProvider"));
				}
			}
		}
	}
}



FKinect2DataProvider::~FKinect2DataProvider() 
{
	UE_LOG(SensorFusionLog, Log, TEXT("Stopping kinect 2 data provider."));
	if (this->Thread)
	{
		if (this->IsRunning())
		{
			this->Thread->Kill(true);
		}
		delete this->Thread;
	}
	
	SafeRelease(this->BFR);
	SafeRelease(this->BFS);

	UE_LOG(SensorFusionLog, Log, TEXT("Unregistering kinect 2 body data."));
	for (int body = 0; body < this->GetMaxBodyCount(); body++)
	{
		for (auto joint : EKinect2JointType())
		{
			this->Registry->UnregisterBone(UKinect2FunctionLibrary::BuildBonePathInternal(body, joint));
		}
	}

	this->Registry->UnregisterSensor(TEXT("Kinect2"));
}



bool FKinect2DataProvider::Init()
{
	BodyBuffer = new IBody*[this->MaxBodyCount];
	for (int i = 0;i < this->MaxBodyCount;i++)
	{
		BodyBuffer[i] = nullptr;
	}
	if (this->BFR && SUCCEEDED(this->BFR->SubscribeFrameArrived(&BodyDataEvent)))
	{
		UE_LOG(SensorFusionLog, Log, TEXT("Kinect body data provider ready and running."));
		this->Running = true;
		return true;
	}
	
	return false;
}



uint32 FKinect2DataProvider::Run()
{
	while (this->IsRunning())
	{
		auto idx = WaitForSingleObject(reinterpret_cast<HANDLE>(BodyDataEvent), 500);

		switch (idx)
		{

		case (WAIT_OBJECT_0):
		{
			// Get the body data
			IBodyFrameArrivedEventArgs* pArgs;
			IBodyFrameReference* pRef;
			IBodyFrame *pF;
			
			if (FAILED(this->BFR->GetFrameArrivedEventData(BodyDataEvent, &pArgs)))
			{
				UE_LOG(SensorFusionLog, Error, TEXT("Failed to acquire frame event."));
				return -1;
			}
			if (FAILED(pArgs->get_FrameReference(&pRef)))
			{
				UE_LOG(SensorFusionLog, Error, TEXT("Failed to acquire frame reference."));
				return -1;
			}
			if (FAILED(pRef->AcquireFrame(&pF)))
			{
				UE_LOG(SensorFusionLog, Error, TEXT("Failed to acquire frame."));
				return -1;
			}
			if (FAILED(pF->GetAndRefreshBodyData(this->MaxBodyCount, BodyBuffer)))
			{
				UE_LOG(SensorFusionLog, Error, TEXT("Failed to acquire frame."));
				return -1;
			}


			//Helper
			float mostCenteredBodyDistance = std::numeric_limits<float>::max();
			char mostCenteredBody = -1;
			float nearestBodyDistance = std::numeric_limits<float>::max();
			char nearestBody = -1;


			for (int bodyIndex = 0; bodyIndex < this->MaxBodyCount; bodyIndex++)
			{
				// What we want to read
				Joint joints[JointType_Count];
				JointOrientation jointOrientations[JointType_Count];
				bool isBodyTracked = false;

				// Read body data
				this->BodyBuffer[bodyIndex]->get_IsTracked(reinterpret_cast<BOOLEAN*>(&isBodyTracked));
				this->BodyBuffer[bodyIndex]->GetJoints(JointType_Count, joints);
				this->BodyBuffer[bodyIndex]->GetJointOrientations(JointType_Count, jointOrientations);

				// Release body
				SafeRelease(this->BodyBuffer[bodyIndex]);
				
				// Save back to registry
				for (int boneIndex = 0; boneIndex < JointType_Count; boneIndex++)
				{
					if (auto NewBoneResult = this->Registry->GetBone(UKinect2FunctionLibrary::BuildBonePathInternal(bodyIndex, static_cast<EKinect2JointType>(boneIndex))))
					{
						auto NewBone = NewBoneResult.GetValue();
							
						//nb->Basis = FBasisVectorMatrix{
						//	{0, -1, 0},
						//	{0, 0, -1},
						//	{1, 0, 0},
						//	{0, 0, 0}
						//};
						NewBone->Confidence = joints[boneIndex].TrackingState / 2.;
						NewBone->IsTracked = !(joints[boneIndex].TrackingState == TrackingState_NotTracked);
						NewBone->Position = FVector(-joints[boneIndex].Position.X, -joints[boneIndex].Position.Z, joints[boneIndex].Position.Y) * 100;

						// This somehow fails
						//FVector axis;
						//float angle;
						//FQuat qk{kQuatVec.x, kQuatVec.y, kQuatVec.z, kQuatVec.w};
						//qk.ToAxisAndAngle(axis, angle);
						//FVector correctedAxis{-axis.Y, -axis.Z, axis.X};
						//FQuat corrected{ correctedAxis, angle };
						//nb->Orientation = corrected.Rotator();

						// No idea how to condensate this 2 steps into one formula...
						//nb->Orientation = FRotator(FQuat(kQuatVec.z, kQuatVec.y, kQuatVec.w, -kQuatVec.x));
						//nb->Orientation.Pitch = -nb->Orientation.Pitch;
						//nb->Orientation.Yaw = -nb->Orientation.Yaw;
						//nb->Orientation.Roll = -nb->Orientation.Roll;

						// FIX.... 
						auto BoneBaseCorrection = FQuat::Identity;
						if (boneIndex <= 4 || boneIndex == 20)
						{
							BoneBaseCorrection = FQuat(FRotator(90, 0, 90));
						}
						else if (boneIndex <= 7)
						{
							BoneBaseCorrection = FQuat(FRotator(90, 0, 90));
						}
						else if (boneIndex <= 11)
						{
							BoneBaseCorrection = FQuat(FRotator(-90, 0, 90));
						}
						else if (boneIndex <= 15)
						{
							BoneBaseCorrection = FQuat(FRotator(-90, 90, 90));
						}
						else if (boneIndex <= 19)
						{
							BoneBaseCorrection = FQuat(FRotator(90, 0, 180));
						}
						else
						{
							// something went probably wrong....
						}

						static const auto ONEEIGHTY = FQuat(FRotator(0, 180, 0));
						FQuat q = ONEEIGHTY*FQuat(-jointOrientations[boneIndex].Orientation.z, jointOrientations[boneIndex].Orientation.x, -jointOrientations[boneIndex].Orientation.y, jointOrientations[boneIndex].Orientation.w);
						q.Normalize();
						NewBone->Orientation = (FQuat(FRotator(0, 90, 0))*q*BoneBaseCorrection).Rotator();
						NewBone->Length = 1;
					}
				}


				// Special values
				auto SpineBaseResult = this->Registry->GetBone(UKinect2FunctionLibrary::BuildBonePathInternal(bodyIndex, EKinect2JointType::SpineBase));
				if (isBodyTracked && SpineBaseResult)
				{
					auto SpineBase = SpineBaseResult.GetValue();
					if (abs(SpineBase->Position.Y) < mostCenteredBodyDistance)
					{
						mostCenteredBodyDistance = abs(SpineBase->Position.Y);
						mostCenteredBody = bodyIndex;
					}

					if (abs(SpineBase->Position.X) < nearestBodyDistance)
					{
						nearestBodyDistance = abs(SpineBase->Position.X);
						nearestBody = bodyIndex;
					}
				}

			}


			// Update special paths
			if (mostCenteredBody >= 0)
			{
				for (auto joint : EKinect2JointType())
				{
					auto BonePath = UKinect2FunctionLibrary::BuildBonePathInternal(mostCenteredBody, joint);
					auto ReferencePath = UKinect2FunctionLibrary::BuildSpecialBonePathInternal(joint, EKinect2SpecialBonePath::Centered);
					this->Registry->UpdateBoneReference(ReferencePath, BonePath);
					auto SourceBoneResult = this->Registry->GetBone(BonePath);
					auto TargetBoneResult = this->Registry->GetBone(USensorFusionFunctionLibrary::BuildMainAvatarBonePath(UKinect2FunctionLibrary::GetAccordingBone(joint)));
					if (SourceBoneResult && TargetBoneResult)
					{
						auto SourceBone = SourceBoneResult.GetValue();
						auto TargetBone = TargetBoneResult.GetValue();
						TargetBone->Position = SourceBone->Position;
						TargetBone->Orientation = SourceBone->Orientation;
						TargetBone->Length = SourceBone->Length;
						TargetBone->IsTracked = SourceBone->IsTracked;
						TargetBone->Confidence = SourceBone->Confidence;
						TargetBone->Which = SourceBone->Which;
					}
				}
			}
			if (nearestBody >= 0)
			{
				for (auto joint : EKinect2JointType())
				{
					auto BonePath = UKinect2FunctionLibrary::BuildBonePathInternal(nearestBody, joint);
					auto ReferencePath = UKinect2FunctionLibrary::BuildSpecialBonePathInternal(joint, EKinect2SpecialBonePath::Nearest);
					this->Registry->UpdateBoneReference(ReferencePath, BonePath);
				}
			}


			// Release frame
			SafeRelease(pF);
		}
		break;

		case WAIT_ABANDONED:
		case WAIT_TIMEOUT:
		case WAIT_FAILED:
		default:
			break;
		}
	}

	return 0;
}



bool FKinect2DataProvider::IsRunning() const
{
	return this->Running;
}

int FKinect2DataProvider::GetMaxBodyCount() const
{
	return this->MaxBodyCount;
}



void FKinect2DataProvider::Stop()
{
	this->Running = false;
	UE_LOG(SensorFusionLog, Log, TEXT("Kinect body data provider stopped."));
}



void FKinect2DataProvider::Exit()
{
	this->BFR->UnsubscribeFrameArrived(BodyDataEvent);
	delete[] this->BodyBuffer;
	UE_LOG(SensorFusionLog, Log, TEXT("Kinect body data provider exited."));
}


#include "HideWindowsPlatformTypes.h"