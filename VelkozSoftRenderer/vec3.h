#pragma once
#include<cmath>
#include<iostream>

using std::sqrt;

class vec3 {
public:
	vec3() : x{ 0 }, y{ 0 }, z{ 0 }, length_squared{ 0 }, length{ 0 } {}
	vec3(double _x, double _y, double _z) : x{ _x }, y{ _y }, z{ _z }, length_squared{ x*x+y*y+z*z }, length{ sqrt(length_squared) } {}

	inline static vec3 random() {
		return vec3(random_double(), random_double(), random_double());
	}

	inline static vec3 random(double min, double max) {
		return vec3(random_double(min, max), random_double(min, max), random_double(min, max));
	}

public:
	/// <summary>
	/// check if near zero vector
	/// </summary>
	/// <returns></returns>
	bool near_zero() const {
		const auto s = 1e-8;
		return (fabs(x) < s) && (fabs(y) < s) && (fabs(z) < s);
	}

	vec3 operator-() const {
		return vec3(-x, -y, -z);
	}

	double operator[](int i) const {
		if (i == 0) {
			return x;
		} else if (i == 1) {
			return y;
		} else {
			return z;
		}
	}

	double& operator[](int i) {
		if (i == 0) {
			return x;
		} else if (i == 1) {
			return y;
		} else {
			return z;
		}
	}

	vec3& operator+=(const vec3& v) {
		x += v.x;
		y += v.y;
		z += v.z;
		length_squared = x * x + y * y + z * z;
		length = sqrt(length_squared);
		return *this;
	}

	vec3& operator-=(const vec3& v) {
		return *this += (-v);
	}

	vec3& operator*=(const double d) {
		x *= d;
		y *= d;
		z *= d;
		length_squared *= d * d;
		length *= d;
		return *this;
	}

	vec3& operator/=(const double d) {
		return *this *= 1 / d;
	}

public:
	double x, y, z, length_squared, length;
};

using point3 = vec3;
using color = vec3;

inline std::ostream& operator<<(std::ostream& out, const vec3& v) {
	return out << v.x << ' ' << v.y << ' ' << v.z;
}

inline vec3 operator+(const vec3& u, const vec3& v) {
	return vec3(u.x + v.x, u.y + v.y, u.z + v.z);
}

inline vec3 operator-(const vec3& u, const vec3& v) {
	return vec3(u.x - v.x, u.y - v.y, u.z - v.z);
}

inline vec3 operator*(const vec3& u, const vec3& v) {
	return vec3(u.x * v.x, u.y * v.y, u.z * v.z);
}

inline vec3 operator*(double d, const vec3& v) {
	return vec3(v.x * d, v.y * d, v.z * d);
}

inline vec3 operator*(const vec3& v, double d) {
	return vec3(v.x * d, v.y * d, v.z * d);
}

inline vec3 operator/(const vec3& v, double d) {
	return (1 / d) * v;
}

inline double dot(const vec3& u, const vec3& v) {
	return u.x * v.x + u.y * v.y + u.z * v.z;
}

inline vec3 cross(const vec3& u, const vec3& v) {
	return vec3(
		u.y * v.z - u.z * v.y,
		u.z * v.x - u.x * v.z,
		u.x * v.y - u.y * v.x
	);
}

inline vec3 normalize(const vec3& v) {
	return v / v.length;
}

inline vec3 random_in_unit_sphere() {
	while (true) {
		auto p = vec3::random(-1, 1);
		if (p.length_squared >= 1) continue;
		return p;
	}
}

inline vec3 random_unit_vector() {
	return normalize(random_in_unit_sphere());
}

inline vec3 random_in_hemisphere(const vec3& normal) {
	vec3 vec_in_unit_sphere = random_in_unit_sphere();
	if (dot(vec_in_unit_sphere, normal) < 0.0) vec_in_unit_sphere = -vec_in_unit_sphere;
	return vec_in_unit_sphere;
}

inline vec3 random_in_unit_disk() {
	while (true) {
		auto p = vec3(random_double(-1, 1), random_double(-1, 1), 0);
		if (p.length_squared >= 1) continue;
		return p;
	}
}

inline vec3 random_cosine_direction() {
	auto r1 = random_double();
	auto r2 = random_double();
	auto z = sqrt(1 - r2);

	auto phi = 2 * pi * r1;
	auto x = cos(phi) * sqrt(r2);
	auto y = sin(phi) * sqrt(r2);

	return vec3(x, y, z);
}

/// <summary>
/// 
/// </summary>
/// <param name="v">points in, should be normalized</param>
/// <param name="n">surface normal, should be normalized</param>
/// <returns>already normalized</returns>
vec3 reflect(const vec3& v, const vec3& n) {
	return v - 2 * dot(v, n) * n;
}

/// <summary>
/// 
/// </summary>
/// <param name="v">points in, should be normalized</param>
/// <param name="n">surface normal, should be normalized</param>
/// <param name="etai_over_etat">in refractive index is the numerator</param>
/// <returns>already normalized</returns>
vec3 refract(const vec3& v, const vec3& n, double etai_over_etat) {
	auto cos_theta = fmin(dot(-v, n), 1.0);
	vec3 r_out_perp = etai_over_etat * (v + cos_theta * n);
	vec3 r_out_para = -sqrt(fabs(1.0 - r_out_perp.length_squared)) * n;
	return r_out_perp + r_out_para;
}