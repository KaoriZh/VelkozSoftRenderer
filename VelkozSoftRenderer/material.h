#pragma once
#include"utility.h"
#include"hittable.h"
#include"pdf.h"
#include"texture.h"
#include"onb.h"

struct scatter_record {
	ray specular_ray;
	bool is_specular;
	color attenuation;
	shared_ptr<pdf> pdf_ptr;
};

class material {
public:
	/// <summary>
	/// 
	/// </summary>
	/// <param name="r_in">indir</param>
	/// <param name="rec">hit point info</param>
	/// <param name="srec">scatter record</param>
	/// <returns>true if scattered</returns>
	virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const {
		return false;
	}

	virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const {
		return 0;
	}

	virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const {
		return color(0, 0, 0);
	}
};

class lambertian : public material {
public:
	lambertian(const color& c) : albedo(make_shared<solid_color>(c)) {}
	lambertian(shared_ptr<texture> a) : albedo(a) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		srec.is_specular = false;
		srec.attenuation = albedo->sample(rec.u, rec.v, rec.p);
		srec.pdf_ptr = make_shared<cosine_pdf>(rec.normal);
		return true;
	}

	virtual double scattering_pdf(const ray& r_in, const hit_record& rec, const ray& scattered) const override {
		auto cosine = dot(rec.normal, normalize(scattered.dir));
		return cosine < 0 ? 0 : cosine / pi;
	}

public:
	shared_ptr<texture> albedo;
};

class metal : public material {
public:
	metal(const color& c, double f) : albedo(c), fuzz(f < 1 ? f : 1) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		vec3 reflected = reflect(normalize(r_in.dir), rec.normal);
		srec.specular_ray = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		srec.attenuation = albedo;
		srec.is_specular = true;
		srec.pdf_ptr = nullptr;
		return true;
	}

public:
	color albedo;
	/// <summary>
	/// factor to randomize the unit reflected dir endpoint by using a small sphere
	/// visually it controls the roughness
	/// </summary>
	double fuzz;
};

/// <summary>
/// 电介质，通俗理解就是非金属
/// </summary>
class dielectric : public material {
public:
	dielectric(double _ir) : ir(_ir) {}

	virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_record& srec) const override {
		double refraction_ratio = rec.front_face ? (1.0 / ir) : ir;
		vec3 dir = normalize(r_in.dir);
		double cos_theta = fmin(dot(-dir, rec.normal), 1.0);
		double sin_theta = sqrt(1.0 - cos_theta * cos_theta);

		bool cannot_refract = refraction_ratio * sin_theta > 1.0;
		vec3 dir_scattered;

		if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
			dir_scattered = reflect(dir, rec.normal);
		} else {
			dir_scattered = refract(dir, rec.normal, refraction_ratio);
		}

		srec.specular_ray = ray(rec.p, dir_scattered, r_in.time);
		srec.is_specular = true;
		srec.pdf_ptr = nullptr;
		srec.attenuation = color(1.0, 1.0, 1.0);

		return true;
	}

private:
	/// <summary>
	/// Fresnel effect Schlick's approximation
	/// </summary>
	/// <param name="cosine"></param>
	/// <param name="ref_idx"></param>
	/// <returns></returns>
	static double reflectance(double cosine, double ref_idx) {
		auto r0 = (1 - ref_idx) / (1 + ref_idx);
		r0 *= r0;
		return r0 + (1 - r0) * pow(1 - cosine, 5);
	}

public:
	/// <summary>
	/// refractive index
	/// </summary>
	double ir;
};

class diffuse_light : public material {
public:
	diffuse_light(shared_ptr<texture> t) : emit(t) {}
	diffuse_light(const color& c) : emit(make_shared<solid_color>(c)) {}

	virtual color emitted(const ray& r_in, const hit_record& rec, double u, double v, const point3& p) const override {
		if (rec.front_face)
			return emit->sample(u, v, p);
		else
			return color(0, 0, 0);
	}

public:
	shared_ptr<texture> emit;
};