#pragma once
#include"hittable.h"
#include"aabb.h"
#include<vector>
#include<memory>

using std::shared_ptr;
using std::make_shared;

class hittable_list : public hittable {
public:
	hittable_list(){}
	hittable_list(shared_ptr<hittable> obj) {
		add(obj);
	}

	void clear() {
		objects.clear();
	}

	void add(shared_ptr<hittable> object) {
		objects.push_back(object);
	}

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool generate_bounding_box(double t0, double t1, aabb& output_box) const override;
	virtual double get_probability(const point3& o, const vec3& v) const override;
	virtual vec3 random(const vec3& o) const override;

public:
	std::vector<shared_ptr<hittable>> objects;
};

bool hittable_list::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	hit_record tmp_rec;
	bool hit_anything = false;
	auto closest_so_far = t_max;

	for (const auto& obj : objects) {
		if (obj.get()->hit(r, t_min, closest_so_far, tmp_rec)) {
			hit_anything = true;
			closest_so_far = tmp_rec.t;
			rec = tmp_rec;
		}
	}

	return hit_anything;
}

bool hittable_list::generate_bounding_box(double t0, double t1, aabb& output_box) const {
	if (objects.empty()) return false;

	aabb a;
	bool first = true;
	for (const auto& obj : objects) {
		if (!obj->generate_bounding_box(t0, t1, a)) return false;
		output_box = first ? a : aabb_union(output_box, a);
		first = false;
	}

	return true;
}

double hittable_list::get_probability(const point3& o, const vec3& v) const {
	auto weight = 1.0 / objects.size();
	auto sum = 0.0;
	for (const auto& obj : objects) {
		sum += weight * obj->get_probability(o, v);
	}
	return sum;
}

vec3 hittable_list::random(const vec3& o) const {
	auto int_size = static_cast<int>(objects.size());
	return objects[random_int(0, int_size - 1)]->random(o);
}