#include "pch.h"

#include "graphics.h"

using namespace std;
using namespace mathlib;

int main() {
    auto img = image{1280, 720};

    const auto pi = atan(1.0f) * 4.0f;

    const auto scale = 1200.0f;
    const auto center = Vec2f{40.0f, 40.0f};
    auto startpoint = Vec2f::zero();
    constexpr auto p = 6;
    const auto h = powf(3.0, -p);
    Vec2f dirs[6];
    generate(begin(dirs), end(dirs), [n = 0, pi]() mutable {
        const auto phi = n++ * pi / 3.0f;
        return Vec2f{cosf(phi), sinf(phi)};
    });
    const int n = 1 << (2 * p);
    for (int i = 0; i < n; ++i) {
        auto s = 0;
        for (auto m = i; m; m /= 4) s += (m % 4 + 1) % 3 - 1;
        const auto endpoint = startpoint + dirs[(6 + s % 6) % 6] * h;
        draw_line(startpoint * scale + center, endpoint * scale + center,
                  [&img](int x, int y, float val) {
                      if (x >= 0 && x < img.width() && y >= 0 && y < img.height())
                          img[{x, y}] = lerp(img[{x, y}], Vec3f::ones(), val);
                  });
        startpoint = endpoint;
    }

    output_ppm(img);
}
