#pragma once 

#include "HitProxies.h"

struct HSensorFusionBoneProxy : public HHitProxy
{
	DECLARE_HIT_PROXY();

	FName BoneName;

	explicit HSensorFusionBoneProxy(const FName& InBoneName)
		: BoneName(InBoneName)
	{}
};