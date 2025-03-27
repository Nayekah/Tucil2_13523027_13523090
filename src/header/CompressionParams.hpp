#ifndef _COMPRESSION_PARAMS_H
#define _COMPRESSION_PARAMS_H

#include <string>

using namespace std;


enum class ErrorMethod {
    VARIANCE = 1,
    MEAN_ABSOLUTE_DEVIATION = 2,
    MAX_PIXEL_DIFFERENCE = 3,
    ENTROPY = 4,
    STRUCTURAL_SIMILARITY = 5
};

struct CompressionParams {
    string inputImagePath;
    ErrorMethod errorMethod;
    double threshold;
    int minBlockSize;
    double targetCompressionPercentage;
    string outputImagePath;
    string gifOutputPath;
    bool generateGif;
    
    CompressionParams() : 
        errorMethod(ErrorMethod::VARIANCE),
        threshold(0.0),
        minBlockSize(1),
        targetCompressionPercentage(0.0),
        generateGif(false) {}
};

#endif