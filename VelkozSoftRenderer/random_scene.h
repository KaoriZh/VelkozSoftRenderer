#pragma once
#include"hittable_list.h"
#include"material.h"
#include"sphere.h"
#include"moving_sphere.h"
#include"aarect.h"
#include"box.h"
#include"constant_medium.h"
#include"bvh.h"

shared_ptr<hittable> random_light(
    double x_min = -100, double x_max = 500, 
    double y_min = 500, double y_max = 600, 
    double z_min = 200, double z_max = 400,
    double i_min = 5.0, double i_max = 10.0, 
    double s_min = 30, double s_max = 60
)
{
    shared_ptr<hittable> light;
    auto intensity = random_double(i_min, i_max);
    auto light_mat = make_shared<diffuse_light>(color(intensity, intensity, intensity));
    auto x = random_double(x_min, x_max);
    auto y = random_double(y_min, y_max);
    auto z = random_double(z_min, z_max);
    auto half_width = random_double(s_min / 2, s_max / 2);
    auto t = random_double();
    if (t < 0.1) light = make_shared<xy_rect>(x - half_width, x + half_width, y - half_width, y + half_width, z, light_mat);
    else if (t < 0.2) light = make_shared<yz_rect>(y - half_width, y + half_width, z - half_width, z + half_width, x, light_mat);
    else light = make_shared<xz_rect>(x - half_width, x + half_width, z - half_width, z + half_width, y, light_mat);
    return light;
}

shared_ptr<material> random_material() {
    shared_ptr<material> mat;
    auto t = random_double();
    if (t < 0.1) mat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    else if (t < 0.2) mat = make_shared<lambertian>(make_shared<noise_texture>(random_double(0.1, 1.0)));
    else if (t < 0.4) mat = make_shared<lambertian>(random_color());
    else if (t < 0.6) mat = make_shared<metal>(random_color(), random_double(0.5, 1.0));
    else mat = make_shared<dielectric>(random_double(1.2, 2.0));
    return mat;
}

shared_ptr<material> random_material_except_noise() {
    shared_ptr<material> mat;
    auto t = random_double();
    if (t < 0.2) mat = make_shared<lambertian>(make_shared<image_texture>("earthmap.jpg"));
    else if (t < 0.4) mat = make_shared<lambertian>(random_color());
    else if (t < 0.6) mat = make_shared<metal>(random_color(), random_double(0.5, 1.0));
    else mat = make_shared<dielectric>(random_double(1.2, 2.0));
    return mat;
}

shared_ptr<hittable> random_sphere(
    double x_min = -100, double x_max = 500,
    double y_min = 200, double y_max = 400,
    double z_min = 200, double z_max = 400,
    double r_min = 40, double r_max = 80
)
{
    shared_ptr<hittable> obj;
    auto mat = random_material();
    auto center = point3(random_double(x_min, x_max), random_double(y_min, y_max), random_double(z_min, z_max));
    auto r = random_double(r_min, r_max);
    obj = make_shared<sphere>(center, r, mat);
    return obj;
}

shared_ptr<hittable> random_moving_sphere(
    double x_min = -100, double x_max = 500,
    double y_min = 200, double y_max = 400,
    double z_min = 200, double z_max = 400,
    double r_min = 40, double r_max = 80,
    double o_min = -20, double o_max = 20
)
{
    shared_ptr<hittable> obj;
    auto mat = random_material();
    auto center = point3(random_double(x_min, x_max), random_double(y_min, y_max), random_double(z_min, z_max));
    auto r = random_double(r_min, r_max);
    auto offset = vec3(random_double(o_min, o_max), random_double(o_min, o_max), random_double(o_min, o_max));
    obj = make_shared<moving_sphere>(center, center + offset, 0, 1, r, mat);
    return obj;
}

shared_ptr<hittable> random_box(
    double x_min = -100, double x_max = 500,
    double y_min = 200, double y_max = 400,
    double z_min = 200, double z_max = 400,
    double s_min = 80, double s_max = 120,
    double a_min = -45, double a_max = 45
)
{
    shared_ptr<hittable> obj;
    auto mat = random_material();
    auto size = vec3(random_double(s_min, s_max), random_double(s_min, s_max), random_double(s_min, s_max));
    obj = make_shared<box>(point3(0, 0, 0), size, mat);
    auto angle = random_double(a_min, a_max);
    obj = make_shared<rotate_y>(obj, angle);
    auto dis = point3(random_double(x_min, x_max), random_double(y_min, y_max), random_double(z_min, z_max));
    obj = make_shared<translate>(obj, dis);
    return obj;
}

