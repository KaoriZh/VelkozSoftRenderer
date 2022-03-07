#pragma once
#include<algorithm>
#include"utility.h"
#include"hittable.h"
#include"hittable_list.h"

/// <summary>
/// if just have one element it'll duplicated in each subtree
/// </summary>
class bvh_node : public hittable {
public:
	bvh_node() {}
	bvh_node(const hittable_list& list, double t0, double t1)
		: bvh_node(list.objects, 0, list.objects.size(), t0, t1) {}
	bvh_node(const std::vector<shared_ptr<hittable>>& _objects, size_t start, size_t end, double t0, double t1);

	virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override;
	virtual bool generate_bounding_box(double time0, double time1, aabb& output_box) const override;

public:
	shared_ptr<hittable> left;
	shared_ptr<hittable> right;
	aabb box;
};

/// <summary>
/// aabb sort comparator using minimum and axis
/// </summary>
/// <param name="a"></param>
/// <param name="b"></param>
/// <param name="axis"></param>
/// <returns></returns>
inline bool box_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b, int axis) {
	aabb box_a;
	aabb box_b;
	if (!a->generate_bounding_box(0, 0, box_a) || !b->generate_bounding_box(0, 0, box_b)) {
		std::cerr << "No BB in bvh_node ctor.\n";
	}
	return box_a.minimum[axis] < box_b.minimum[axis];
}

bool box_x_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 0);
}

bool box_y_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 1);
}

bool box_z_compare(const shared_ptr<hittable> a, const shared_ptr<hittable> b) {
	return box_compare(a, b, 2);
}

bvh_node::bvh_node(const std::vector<shared_ptr<hittable>>& _objects, size_t start, size_t end, double t0, double t1) {
	auto objects = _objects;
	int axis = random_int(0, 2);
	auto comparator = (axis == 0) ? box_x_compare 
					: (axis == 1) ? box_y_compare 
					:				box_z_compare;

	size_t object_span = end - start;

	if (object_span == 1) {
		left = right = objects[start];
	} else if (object_span == 2) {
		if (comparator(objects[start], objects[start + 1])) {
			left = objects[start];
			right = objects[start + 1];
		} else {
			left = objects[start + 1];
			right = objects[start];
		}
	} else {
		std::sort(objects.begin() + start, objects.begin() + end, comparator);
		auto mid = start + object_span / 2;
		left = make_shared<bvh_node>(objects, start, mid, t0, t1);
		right = make_shared<bvh_node>(objects, mid, end, t0, t1);
	}

	aabb box_left, box_right;
	if (!left->generate_bounding_box(t0, t1, box_left) ||
		!right->generate_bounding_box(t0, t1, box_right)) {
		std::cerr << "No BB in bvh_node ctor.\n";
	}
	box = aabb_union(box_left, box_right);
}

bool bvh_node::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
	if (!box.hit(r, t_min, t_max)) return false;
	bool hit_left = left->hit(r, t_min, t_max, rec);
	// IMPORTANT
	bool hit_right = right->hit(r, t_min, hit_left ? rec.t : t_max, rec);
	return hit_left || hit_right;
}

bool bvh_node::generate_bounding_box(double time0, double time1, aabb& output_box) const {
	output_box = box;
	return true;
}