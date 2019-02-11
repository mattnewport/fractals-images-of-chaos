#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <tuple>
#include <vector>

#include <mathlib/vector.h>

using xy = std::tuple<int, int>;
using rgb = mathlib::Vec3f;

auto to_r8g8b8_srgb(rgb x) {
    const auto srgb = map([](auto y) { return pow(y, 1.0f / 2.2f) * 255.0f; }, x);
    return std::tuple{static_cast<int>(srgb.x()), static_cast<int>(srgb.y()),
                 static_cast<int>(srgb.z())};
}

struct image {
    image(int w, int h) : w_{w}, pixels_{w * h} {}
    rgb operator[](xy xy) const {
        const auto [x, y] = xy;
        assert(x < width() && y < height());
        return pixels_[y * w_ + x];
    }
    rgb &operator[](xy xy) {
        const auto [x, y] = xy;
        assert(x < width() && y < height());
        return pixels_[y * w_ + x];
    }
    int width() const { return w_; }
    int height() const { return static_cast<int>(pixels_.size()) / w_; }

private:
    int w_;
    std::vector<rgb> pixels_;
};

void output_ppm(const image &img) {
    using namespace std;
    cout << "P3\n" << img.width() << " " << img.height() << "\n255\n";
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            const auto [r, g, b] = to_r8g8b8_srgb(img[{x, y}]);
            cout << r << " " << g << " " << b << " ";
        }
        cout << '\n';
    }
}

auto modf(float x) {
    auto intpart = 0.0f;
    const auto frac = modf(x, &intpart);
    return std::tuple{frac, static_cast<int>(intpart)};
}

auto frac(float x) { return std::get<0>(modf(x)); }

template <typename F>
void draw_line(mathlib::Vec2f q0, mathlib::Vec2f q1, F plot) {
    const auto draw = [](auto p0, auto p1, auto pl) {
        if (p0.x() > p1.x()) std::swap(p0, p1);

        const auto d = p1 - p0;
        const auto gradient = d.x() == 0.0f ? 1.0f : d.y() / d.x();

        const auto [x0, y0] = p0;
        const auto [x1, y1] = p1;

        const auto xend = round(x0);
        const auto yend = fma(gradient, xend - x0, y0);
        const auto xgap = 1.0f - frac(x0 + 0.5f);
        const auto xpxl1 = static_cast<int>(xend);
        const auto ypxl1 = static_cast<int>(floor(yend));
        pl(xpxl1, ypxl1, (1.0f - frac(yend)) * xgap);
        pl(xpxl1, ypxl1 + 1, frac(yend) * xgap);

        const auto xend2 = round(x1);
        const auto yend2 = fma(gradient, xend2 - x1, y1);
        const auto xgap2 = frac(x1 + 0.5f);
        const auto xpxl2 = static_cast<int>(xend2);
        const auto ypxl2 = static_cast<int>(floor(yend2));
        pl(xpxl2, ypxl2, (1.0f - frac(yend2)) * xgap2);
        pl(xpxl2, ypxl2 + 1, frac(yend2) * xgap2);

        for (int x = xpxl1 + 1, i = 1; x < xpxl2; ++x, ++i) {
            const auto intery = fmaf(gradient, static_cast<float>(i), yend);
            const auto [frac, intpart] = modf(intery);
            pl(x, intpart, 1.0f - frac);
            pl(x, intpart + 1, frac);
        }
    };

    const auto d = q1 - q0;
    if (abs(d.y()) > abs(d.x())) {
        draw(q0.yx(), q1.yx(), [plot](int x, int y, float val) { plot(y, x, val); });
    } else {
        draw(q0, q1, plot);
    }
}
