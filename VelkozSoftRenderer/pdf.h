#pragma once
#include"utility.h"
#include"vec3.h"
#include"onb.h"


inline vec3 random_to_sphere(double radius, double distance_squared) {
	auto r1 = random_double();
	auto r2 = random_double();
	auto z = 1 + r2 * (sqrt(1 - radius * radius / distance_squared) - 1);
	auto phi = 2 * pi * r1;
	auto x = cos(phi) * sqrt(1 - z * z);
	auto y = sin(phi) * sqrt(1 - z * z);
	return vec3(x, y, z);
}

class pdf {
public:
	virtual ~pdf() {}

	/// <summary>
	/// the probability of given dir
	/// </summary>
	/// <param name="dir"></param>
	/// <returns></returns>
	virtual double get_probability(const vec3& dir) const = 0;

	/// <summary>
	/// generate a sample with this distribution
	/// </summary>
	/// <returns></returns>
	virtual vec3 generate() const = 0;
};

class cosine_pdf : public pdf {
public:
	cosine_pdf(const vec3& w) {
		uvw.build_from_w(w);
	}

	virtual double get_probability(const vec3& dir) const override {
		auto cosine = dot(normalize(dir), uvw.w);
		return (cosine <= 0) ? 0 : cosine / pi;
	}

	virtual vec3 generate() const override {
		return uvw.local(random_cosine_direction());
	}

public:
	onb uvw;
};

class hittable_pdf : public pdf {
public:
	hittable_pdf(shared_ptr<hittable> p, const point3& origin) : ptr(p), o(origin) {}

	virtual double get_probability(const vec3& dir) const override {
		return ptr->get_probability(o, dir);
	}

	virtual vec3 generate() const override {
		return ptr->random(o);
	}

public:
	point3 o;
	shared_ptr<hittable> ptr;
};

class mixture_pdf : public pdf {
public:
	mixture_pdf(shared_ptr<pdf> p0, shared_ptr<pdf> p1) {
		p[0] = p0;
		p[1] = p1;
	}

	virtual double get_probability(const vec3& dir) const override {
		return 0.5 * p[0]->get_probability(dir) + 0.5 * p[1]->get_probability(dir);
	}

	virtual vec3 generate() const override {
		if (random_double() < 0.5)
			return p[0]->generate();
		else
			return p[1]->generate();
	}

public:
	shared_ptr<pdf> p[2];
};