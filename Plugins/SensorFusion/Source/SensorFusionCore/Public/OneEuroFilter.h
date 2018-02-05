#pragma once

#include "Engine.h"

#include "SensorFusionBone.h"

#include "OneEuroFilter.generated.h"



//! @brief A very simple, low-latency filter to smooth real-time bone data streams.
//! @details Implementation according to this paper: https://hal.inria.fr/hal-00670496/file/CHI2012-casiez.pdf
//! @ingroup sffilters
UCLASS(ClassGroup = "Sensor Fusion|Filter")
class SENSORFUSIONCORE_API UOneEuroFilter : public USensorFusionBoneFilter
{
	GENERATED_BODY()

private:
	//! Internal helper class.
	class LowPassFilter 
	{
	private:
		bool firstTime = true;
		float xhatprev;

	public:
		float hatprev()const { return xhatprev; }

		void reset() { firstTime = true; }

		float filter(float x, float alpha)
		{
			if (firstTime)
			{
				firstTime = false;
				xhatprev = x;
			}
			float xhat = alpha*x + (1 - alpha)*xhatprev;
			xhatprev = xhat;
			return xhat;
		}
	};


	float ComputeAlpha(float cutoff)
	{
		float tau = 1.0 / (2 * PI *cutoff);
		float te = 1.0 / this->rate;
		return 1.0 / (1.0 + tau / te);
	}

	bool firstTime = true;

	//Pitch filter
	LowPassFilter pfilt;
	LowPassFilter dpfilt;
	//Roll filter
	LowPassFilter rfilt;
	LowPassFilter drfilt;
	//Yaw filter
	LowPassFilter yfilt;
	LowPassFilter dyfilt;

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = 0.000001))
	float mincutoff = 1.0;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = 0.000001))
	float dcutoff = 1.0;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float beta = 0.05;


	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (ClampMin = 0.000001))
	float rate = 1.0/30.0;


	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Filter")
	USensorFusionBone* ApplyFilter(USensorFusionBone* Bone) override;


	UFUNCTION(BlueprintCallable, Category = "Sensor Fusion|Filter")
	void Reset() override;
};