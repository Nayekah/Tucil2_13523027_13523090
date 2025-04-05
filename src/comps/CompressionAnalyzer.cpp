// Include header file
#include "CompressionAnalyzer.hpp"


CompressionAnalyzer::CompressionAnalyzer(size_t originalSize, size_t compressedSize, const QuadTree& quadTree) : originalImageSize(originalSize), compressedImageSize(compressedSize), quadTree(quadTree) {
    // cons
}

CompressionAnalyzer::~CompressionAnalyzer() {
    // dtor
}

// percentage calc
double CompressionAnalyzer::calculateCompressionPercentage() const {
    if (originalImageSize == 0) {
        return 0.0;
    }
    return (1.0 - static_cast<double>(compressedImageSize) / originalImageSize) * 100.0;
}

// handler
void CompressionAnalyzer::displayResults(double executionTimeSeconds) const {
    cout << "\n========================================" << endl;
    cout << "       Compression Results" << endl;
    cout << "========================================" << endl;

    cout << "Execution Time: " << fixed << setprecision(3)
         << executionTimeSeconds << " seconds" << endl;

    cout << "Original Image Size: " << originalImageSize << " bytes";
    if (originalImageSize >= 1024) {
        cout << " (" << fixed << setprecision(2)
             << (originalImageSize / 1024.0) << " KB)";
    }
    if (originalImageSize >= 1024 * 1024) {
        cout << " (" << fixed << setprecision(2)
             << (originalImageSize / (1024.0 * 1024.0)) << " MB)";
    }
    cout << endl;

    cout << "Compressed Image Size: " << compressedImageSize << " bytes";
    if (compressedImageSize >= 1024) {
        cout << " (" << fixed << setprecision(2)
             << (compressedImageSize / 1024.0) << " KB)";
    }
    if (compressedImageSize >= 1024 * 1024) {
        cout << " (" << fixed << setprecision(2)
             << (compressedImageSize / (1024.0 * 1024.0)) << " MB)";
    }
    cout << endl;

    cout << "Compression Percentage: " << fixed << setprecision(2)
         << calculateCompressionPercentage() << "%" << endl;

    cout << "QuadTree Depth: " << quadTree.getDepth() << endl;
    cout << "QuadTree Node Count: " << quadTree.getNodeCount() << endl;

    cout << "========================================" << endl;
}