shared_ptr<hittable> random_object(
    double x_min = -100, double x_max = 500,
    double y_min = 200, double y_max = 400,
    double z_min = 200, double z_max = 400,
    double r_min = 40, double r_max = 80,
    double o_min = -20, double o_max = 20,
    double s_min = 80, double s_max = 120,
    double a_min = -45, double a_max = 45
) {
    shared_ptr<hittable> obj;
    auto x = random_double();
    if (x < 0.3) obj = random_box(x_min, x_max, y_min, y_max, z_min, z_max, s_min, s_max, a_min, a_max);
    else if (x < 0.4) obj = random_moving_sphere(x_min, x_max, y_min, y_max, z_min, z_max, r_min, r_max, o_min, o_max);
    else obj = random_sphere(x_min, x_max, y_min, y_max, z_min, z_max, r_min, r_max);
    return obj;
}

shared_ptr<hittable> random_object_except_box(
    double x_min = -100, double x_max = 500,
    double y_min = 200, double y_max = 400,
    double z_min = 200, double z_max = 400,
    double r_min = 40, double r_max = 80,
    double o_min = -20, double o_max = 20,
    double s_min = 80, double s_max = 120,
    double a_min = -45, double a_max = 45
) {
    shared_ptr<hittable> obj;
    auto x = random_double();
    if (x < 0.2) obj = random_moving_sphere(x_min, x_max, y_min, y_max, z_min, z_max, r_min, r_max, o_min, o_max);
    else obj = random_sphere(x_min, x_max, y_min, y_max, z_min, z_max, r_min, r_max);
    return obj;
}

hittable_list random_scene(const int light_count = 20, const int object_count = 40) {
    hittable_list objects;

    auto light = make_shared<diffuse_light>(color(7, 7, 7));
    objects.add(make_shared<xz_rect>(123, 423, 147, 412, 554, light));

    for (int i = 0; i < 48; i++) {
        for (int j = 0; j < 27; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;
            auto ground = make_shared<lambertian>(random_color());
            objects.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    for (int i = 0; i < object_count; ++i) {
        auto obj = random_object();
        objects.add(obj);
    }

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));
    
    auto node = make_shared<bvh_node>(objects, 0, 1);
    hittable_list root;
    root.add(node);
    return root;
}

hittable_list random_scene2(const int light_count = 20, const int object_count = 40) {

    const int x_min = -400, x_max = 800;
    const int z_min = 0, z_max = 600;
    const int y_min_light = 500, y_max_light = 600;
    const int y_min_object = 150, y_max_object = 450;
    const int y_min_ground = 1, y_max_ground = 101;

    hittable_list objects;

    for (int i = 0; i < 48; i++) {
        for (int j = 0; j < 27; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(y_min_ground, y_max_ground);
            auto z1 = z0 + w;
            auto ground = random_material();
            objects.add(make_shared<box>(point3(x0, y0, z0), point3(x1, y1, z1), ground));
        }
    }

    const int r_min = 40, r_max = 80;
    const int o_min = -20, o_max = 20;
    const int s_min = 80, s_max = 120;
    const int a_min = -45, a_max = 45;
    for (int i = 0; i < object_count; ++i) {
        auto obj = random_object_except_box(
            x_min, x_max, y_min_object, y_max_object, z_min, z_max,
            r_min, r_max, o_min, o_max, s_min, s_max, a_min, a_max
        );
        objects.add(obj);
    }

    const int i_min = 10, i_max = 15;
    const int s_min_light = 100, s_max_light = 150;
    for (int i = 0; i < light_count; ++i) {
        auto light = random_light(
            x_min, x_max, y_min_light, y_max_light, z_min, z_max,
            i_min, i_max, s_min_light, s_max_light
        );
        objects.add(light);
    }

    auto boundary = make_shared<sphere>(point3(360, 150, 145), 70, make_shared<dielectric>(1.5));
    objects.add(boundary);
    objects.add(make_shared<constant_medium>(boundary, 0.2, color(0.2, 0.4, 0.9)));
    boundary = make_shared<sphere>(point3(0, 0, 0), 5000, make_shared<dielectric>(1.5));
    objects.add(make_shared<constant_medium>(boundary, .0001, color(1, 1, 1)));

    auto node = make_shared<bvh_node>(objects, 0, 1);
    hittable_list root;
    root.add(node);
    return root;
}