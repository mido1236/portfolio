//
// Created by ahmed on 2025-03-03.
//

#ifndef PIXEL_H
#define PIXEL_H
struct Pixel {
    unsigned char r, g, b;

    Pixel() = default;

    Pixel(const unsigned char r, const unsigned char g, const unsigned char b): r(r), g(g), b(b) {}

    bool operator==(const Pixel &p) const {
        return r == p.r && g == p.g && b == p.b;
    }
};
#endif //PIXEL_H
