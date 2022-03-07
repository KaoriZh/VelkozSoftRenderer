#include"utility.h"
#include"camera.h"
#include"color.h"
#include"hittable_list.h"
#include"sphere.h"
#include"moving_sphere.h"
#include"material.h"
#include"aarect.h"
#include"scene.h"
#include"pdf.h"
#include<iostream>
#include<vector>
#include<iomanip>
#include<cmath>
#include<cstdlib>
#include<omp.h>

color ray_color(const ray& r, const color& background, const hittable& world, shared_ptr<hittable_list>& objs_sample, int depth) {
    hit_record rec;

    if (depth <= 0)
        return color(0, 0, 0);
    if (!world.hit(r, 0.001, infinity, rec))
        return background;

    scatter_record srec;
    color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);
    if (!rec.mat_ptr->scatter(r, rec, srec))
        return emitted;

    // if fuzziness is high
    // this surface isn’t ideally specular, 
    // but the implicit sampling works just like it did before.
    // ray_color() just needs a new case to 
    // generate an implicitly sampled ray
    if (srec.is_specular)
        return srec.attenuation * ray_color(srec.specular_ray, background, world, objs_sample, depth - 1);

    auto light_ptr = make_shared<hittable_pdf>(objs_sample, rec.p);
    mixture_pdf mixed_pdf(light_ptr, srec.pdf_ptr);

    ray scattered = ray(rec.p, mixed_pdf.generate(), r.time);
    auto pdf_val = mixed_pdf.get_probability(scattered.dir);

    // rec.mat_ptr->scattering_pdf : sampling a ray using hit point's BSDF
    return emitted + srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered) * ray_color(scattered, background, world, objs_sample, depth - 1) / pdf_val;
}

int main() {
    timer_start("Scene initialization");
    // Image
    double aspect_ratio = 16.0 / 9.0;
    int image_width = 400;
    int samples_per_pixel = 100;
    int max_depth = 50;

    // World
    hittable_list world;
    color background(0, 0, 0);

    // Camera
    point3 lookfrom(13, 2, 3);
    point3 lookat(0, 0, 0);
    vec3 up(0, 1, 0);
    auto vfov = 0.0;
    auto dist_to_focus = 10.0;
    auto aperture = 0.0;

#ifdef DEBUG
    
    auto objs_sample = make_shared<hittable_list>();
    objs_sample->add(make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<diffuse_light>()));
    objs_sample->add(make_shared<sphere>(point3(190, 90, 190), 90, shared_ptr<diffuse_light>()));
    shared_ptr<hittable> lights = make_shared<xz_rect>(213, 343, 227, 332, 554, shared_ptr<diffuse_light>());

    world = cornell_box();
    aspect_ratio = 1.0;
    image_width = 600;
    samples_per_pixel = 1000;
    lookfrom = point3(278, 278, -800);
    lookat = point3(278, 278, 0);
    vfov = 40.0;
#else
    switch (0) {
    case 1:
        background = color(0.70, 0.80, 1.00);
        world = random_simple_scene();
        vfov = 20.0;
        aperture = 0.1;
        break;
    case 2:
        background = color(0.70, 0.80, 1.00);
        world = two_sphere();
        vfov = 20.0;
        aperture = 0.0;
        break;
    case 3:
        background = color(0.70, 0.80, 1.00);
        world = two_perlin_sphere();
        vfov = 20.0;
        aperture = 0.0;
        break;
    case 4:
        world = earth();
        vfov = 20.0;
        break;
    case 5:
        world = simple_light();
        samples_per_pixel = 400;
        lookfrom = point3(26, 3, 6);
        lookat = point3(0, 2, 0);
        vfov = 20.0;
        break;
    case 6:
        world = cornell_box();
        aspect_ratio = 1.0;
        image_width = 600;
        samples_per_pixel = 200;
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;
    case 7:
        world = cornell_smoke();
        aspect_ratio = 1.0;
        image_width = 600;
        samples_per_pixel = 200;
        lookfrom = point3(278, 278, -800);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;
    case 8:
    default:
        world = final_scene();
        aspect_ratio = 1.0;
        image_width = 800;
        samples_per_pixel = 100;
        lookfrom = point3(478, 278, -600);
        lookat = point3(278, 278, 0);
        vfov = 40.0;
        break;
    }
#endif // DEBUG

    int image_height = static_cast<int>(image_width / aspect_ratio);
    camera cam(lookfrom, lookat, up, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

    timer_end();

    // Render
#ifdef DEBUG
#ifdef DEBUG_IMAGE_SUB_AREA
    const int debug_area_x_min = 400;
    const int debug_area_x_max = 637;
    const int debug_area_y_min = 400;
    const int debug_area_y_max = 601;
    const color debug_color = color(0.0, 0.5, 1.0);
#else
    const int debug_area_x_min = 0;
    const int debug_area_x_max = image_width;
    const int debug_area_y_min = 0;
    const int debug_area_y_max = image_height;
    const color debug_color = color(0.0, 0.0, 0.0);
#endif // DEBUG_IMAGE_SUB_AREA
#endif // DEBUG

    timer_start("Ray tracing");
    std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

#ifdef OMP
    auto num_threads = omp_get_num_procs();
    omp_set_num_threads(num_threads);
    std::cerr << "Threads amount: " << num_threads << "\n";
    std::vector<std::vector<color>> image(image_width, std::vector<color>(image_height, color(0, 0, 0)));
#endif // OMP

    int j = 0, i = 0, s = 0;
#ifdef OMP
#pragma omp parallel for private(i, j, s)
#endif // OMP
    for (j = image_height - 1; j >= 0; --j) {
        for (i = 0; i < image_width; ++i) {
#ifdef DEBUG
            if (debug_area_x_min <= i && i <= debug_area_x_max && debug_area_y_min <= j && j <= debug_area_y_max) {
                color c(0, 0, 0);
                for (s = 0; s < samples_per_pixel; ++s) {
                    auto u = (i + random_double()) / (image_width - 1);
                    auto v = (j + random_double()) / (image_height - 1);
                    ray r = cam.generate_ray(u, v);
                    c += ray_color(r, background, world, objs_sample, max_depth);
                }
#ifdef OMP
                image[i][j] += c;
#else
                write_color(std::cout, c, samples_per_pixel);
#endif // OMP
            } else {
#ifdef OMP
                image[i][j] += debug_color * samples_per_pixel;
#else
                write_color(std::cout, debug_color * samples_per_pixel, samples_per_pixel);
#endif // OMP
            }
#ifdef OMP_PIXEL
            timer_tick();
            std::cerr << " in thread: " << omp_get_thread_num() <<
                " at height: " << j << ", width: " << i << ".\n";
#endif // OMP

#else
            color c(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.generate_ray(u, v);
                c += ray_color(r, background, world, max_depth);
            }
            write_color(std::cout, c, samples_per_pixel);
#endif // DEBUG
        }
#ifdef OMP
        timer_tick();
        std::cerr<< " in thread: " << omp_get_thread_num() <<
            " at height: " << j << ".\n";
#else
        //timer_tick();
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
#endif // OMP
    }

#ifdef OMP
    for (int j = image_height - 1; j >= 0; --j) {
        for (int i = 0; i < image_width; ++i) {
            write_color(std::cout, image[i][j], samples_per_pixel);
        }
    }
#endif // OMP

    std::cerr << "\nDone.\n";
    timer_end();
    return 0;
}