#pragma once
#include"vec3.h"

class onb {
public:
	onb() {}

	inline vec3 operator[](int i) const { return axis[i]; }

	vec3 local(double a, double b, double c) const {
		return a * u + b * v + c * w;
	}

	vec3 local(const vec3& a) const {
		return a.x * u + a.y * v + a.z * w;
	}

	void build_from_w(const vec3&);

public:
	vec3 axis[3], u, v, w;
};

void onb::build_from_w(const vec3& n) {
	w = axis[2] = normalize(n);
	vec3 a = (fabs(w.x) > 0.9) ? vec3(0, 1, 0) : vec3(1, 0, 0);
	v = axis[1] = normalize(cross(w, a));
	u = axis[0] = cross(v, w);
}