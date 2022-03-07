#pragma once
#include<cmath>
#include<limits>
#include<memory>
#include<random>
#include<chrono>
#include<iostream>
#include<string>

#define DEBUG
#ifdef DEBUG
//#define DEBUG_IMAGE_SUB_AREA
#endif // DEBUG

#define OMP

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

inline double degrees_to_radians(double degrees) {
	return degrees * pi / 180.0;
}

/// <summary>
/// returns a random real in [0,1)
/// </summary>
/// <returns></returns>
inline double random_double() {
	static std::uniform_real_distribution<double> distribution(0.0, 1.0);
	static std::mt19937 generator;
	return distribution(generator);
}

/// <summary>
/// returns a random real in [min,max)
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
/// <returns></returns>
inline double random_double(double min, double max) {
	return min + (max - min) * random_double();
}

/// <summary>
/// returns a random int in [min,max]
/// </summary>
/// <param name="min"></param>
/// <param name="max"></param>
/// <returns></returns>
inline int random_int(int min, int max) {
	return static_cast<int>(random_double(min, max + 1));
}

inline double clamp(double x, double min, double max) {
	if (x < min)return min;
	if (x > max)return max;
	return x;
}

static std::chrono::system_clock::time_point time_start;
static std::string time_description;

inline void timer_start(const char* description) {
	time_start = std::chrono::system_clock::now();
	time_description = description;
}

inline void timer_tick() {
	std::chrono::system_clock::time_point time_end = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
	// time in seconds
	auto time = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	int h = static_cast<int>(time / 3600);
	time -= 3600.0 * h;
	int m = static_cast<int>(time / 60);
	time -= 60.0 * m;
	std::cerr << "Time cost: " << h << " h " << m << " min " << time << " s. ";
}

inline void timer_end() {
	std::chrono::system_clock::time_point time_end = std::chrono::system_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start);
	// time in seconds
	auto time = double(duration.count()) * std::chrono::microseconds::period::num / std::chrono::microseconds::period::den;
	int h = static_cast<int>(time / 3600);
	time -= 3600.0 * h;
	int m = static_cast<int>(time / 60);
	time -= 60.0 * m;
	std::cerr << time_description << ": " << h << " h " << m << " min " << time << " s\n";
}

#include"ray.h"
#include"vec3.h"