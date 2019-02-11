// fractals-images-of-chaos.cpp : This file contains the 'main' function.
// Program execution begins and ends there.
//

#include "pch.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <tuple>
#include <vector>

#include <mathlib/vector.h>

#include "graphics.h"

using namespace std;
using namespace mathlib;


int main() {
    auto img = image{640, 480};

    const auto [invWidth, invHeight] = tuple{1.0f / img.width(), 1.0f / img.height()};

    const auto pi = atan(1.0f) * 4.0f;

    for (int i = 0; i < 360; i += 15) {
        const auto angle = i * (pi / 180.0f);
        const auto radius = 200.0f;
        const auto [x, y] = tuple{cos(angle) * radius, sin(angle) * radius};
        draw_line({320.0f, 240.0f}, {320.0f + x, 240.0f + y}, [&img](int x, int y, float val) {
            img[{x, y}] = lerp(img[{x, y}], Vec3f::ones(), val);
        });
    }

    output_ppm(img);
}
