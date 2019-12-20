#ifndef _KEY_H_
#define _KEY_H_

#include "MathGeoLib\Math\Quat.h"
#include "MathGeoLib\Math\float3.h"

class float3Key {
public:
	float3 value;
	double time;

	float3Key() {}
	float3Key(float3 v, double t);
};

class quatKey {
public:
	Quat value;
	double time;

	quatKey() {}
	quatKey(Quat v, double t);
};

#endif