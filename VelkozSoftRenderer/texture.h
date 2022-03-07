#pragma once
#include"utility.h"
#include"rt_stb_image.h"
#include"perlin.h"

class texture {
public:
	virtual color sample(double u, double v, const point3& p) const = 0;
};

class solid_color : public texture {
public:
	solid_color() {}
	solid_color(const color& _c) : c(_c) {}
	solid_color(double r, double g, double b)
		: solid_color(color(r, g, b)) {}

	virtual color sample(double u, double v, const point3& p) const override {
		return c;
	}

private:
	/// <summary>
	/// texture solid color
	/// </summary>
	color c;
};

class checker_texture : public texture {
public:
	checker_texture() {}
	checker_texture(shared_ptr<texture> _even, shared_ptr<texture> _odd)
		: odd(_odd), even(_even) { }
	checker_texture(const color& c1, const color& c2)
		: checker_texture(make_shared<solid_color>(c1), make_shared<solid_color>(c2)) {}

	virtual color sample(double u, double v, const point3& p) const override {
		auto sines = sin(10 * p.x) * sin(10 * p.y) * sin(10 * p.z);
		if (sines < 0) return odd->sample(u, v, p);
		else return even->sample(u, v, p);
	}

public:
	shared_ptr<texture> odd;
	shared_ptr<texture> even;
};

class noise_texture : public texture {
public:
	noise_texture() {}
	noise_texture(double sc) : scale(sc) {}

	virtual color sample(double u, double v, const point3& p) const override {
		return color(1, 1, 1) * 0.5 * (1.0 + sin(scale * p.z + 10 * noise.turbulence(p)));
	}

public:
	/// <summary>
	/// noise generator
	/// </summary>
	perlin noise;
	/// <summary>
	/// control pattern frequency
	/// </summary>
	double scale;
};

class image_texture : public texture {
public:
	static const int bytes_per_pixel = 3;

	image_texture()
		: data(nullptr), width(0), height(0), bytes_per_scanline(0) {}

	image_texture(const char* filename) {
		auto components_per_pixel = bytes_per_pixel;

		data = stbi_load(filename, &width, &height, &components_per_pixel, components_per_pixel);
		if (!data) {
			std::cerr << "ERROR: Could NOT load texture image file '" << filename << "'.\n";
			width = height = 0;
		}
		bytes_per_scanline = bytes_per_pixel * width;
	}

	~image_texture() {
		delete data;
	}

	virtual color sample(double u, double v, const point3& p) const override {
		u = clamp(u, 0.0, 1.0);
		v = 1.0 - clamp(v, 0.0, 1.0);
		auto i = static_cast<int>(u * width);
		auto j = static_cast<int>(v * height);
		// clamp
		if (i >= width) i = width - 1;
		if (j >= height) j = height - 1;

		const auto color_scale = 1.0 / 255.0;
		// i in [0, width), j in [0, height)
		// get pixel data by the following expression
		auto pixel = data + j * bytes_per_scanline + i * bytes_per_pixel;
		return color(color_scale * pixel[0], color_scale * pixel[1], color_scale * pixel[2]);
	}

private:
	unsigned char* data;
	int width, height;
	/// <summary>
	/// bytes count of one image line in data
	/// </summary>
	int bytes_per_scanline;
};