// include header file
#include "ErrorCalculator.hpp"


// handler
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

// rgb channel
unsigned char getChannelValue(const Pixel& pixel, int channel) {
    switch (channel) {
        case 0: return pixel.r;
        case 1: return pixel.g;
        case 2: return pixel.b;
        default: return 0;
    }
}

// variance sum up
double VarianceErrorCalculator::calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    return (calculateVarianceForChannel(pixels, x, y, width, height, 0) +
            calculateVarianceForChannel(pixels, x, y, width, height, 1) +
            calculateVarianceForChannel(pixels, x, y, width, height, 2)) / 3.0;
}

// variance per channel
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

// MAD sum up
double MADErrorCalculator::calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    return (calculateMADForChannel(pixels, x, y, width, height, 0) +
            calculateMADForChannel(pixels, x, y, width, height, 1) +
            calculateMADForChannel(pixels, x, y, width, height, 2)) / 3.0;
}

// MAD per channel
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

// Diff sum up
double MaxPixelDifferenceCalculator::calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    return (calculateMaxDiffForChannel(pixels, x, y, width, height, 0) +
            calculateMaxDiffForChannel(pixels, x, y, width, height, 1) +
            calculateMaxDiffForChannel(pixels, x, y, width, height, 2)) / 3.0;
}

// Diff per channel
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

// Entropy sum up
double EntropyCalculator::calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    return (calculateEntropyForChannel(pixels, x, y, width, height, 0) +
            calculateEntropyForChannel(pixels, x, y, width, height, 1) +
            calculateEntropyForChannel(pixels, x, y, width, height, 2)) / 3.0;
}

// Entropy per channel
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

// SSIM sum up
double SSIMCalculator::calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) {
    // Ciptakan blok gambar terkompresi (dengan warna rata-rata)
    Pixel avgColor = calculateAverageColor(pixels, x, y, width, height);
    vector<vector<Pixel>> compressedBlock(height, vector<Pixel>(width, avgColor));
    
    // Hitung SSIM untuk setiap kanal warna
    double ssim_r = calculateSSIMForChannel(pixels, compressedBlock, x, y, width, height, 0);
    double ssim_g = calculateSSIMForChannel(pixels, compressedBlock, x, y, width, height, 1);
    double ssim_b = calculateSSIMForChannel(pixels, compressedBlock, x, y, width, height, 2);
    
    // Rata-rata SSIM untuk semua kanal (bobot seragam)
    double avg_ssim = (ssim_r + ssim_g + ssim_b) / 3.0;
    
    // Definisi "error" = 1 - SSIM, makin besar SSIM makin kecil error
    return 1.0 - avg_ssim;
}

// SSIM per channel
double SSIMCalculator::calculateSSIMForChannel(const vector<vector<Pixel>>& originalBlock, const vector<vector<Pixel>>& compressedBlock, int x, int y, int width, int height, int channel){
    const double L = 255.0;
    const double K1 = 0.01;
    const double K2 = 0.03;
    const double C1 = (K1 * L) * (K1 * L);
    const double C2 = (K2 * L) * (K2 * L);
    
    int N = width * height;
    if (N < 1) {
        return 1.0;
    }
    
    // mean for original and compressed blocks
    double sum_x = 0.0, sum_y = 0.0;
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            sum_x += getChannelValue(originalBlock[y+j][x+i], channel);
            sum_y += getChannelValue(compressedBlock[j][i], channel);
        }
    }
    
    double mu_x = sum_x / N;
    double mu_y = sum_y / N;
    
    // variance and covar
    double var_x = 0.0, var_y = 0.0, covar_xy = 0.0;
    
    for (int j = 0; j < height; j++) {
        for (int i = 0; i < width; i++) {
            double x_val = getChannelValue(originalBlock[y+j][x+i], channel);
            double y_val = getChannelValue(compressedBlock[j][i], channel);
            
            double diff_x = x_val - mu_x;
            double diff_y = y_val - mu_y;
            
            var_x += diff_x * diff_x;
            var_y += diff_y * diff_y;
            covar_xy += diff_x * diff_y;
        }
    }
    
    if (N > 1) {
        var_x /= (N - 1);
        var_y /= (N - 1);
        covar_xy /= (N - 1);
    } else {
        var_x = var_y = covar_xy = 0.0;
    }
    
    //    SSIM(x,y) = ((2μxμy + C1)(2σxy + C2)) / ((μx² + μy² + C1)(σx² + σy² + C2))
    double numerator = (2.0 * mu_x * mu_y + C1) * (2.0 * covar_xy + C2);
    double denominator = (mu_x * mu_x + mu_y * mu_y + C1) * (var_x + var_y + C2);
    
    if (denominator < 1e-10) {
        return 1.0;
    }
    
    double ssim = numerator / denominator;
    
    return ssim;
}

// compressed block
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
