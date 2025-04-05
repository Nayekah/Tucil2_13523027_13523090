#ifndef _COMPRESSION_ANALYZER_HPP
#define _COMPRESSION_ANALYZER_HPP


// include lib files
#include <cstddef>
#include <iostream>
#include <iomanip>

// include header file
#include "QuadTree.hpp"


// namespace
using namespace std;

class CompressionAnalyzer {
    public:
        CompressionAnalyzer(size_t originalSize, size_t compressedSize, const QuadTree& quadTree); //Ctor
        ~CompressionAnalyzer(); //Dtor
        
        // Calculate compression metrics
        double calculateCompressionPercentage() const;
        
        // Display results
        void displayResults(double executionTimeSeconds) const;
        
    private:
        // Params
        size_t originalImageSize;
        size_t compressedImageSize;
        const QuadTree& quadTree;
    };

#endif