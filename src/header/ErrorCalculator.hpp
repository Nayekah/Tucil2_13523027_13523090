#ifndef _ERROR_CALCULATOR_HPP
#define _ERROR_CALCULATOR_HPP

#include <vector>
#include <memory>
#include "CompressionParams.hpp"
#include "Pixel.hpp"

using namespace std;

class ErrorCalculator {
public:
    virtual ~ErrorCalculator() = default;
    virtual double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) = 0;
    
    // Factory method to create appropriate error calculator
    static unique_ptr<ErrorCalculator> create(ErrorMethod method);
    
protected:
    // Helper method to validate region bounds
    bool isValidRegion(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) const;
};

class VarianceErrorCalculator : public ErrorCalculator {
public:
    double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) override;
    
private:
    double calculateVarianceForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel);
};

class MADErrorCalculator : public ErrorCalculator {
public:
    double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) override;
    
private:
    double calculateMADForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel);
};

class MaxPixelDifferenceCalculator : public ErrorCalculator {
public:
    double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) override;
    
private:
    double calculateMaxDiffForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel);
};

class EntropyCalculator : public ErrorCalculator {
public:
    double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) override;
    
private:
    double calculateEntropyForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel);
};

class SSIMCalculator : public ErrorCalculator {
public:
    double calculateError(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height) override;
    
private:
    double calculateSSIMForChannel(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height, int channel);
    Pixel calculateAverageColor(const vector<vector<Pixel>>& pixels, int x, int y, int width, int height);
};

#endif