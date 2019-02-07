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

using namespace std;

using xy = tuple<int, int>;
using rgb = tuple<float, float, float>;

auto to_r8g8b8_srgb(rgb x) {
  return apply(
      [](auto... y) {
        return tuple{static_cast<int>(pow(y, 1.0f / 2.2f) * 255.0f)...};
      },
      x);
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
  vector<rgb> pixels_;
};

void output_ppm(const image &img) {
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
  return tuple{frac, static_cast<int>(intpart)};
}

auto frac(float x) { return get<0>(modf(x)); }

auto operator-(tuple<float, float> x, tuple<float, float> y) {
  return tuple{get<0>(x) - get<0>(y), get<1>(x) - get<1>(y)};
}

template <typename F>
void draw_line(tuple<float, float> q0, tuple<float, float> q1, F plot) {
    const auto draw = [](auto p0, auto p1, auto pl) {
    if (get<0>(p0) > get<0>(p1))
      swap(p0, p1);

    const auto d = p1 - p0;
    const auto gradient = get<0>(d) == 0.0f ? 1.0f : get<1>(d) / get<0>(d);

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
  if (abs(get<1>(d)) > abs(get<0>(d))) {
    draw(tuple{get<1>(q0), get<0>(q0)}, tuple{get<1>(q1), get<0>(q1)},
         [plot](int x, int y, float val) { plot(y, x, val); });
  } else {
    draw(q0, q1, plot);
  }
}

int main() {
  auto img = image{640, 480};

  const auto [invWidth, invHeight] =
      tuple{1.0f / img.width(), 1.0f / img.height()};

  const auto pi = atan(1.0f) * 4.0f;

  for (int i = 0; i < 360; i += 15) {
    const auto angle = i * (pi / 180.0f);
    const auto radius = 200.0f;
    const auto [x, y] = tuple{cos(angle) * radius, sin(angle) * radius};
    draw_line({320.0f, 240.0f}, {320.0f + x, 240.0f + y},
              [&img](int x, int y, float val) {
                img[{x, y}] = {val, val, val};
              });
  }

  output_ppm(img);
}
