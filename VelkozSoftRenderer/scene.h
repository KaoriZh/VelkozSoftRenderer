#pragma once
#include"hittable_list.h"
#include"material.h"
#include"sphere.h"
#include"moving_sphere.h"
#include"aarect.h"
#include"box.h"
//#include"constant_medium.h"
#include"bvh.h"
//#include"random_scene.h"

//hittable_list cornell_smoke() {
//    hittable_list objects;
//
//    auto red = make_shared<lambertian>(color(.65, .05, .05));
//    auto white = make_shared<lambertian>(color(.73, .73, .73));
//    auto green = make_shared<lambertian>(color(.12, .45, .15));
//    auto light = make_shared<diffuse_light>(color(7, 7, 7));
//
//    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
//    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
//    objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
//    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
//    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
//    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));
//
//    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(100, 200, 100), white);
//    box1 = make_shared<rotate_y>(box1, -18);
//    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
//    objects.add(box1);
//
//    shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(100, 100, 100), white);
//    box2 = make_shared<rotate_y>(box2, 15);
//    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
//    objects.add(box2);
//
//    box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), red);
//    box1 = make_shared<rotate_y>(box1, 15);
//    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
//    box1 = make_shared<constant_medium>(box1, 0.01, color(0, 0, 0));
//    objects.add(box1);
//
//    box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), green);
//    box2 = make_shared<rotate_y>(box2, -18);
//    box2 = make_shared<translate>(box2, vec3(130, 0, 65));
//    box2 = make_shared<constant_medium>(box2, 0.01, color(1, 1, 1));
//    objects.add(box2);
//
//    return objects;
//}

hittable_list cornell_box() {
    hittable_list objects;

    auto red = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

    shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
    box1 = make_shared<rotate_y>(box1, 15);
    box1 = make_shared<translate>(box1, vec3(265, 0, 295));
    objects.add(box1);

    //shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
    //box2 = make_shared<rotate_y>(box2, -18);
    //box2 = make_shared<translate>(box2, vec3(130, 0, 65));
    //objects.add(box2);

    auto glass = make_shared<dielectric>(1.5);
    objects.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));

    return objects;
}