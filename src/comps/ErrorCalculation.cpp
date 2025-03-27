#include "ErrorCalculator.hpp"
#include <cmath>
#include <algorithm>
#include <array>
#include <map>


unique_ptr<ErrorCalculator> ErrorCalculator::create(ErrorMethod method) {
    switch (method) {
        case ErrorMethod::VARIANCE:
            return make_unique<VarianceErrorCalculator>();
        case ErrorMethod::MEAN_ABSOLUTE_DEVIATION:
            return make_unique<MADErrorCalculator>();
        case ErrorMethod::MAX_PIXEL_DIFFERENCE:
            return make_unique<MaxPixelDifferenceCalculator>();
        case ErrorMethod::ENTROPY:
            return make_unique<EntropyCalculator>();
        case ErrorMethod::STRUCTURAL_SIMILARITY:
            return make_unique<SSIMCalculator>();
        default:
            return make_unique<VarianceErrorCalculator>();
    }
}

unsigned char getChannelValue(const Pixel& pixel, int channel) {
    switch (channel) {
        case 0: return pixel.r;
        case 1: return pixel.g;
        case 2: return pixel.b;
        default: return 0;
    }
}

double VarianceErrorCalculator::calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    return (calculateVarianceForChannel(pixels, x, y, width, height, 0) +
            calculateVarianceForChannel(pixels, x, y, width, height, 1) +
            calculateVarianceForChannel(pixels, x, y, width, height, 2)) / 3.0;
}

double VarianceErrorCalculator::calculateVarianceForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel) {
    double sum = 0;
    for (int j = y; j < y + height; ++j)
        for (int i = x; i < x + width; ++i)
            sum += getChannelValue(pixels[j][i], channel);

    double mean = sum / (width * height);
    double variance = 0;
    for (int j = y; j < y + height; ++j)
        for (int i = x; i < x + width; ++i) {
            double diff = getChannelValue(pixels[j][i], channel) - mean;
            variance += diff * diff;
        }

    return variance / (width * height);
}

double MADErrorCalculator::calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    return (calculateMADForChannel(pixels, x, y, width, height, 0) +
            calculateMADForChannel(pixels, x, y, width, height, 1) +
            calculateMADForChannel(pixels, x, y, width, height, 2)) / 3.0;
}

double MADErrorCalculator::calculateMADForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel) {
    double sum = 0;
    for (int j = y; j < y + height; ++j)
        for (int i = x; i < x + width; ++i)
            sum += getChannelValue(pixels[j][i], channel);

    double mean = sum / (width * height);
    double mad = 0;
    for (int j = y; j < y + height; ++j)
        for (int i = x; i < x + width; ++i)
            mad += abs(getChannelValue(pixels[j][i], channel) - mean);

    return mad / (width * height);
}

double MaxPixelDifferenceCalculator::calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    return (calculateMaxDiffForChannel(pixels, x, y, width, height, 0) +
            calculateMaxDiffForChannel(pixels, x, y, width, height, 1) +
            calculateMaxDiffForChannel(pixels, x, y, width, height, 2)) / 3.0;
}

double MaxPixelDifferenceCalculator::calculateMaxDiffForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel) {
    unsigned char minVal = 255, maxVal = 0;
    for (int j = y; j < y + height; ++j)
        for (int i = x; i < x + width; ++i) {
            auto val = getChannelValue(pixels[j][i], channel);
            minVal = min(minVal, val);
            maxVal = max(maxVal, val);
        }
    return static_cast<double>(maxVal - minVal);
}

double EntropyCalculator::calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    return (calculateEntropyForChannel(pixels, x, y, width, height, 0) +
            calculateEntropyForChannel(pixels, x, y, width, height, 1) +
            calculateEntropyForChannel(pixels, x, y, width, height, 2)) / 3.0;
}

double EntropyCalculator::calculateEntropyForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel) {
    array<int,256> histogram{};
    for (int j = y; j < y + height; ++j)
        for (int i = x; i < x + width; ++i)
            histogram[getChannelValue(pixels[j][i], channel)]++;

    double entropy = 0, total = width * height;
    for (auto count : histogram)
        if (count)
            entropy -= (count/total) * log2(count/total);

    return entropy;
}

double SSIMCalculator::calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    return 1.0 - ((calculateSSIMForChannel(pixels, x, y, width, height, 0) +
                   calculateSSIMForChannel(pixels, x, y, width, height, 1) +
                   calculateSSIMForChannel(pixels, x, y, width, height, 2)) / 3.0);
}

double SSIMCalculator::calculateSSIMForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel) {
    // Constants for SSIM calculation (commonly used values)
    const double C1 = (0.01 * 255) * (0.01 * 255);
    const double C2 = (0.03 * 255) * (0.03 * 255);
    
    // Calculate the average color of the block
    Pixel avgColor = calculateAverageColor(pixels, x, y, width, height);
    
    // Treat this as the reference image (x)
    double mu_x = getChannelValue(avgColor, channel);
    
    // Original pixel values as the distorted image (y)
    // Calculate mean of original pixel values
    double mu_y = 0.0;
    for (int j = y; j < y + height; ++j) {
        for (int i = x; i < x + width; ++i) {
            mu_y += getChannelValue(pixels[j][i], channel);
        }
    }
    mu_y /= (width * height);
    
    // Calculate variance and covariance
    double sigma_x_squared = 0.0;
    double sigma_y_squared = 0.0;
    double sigma_xy = 0.0;
    
    for (int j = y; j < y + height; ++j) {
        for (int i = x; i < x + width; ++i) {
            double y_val = getChannelValue(pixels[j][i], channel);
            
            // Variance for x (every pixel is the same - the average)
            sigma_x_squared += (mu_x - mu_x) * (mu_x - mu_x);
            
            // Variance for y (actual pixel distribution)
            sigma_y_squared += (y_val - mu_y) * (y_val - mu_y);
            
            // Covariance
            sigma_xy += (mu_x - mu_x) * (y_val - mu_y);
        }
    }
    
    sigma_x_squared /= (width * height - 1);
    sigma_y_squared /= (width * height - 1);
    sigma_xy /= (width * height - 1);
    
    // Since every pixel in x is the same (the average), sigma_x_squared is 0
    // We set it to a small value to avoid division by zero
    sigma_x_squared = 0.000001;
    
    // Calculate SSIM
    double ssim = ((2 * mu_x * mu_y + C1) * (2 * sigma_xy + C2)) / 
                 ((mu_x * mu_x + mu_y * mu_y + C1) * (sigma_x_squared + sigma_y_squared + C2));
    
    return ssim;
}

Pixel SSIMCalculator::calculateAverageColor(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    int totalR = 0, totalG = 0, totalB = 0;
    int count = width * height;
    
    for (int j = y; j < y + height; ++j) {
        for (int i = x; i < x + width; ++i) {
            totalR += pixels[j][i].r;
            totalG += pixels[j][i].g;
            totalB += pixels[j][i].b;
        }
    }
    
    return Pixel(
        static_cast<unsigned char>(totalR / count),
        static_cast<unsigned char>(totalG / count),
        static_cast<unsigned char>(totalB / count)
    );
}