#ifndef _ERROR_CALCULATOR_HPP
#define _ERROR_CALCULATOR_HPP


// include lib files
#include <map>
#include <array>
#include <cmath>
#include <memory>
#include <vector>
#include <algorithm>


// include header files
#include "CompressionParams.hpp"
#include "Pixel.hpp"

// namespace
using namespace std;


class ErrorCalculator {
    public:
        virtual ~ErrorCalculator() = default; // Dtor
        virtual double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) = 0;
        
        // Factory method to create appropriate error calculator
        static unique_ptr<ErrorCalculator> create(ErrorMethod method);
        
    protected:
        // Helper method to validate region bounds
        bool isValidRegion(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) const;
};



// Variance
class VarianceErrorCalculator : public ErrorCalculator {
    public:
        // Method for calculating error using variance
        double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) override;
        
    private:
        // Helper method to calculate variance for each rgb channel
        double calculateVarianceForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel);
};



// Mean Absolute Deviation (MAD)
class MADErrorCalculator : public ErrorCalculator {
    public:
        // Method for calculating error using MAD
        double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) override;
        
    private:
        // Helper method to calculate MAD for each rgb channel
        double calculateMADForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel);
};



// Max Pixel Difference
class MaxPixelDifferenceCalculator : public ErrorCalculator {
    public:
        // Method for calculating error using max pixel difference
        double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) override;
        
    private:
        // Helper method to calculate max pixel difference for each rgb channel
        double calculateMaxDiffForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel);
};



// Entropy
class EntropyCalculator : public ErrorCalculator {
    public:
        // Method for calculating error using entropy
        double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) override;
        
    private:
        // Helper method to calculate entropy for each rgb channel
        double calculateEntropyForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel);
};



// Structural Similarity Index (SSIM) - Bonus
class SSIMCalculator : public ErrorCalculator {
    public:
        // Method for calculating error using SSIM
        double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) override;
        
    private:
        // Helper method to calculate SSIM for each rgb channel
        double calculateSSIMForChannel(
            const vector<vector<Pixel>>& originalBlock,
            const vector<vector<Pixel>>& compressedBlock,
            int x, int y, int width, int height, int channel);
            
        Pixel calculateAverageColor(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height);
    };

#endif
