#ifndef _PIXEL_H
#define _PIXEL_H


struct Pixel {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    
    Pixel() : r(0), g(0), b(0) {}
    Pixel(unsigned char r, unsigned char g, unsigned char b) : r(r), g(g), b(b) {}
    
    // Overload operators for pixel calculations
    Pixel operator+(const Pixel& other) const {
        return Pixel(r + other.r, g + other.g, b + other.b);
    }
    
    Pixel operator-(const Pixel& other) const {
        return Pixel(r - other.r, g - other.g, b - other.b);
    }
    
    Pixel operator/(int divisor) const {
        return Pixel(r / divisor, g / divisor, b / divisor);
    }
    
    bool operator==(const Pixel& other) const {
        return r == other.r && g == other.g && b == other.b;
    }
    
    bool operator!=(const Pixel& other) const {
        return !(*this == other);
    }
};

#endif