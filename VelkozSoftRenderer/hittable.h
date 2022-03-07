#pragma once
#include"utility.h"
#include"aabb.h"

class material;

struct hit_record {
	point3 p;
	/// <summary>
	/// should be normalized
	/// </summary>
	vec3 normal;
	shared_ptr<material> mat_ptr;
	/// <summary>
	/// ray factor t
	/// </summary>
	double t;
	/// <summary>
	/// uv coord
	/// </summary>
	double u, v;
	/// <summary>
	/// true if hit the front face
	/// </summary>
	bool front_face;

	/// <summary>
	/// 
	/// </summary>
	/// <param name="r">ray hitting the surface</param>
	/// <param name="outward_normal"></param>
	inline void set_face_normal(const ray& r, const vec3& outward_normal) {
		front_face = dot(r.dir, outward_normal) < 0;
		normal = front_face ? outward_normal : -outward_normal;
	}
};

class hittable {
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="r"></param>
	/// <param name="t_min">valid infimum</param>
	/// <param name="t_max">valid supremum</param>
	/// <param name="rec"></param>
	/// <returns></returns>
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
	/// <summary>
	/// 
	/// </summary>
	/// <param name="t0">time start</param>
	/// <param name="t1">time end</param>
	/// <param name="output_box"></param>
	/// <returns></returns>
	virtual bool generate_bounding_box(double t0, double t1, aabb& output_box) const = 0;
	/// <summary>
	/// get p in dir. v at location o
	/// </summary>
	/// <param name="o"></param>
	/// <param name="v"></param>
	/// <returns></returns>
	virtual double get_probability(const point3& o, const vec3& v) const {
		return 0.0;
	}

	virtual vec3 random(const vec3& o) const {
		return vec3(1, 0, 0);
	}
};

class translate : public hittable {
public:
	translate(shared_ptr<hittable> p, const vec3& displacement)
		: ptr(p), offset(displacement){ }
	/// <summary>
	/// moving ray in the opposite dir
	/// instead of moving the object
	/// </summary>
	/// <param name="r"></param>
	/// <param name="t_min"></param>
	/// <param name="t_max"></param>
	/// <param name="rec"></param>
	/// <returns></returns>
	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	/// <summary>
	/// generate the BB after translation
	/// </summary>
	/// <param name="t0"></param>
	/// <param name="t1"></param>
	/// <param name="output_box"></param>
	/// <returns></returns>
	virtual bool generate_bounding_box(double t0, double t1, aabb& output_box) const override;

public:
	/// <summary>
	/// origin object ptr
	/// </summary>
	shared_ptr<hittable> ptr;
	/// <summary>
	/// object translation for calculation
	/// </summary>
	vec3 offset;
};

bool translate::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	ray moved_r(r.o - offset, r.dir, r.time);
	if (!ptr->hit(moved_r, t_min, t_max, rec))
		return false;

	rec.p += offset;
	rec.set_face_normal(moved_r, rec.normal);
	return true;
}

bool translate::generate_bounding_box(double t0, double t1, aabb& output_box) const {
	if (!ptr->generate_bounding_box(t0, t1, output_box))
		return false;

	output_box = aabb(
		output_box.minimum + offset,
		output_box.maximum + offset
	);

	return true;
}

class rotate_y : public hittable {
public:
	/// <summary>
	/// follow the right-hand screw rule
	/// </summary>
	/// <param name="p"></param>
	/// <param name="angle"></param>
	rotate_y(shared_ptr<hittable> p, double angle);

	virtual bool hit(
		const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool generate_bounding_box(double t0, double t1, aabb& output_box) const override {
		output_box = bb;
		return hasbox;
	}

public:
	shared_ptr<hittable> ptr;
	double sin_theta;
	double cos_theta;
	bool hasbox;
	aabb bb;
};

rotate_y::rotate_y(shared_ptr<hittable> p, double angle)
	: ptr(p)
{
	auto radians = degrees_to_radians(angle);
	sin_theta = sin(radians);
	cos_theta = cos(radians);
	hasbox = ptr->generate_bounding_box(0, 1, bb);

	point3 min( infinity,  infinity,  infinity);
	point3 max(-infinity, -infinity, -infinity);

	for (int i = 0; i < 2; ++i)
		for (int j = 0; j < 2; ++j)
			for (int k = 0; k < 2; ++k) {
				auto x = i * bb.maximum.x + (1 - i) * bb.minimum.x;
				auto y = j * bb.maximum.y + (1 - j) * bb.minimum.y;
				auto z = k * bb.maximum.z + (1 - k) * bb.minimum.z;

				auto newx =  cos_theta * x + sin_theta * z;
				auto newz = -sin_theta * x + cos_theta * z;

				vec3 tester(newx, y, newz);

				for (int c = 0; c < 3; ++c) {
					min[c] = fmin(min[c], tester[c]);
					max[c] = fmax(max[c], tester[c]);
				}
			}

	bb = aabb(min, max);
}

bool rotate_y::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	auto o   = r.o;
	auto dir = r.dir;
	o.x   = cos_theta * r.o.x   - sin_theta * r.o.z;
	o.z   = sin_theta * r.o.x   + cos_theta * r.o.z;
	dir.x = cos_theta * r.dir.x - sin_theta * r.dir.z;
	dir.z = sin_theta * r.dir.x + cos_theta * r.dir.z;

	ray rotated_r(o, dir, r.time);
	if (!ptr->hit(rotated_r, t_min, t_max, rec))
		return false;

	auto p      = rec.p;
	auto normal = rec.normal;
	p.x      =  cos_theta * rec.p.x      + sin_theta * rec.p.z;
	p.z      = -sin_theta * rec.p.x      + cos_theta * rec.p.z;
	normal.x =  cos_theta * rec.normal.x + sin_theta * rec.normal.z;
	normal.z = -sin_theta * rec.normal.x + cos_theta * rec.normal.z;
	
	rec.p = p;
	rec.set_face_normal(rotated_r, normal);

	return true;
}

class flip_face : public hittable {
public:
	flip_face(shared_ptr<hittable> p) : ptr(p) {}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
		if (!ptr->hit(r, t_min, t_max, rec))
			return false;

		rec.front_face = !rec.front_face;
		return true;
	}

	virtual bool generate_bounding_box(double t0, double t1, aabb& output_box) const override {
		return ptr->generate_bounding_box(t0, t1, output_box);
	}

public:
	shared_ptr<hittable> ptr;
};