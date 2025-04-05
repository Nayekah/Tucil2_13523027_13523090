#ifndef _IMAGE_PROCESSOR_HPP
#define _IMAGE_PROCESSOR_HPP

// include header files
#include "Pixel.hpp"
#include "QuadTree.hpp"
#include "ErrorCalculator.hpp"
#include "CompressionParams.hpp"

// include lib files
#include <cmath>
#include <mutex>
#include <future>
#include <limits>
#include <memory>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <algorithm>

// namespace
using namespace std;

// include OpenCV for image processing
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


class ImageProcessor {
    public:
        ImageProcessor(const CompressionParams& params); // Ctor
        ~ImageProcessor(); // Dtor
        
        // Image operations
        bool loadImage(const string& imagePath);
        QuadTree compressImage();
        bool saveCompressedImage(const string& outputPath);
        bool saveCompressedImageToBuffer(const string& extension, vector<unsigned char>& buffer);
        size_t calculateTheoricalCompressedSize(const QuadTree& tree) const;
        
        // Getters
        size_t getOriginalImageSize() const { return originalImageSize; }
        size_t getCompressedImageSize() const { return compressedImageSize; }
        int getImageWidth() const { return imageWidth; }
        int getImageHeight() const { return imageHeight; }
        
    private:
        CompressionParams params;
        int imageWidth;
        int imageHeight;
        vector<vector<Pixel>> pixels;
        unique_ptr<ErrorCalculator> errorCalculator;
        size_t originalImageSize;
        size_t compressedImageSize;
        QuadTree quadTree;
        
        // Helper methods
        void adjustMinimumBlockSize();
        size_t getFileSize(const string& filename) const;
        void initializeErrorCalculator();
        shared_ptr<QuadTreeNode> buildQuadTree(int x, int y, int width, int height, int depth);
        bool shouldSubdivide(int x, int y, int width, int height, double& error);
        Pixel calculateAverageColor(int x, int y, int width, int height);
        
        // Target compression methods (bonus)
        double findThresholdForTargetCompression(double targetPercentage);
        double compressWithThreshold(double threshold);
        
        // Validator
        bool isValidRegion(int x, int y, int width, int height) const;
};

#endif
