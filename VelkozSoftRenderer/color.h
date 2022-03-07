#pragma once
#include"vec3.h"

color random_color(
	double r_min = 0.0, double r_max = 1.0,
	double g_min = 0.0, double g_max = 1.0,
	double b_min = 0.0, double b_max = 1.0
) {
	return color(random_double(r_min, r_max), random_double(g_min, g_max), random_double(b_min, b_max));
}

/// <summary>
/// 
/// </summary>
/// <param name="out"></param>
/// <param name="c">pixel color</param>
/// <param name="spp">samples per pixel</param>
void write_color(std::ostream& out, color c, int spp) {
	auto r = c.x;
	auto g = c.y;
	auto b = c.z;

	if (r != r) r = 0.0;
	if (g != g) g = 0.0;
	if (b != b) b = 0.0;

	auto scale = 1.0 / spp;
	r = sqrt(scale * r);
	g = sqrt(scale * g);
	b = sqrt(scale * b);

	out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
		<< static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}