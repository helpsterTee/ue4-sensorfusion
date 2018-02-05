#include "SensorFusionCorePCH.h"

#include "OneEuroFilter.h"


USensorFusionBone *UOneEuroFilter::ApplyFilter(USensorFusionBone* Bone)
{
	float dp, dr, dy;
	if (firstTime)
	{
		firstTime = false;
		dp = 0;
		dr = 0;
		dy = 0;
	}
	else
	{
		dp = (Bone->Orientation.Pitch - pfilt.hatprev())*rate;
		dr = (Bone->Orientation.Roll - rfilt.hatprev())*rate;
		dy = (Bone->Orientation.Yaw - yfilt.hatprev())*rate;
	}
	// Pitch
	float edp = dpfilt.filter(dp, ComputeAlpha(dcutoff));
	float cutoff = mincutoff + beta*FMath::Abs(edp);
	Bone->Orientation.Pitch = pfilt.filter(Bone->Orientation.Pitch, ComputeAlpha(cutoff));
	// Yaw
	float edr = drfilt.filter(dr, ComputeAlpha(dcutoff));
	cutoff = mincutoff + beta*FMath::Abs(edr);
	Bone->Orientation.Roll = rfilt.filter(Bone->Orientation.Roll, ComputeAlpha(cutoff));
	// Roll
	float edy = dyfilt.filter(dy, ComputeAlpha(dcutoff));
	cutoff = mincutoff + beta*FMath::Abs(edy);
	Bone->Orientation.Yaw = yfilt.filter(Bone->Orientation.Yaw, ComputeAlpha(cutoff));

	return Bone;
}

void UOneEuroFilter::Reset()
{
	firstTime = true;
	pfilt.reset();
	dpfilt.reset();
	rfilt.reset();
	drfilt.reset();
	yfilt.reset();
	dyfilt.reset();
}