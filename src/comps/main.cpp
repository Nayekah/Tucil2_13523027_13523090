#include <iostream>
#include <chrono>
#include "QuadTree.hpp"
#include "InputManager.hpp"
#include "ImageProcessor.hpp"
#include "CompressionAnalyzer.hpp"
#include "GifGenerator.hpp"

int main() {
    InputManager inputManager;
    
    // Start the paged input process
    CompressionParams params = inputManager.getCompressionParams();
    
    // Process the image
    auto start = chrono::high_resolution_clock::now();
    
    ImageProcessor processor(params);
    processor.loadImage(params.inputImagePath);
    
    QuadTree quadTree = processor.compressImage();
    
    processor.saveCompressedImage(params.outputImagePath);
    
    if (params.generateGif && !params.gifOutputPath.empty()) {
        GifGenerator gifGen;
        gifGen.generateGif(quadTree, params.gifOutputPath);
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    
    // Display results
    CompressionAnalyzer analyzer(processor.getOriginalImageSize(), processor.getCompressedImageSize(), quadTree);
    analyzer.displayResults(elapsed.count());
    return 0;
}