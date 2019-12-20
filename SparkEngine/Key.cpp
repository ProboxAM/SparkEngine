#include "Key.h"

float3Key::float3Key(float3 v, double t)
{
	value = v;
	time = t;
}

quatKey::quatKey(Quat v, double t)
{
	value = v;
	time = t;
}
