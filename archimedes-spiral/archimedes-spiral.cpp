#include "pch.h"

#include "graphics.h"

using namespace std;
using namespace mathlib;

int main() {
    auto img = image{640, 480};

    const auto pi = atan(1.0f) * 4.0f;

    const auto center = Vec2f{img.width() / 2.0f, 6.0f + img.height() / 2.0f};
    auto startpoint = center;
    for (float t = 0.0f; t < 16.0f * pi; t+= 0.1f) {
        const auto r = 0.1f * t;
        const auto endpoint = Vec2f{r * cos(t), r * sin(t)} * 50.0f + center;
        draw_line(startpoint, endpoint, [&img](int x, int y, float val) {
            if (x >= 0 && x < img.width() && y >= 0 && y < img.height())
                img[{x, y}] = lerp(img[{x, y}], Vec3f::ones(), val);
        });
        startpoint = endpoint;
    }

    output_ppm(img);
}
