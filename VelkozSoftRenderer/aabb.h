#pragma once
#include"utility.h"

class aabb {
public:
	aabb() {}
	aabb(const point3& min, const point3& max) : minimum(min), maximum(max) {}

	bool hit(const ray& r, double t_min, double t_max) const {
		for (int i = 0; i < 3; ++i) {
			auto invD = 1.0f / r.dir[i];
			auto t0 = (minimum[i] - r.o[i]) * invD;
			auto t1 = (maximum[i] - r.o[i]) * invD;
			if (invD < 0.0f) std::swap(t0, t1);
			t_min = t0 > t_min ? t0 : t_min;
			t_max = t1 < t_max ? t1 : t_max;
			if (t_max <= t_min) return false;
		}
		return true;
	}

public:
	point3 minimum;
	point3 maximum;
};

aabb aabb_union(const aabb& a, const aabb& b) {
	point3 min(
		fmin(a.minimum.x, b.minimum.x),
		fmin(a.minimum.y, b.minimum.y),
		fmin(a.minimum.z, b.minimum.z) 
	);
	point3 max(
		fmax(a.maximum.x, b.maximum.x),
		fmax(a.maximum.y, b.maximum.y),
		fmax(a.maximum.z, b.maximum.z)
	);
	return aabb(min, max);
}