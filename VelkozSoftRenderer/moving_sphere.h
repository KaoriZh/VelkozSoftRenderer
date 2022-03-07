#pragma once
#include"utility.h"
#include"hittable.h"

class moving_sphere : public hittable {
public:
	moving_sphere() {}
	moving_sphere(point3 c0, point3 c1, double t0, double t1, double r, shared_ptr<material> m)
		: center0(c0), center1(c1), time0(t0), time1(t1), radius(r), mat_ptr(m) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool generate_bounding_box(double t0, double t1, aabb& output_box) const override;

	point3 center(double time) const;

public:
	point3 center0, center1;
	double time0, time1;
	double radius;
	shared_ptr<material> mat_ptr;
};

bool moving_sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 oc = r.o - center(r.time);
	auto a = r.dir.length_squared;
	auto half_b = dot(oc, r.dir);
	auto c = oc.length_squared - radius * radius;

	auto discriminant = half_b * half_b - a * c;
	if (discriminant < 0) return false;
	auto sqrtd = sqrt(discriminant);

	auto root = (-half_b - sqrtd) / a;
	if (root < t_min || t_max < root) {
		root = (-half_b + sqrtd) / a;
		if (root < t_min || t_max < root) return false;
	}

	rec.t = root;
	rec.p = r.at(rec.t);
	vec3 normal = (rec.p - center(r.time)) / radius;
	rec.set_face_normal(r, normal);
	rec.mat_ptr = mat_ptr;

	return true;
}

bool moving_sphere::generate_bounding_box(double t0, double t1, aabb& output_box) const {
	aabb a(
		center(t0) - vec3(radius, radius, radius),
		center(t0) + vec3(radius, radius, radius)
	);
	aabb b(
		center(t1) - vec3(radius, radius, radius),
		center(t1) + vec3(radius, radius, radius)
	);
	output_box = aabb_union(a, b);
	return true;
}

point3 moving_sphere::center(double time) const {
	return center0 + ((time - time0) / (time1 - time0)) * (center1 - center0);
}