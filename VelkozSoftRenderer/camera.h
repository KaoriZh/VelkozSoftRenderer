#pragma once
#include"utility.h"

class camera {
public:
	/// <summary>
	/// camera facing -z in view space
	/// </summary>
	/// <param name="lookfrom"></param>
	/// <param name="lookat"></param>
	/// <param name="up"></param>
	/// <param name="vertical_fov">in degrees</param>
	/// <param name="aspect_ratio">width over height</param>
	/// <param name="aperture">the diameter of the hole to control how big the lens is effectively</param>
	/// <param name="focus_dist">distance between the lens and the focus plane</param>
	/// <param name="t_on">time shutter on</param>
	/// <param name="t_off">time shutter off</param>
	camera(point3 lookfrom, point3 lookat, vec3 up, double vertical_fov, double aspect_ratio, double aperture, double focus_dist, double t_on = 0.0, double t_off = 0.0) {
		auto theta = degrees_to_radians(vertical_fov);
		auto h = tan(theta / 2);
		auto viewport_height = 2.0 * h;
		auto viewport_width = aspect_ratio * viewport_height;

		w = normalize(lookfrom - lookat);
		u = normalize(cross(up, w));
		v = cross(w, u);

		origin = lookfrom;
		horizontal = focus_dist * viewport_width * u;
		vertical = focus_dist * viewport_height * v;
		lower_left_corner = origin - horizontal / 2 - vertical / 2 - focus_dist * w;
		
		lens_radius = aperture / 2;
		time_on = t_on;
		time_off = t_off;
	}

	/// <summary>
	/// generates a ray using screen uv coordinate
	/// </summary>
	/// <param name="u"></param>
	/// <param name="v"></param>
	/// <returns></returns>
	ray generate_ray(double s, double t) const {
		vec3 rd = lens_radius * random_in_unit_disk();
		vec3 offset = u * rd.x + v * rd.y;
		return ray(origin + offset, lower_left_corner + s * horizontal + t * vertical - origin - offset, random_double(time_on, time_off));
	}

private:
	point3 origin;
	point3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
	vec3 u, v, w;
	double lens_radius;
	/// <summary>
	/// shutter open/close times
	/// </summary>
	double time_on, time_off;
};