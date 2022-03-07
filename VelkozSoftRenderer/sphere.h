#pragma once
#include"hittable.h"
#include"vec3.h"
#include"onb.h"
#include"pdf.h"

class sphere : public hittable {
public:
	sphere(){}
	sphere(const point3& cen, double r, shared_ptr<material> m) : center(cen), radius(r), mat_ptr(m) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool generate_bounding_box(double t0, double t1, aabb& output_box) const override;
	virtual double get_probability(const point3& o, const vec3& v) const override;
	virtual vec3 random(const vec3& o) const override;


private:
	/// <summary>
	/// (1,0,0)->( 0.5,0.5)
	/// (0,1,0)->( 0.5,1.0)
	/// (0,0,1)->(0.25,0.5)
	/// </summary>
	/// <param name="p">a given point on the sphere of radius one, centered at the origin</param>
	/// <param name="u">returned value [0,1] of angle around the y-axis from x=-1</param>
	/// <param name="v">returned value [0,1] of angle from y=-1 to y=+1</param>
	static void get_uv(const point3& p, double& u, double& v) {
		auto theta = acos(-p.y);
		auto phi = atan2(-p.z, p.x) + pi;
		u = phi / (2 * pi);
		v = theta / pi;
	}

public:
	point3 center;
	double radius;
	shared_ptr<material> mat_ptr;
};

bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	vec3 oc = r.o - center;
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
	vec3 normal = (rec.p - center) / radius;
	rec.set_face_normal(r, normal);
	get_uv(normal, rec.u, rec.v);
	rec.mat_ptr = mat_ptr;

	return true;
}

bool sphere::generate_bounding_box(double t0, double t1, aabb& output_box) const {
	output_box = aabb(
		center - vec3(radius, radius, radius), 
		center + vec3(radius, radius, radius)
	);
	return true;
}

double sphere::get_probability(const point3& o, const vec3& v) const {
	hit_record rec;
	if (!this->hit(ray(o, v), 0.001, infinity, rec))
		return 0;

	auto cos_theta_max = sqrt(1 - radius * radius / (center - o).length_squared);
	auto solid_angle = 2 * pi * (1 - cos_theta_max);
	return 1 / solid_angle;
}

vec3 sphere::random(const vec3& o) const {
	vec3 dir = center - o;
	auto dis_squared = dir.length_squared;
	onb uvw;
	uvw.build_from_w(dir);
	return uvw.local(random_to_sphere(radius, dis_squared));
}