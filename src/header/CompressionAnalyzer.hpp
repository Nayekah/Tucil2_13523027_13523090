#ifndef _COMPRESSION_ANALYZER_H
#define _COMPRESSION_ANALYZER_H

#include <cstddef>
#include "QuadTree.hpp"

using namespace std;

class CompressionAnalyzer {
public:
    CompressionAnalyzer(size_t originalSize, size_t compressedSize, const QuadTree& quadTree);
    ~CompressionAnalyzer();
    
    // Calculate compression metrics
    double calculateCompressionPercentage() const;
    
    // Display results
    void displayResults(double executionTimeSeconds) const;
    
private:
    size_t originalImageSize;
    size_t compressedImageSize;
    const QuadTree& quadTree;
};

#endif