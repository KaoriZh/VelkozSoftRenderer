#pragma once
#include"vec3.h"

class ray {
public:
	ray(){}
	ray(const point3& origin, const vec3& direction, double tm = 0.0) : o(origin), dir(direction), time(tm) {}

	point3 at(double t) const {
		return o + t * dir;
	}

public:
	point3 o;
	vec3 dir;
	/// <summary>
	/// when the ray is generated while the shutter is open
	/// </summary>
	double time;
};