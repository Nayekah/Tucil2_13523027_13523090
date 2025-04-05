// include all necessary headers
#include <chrono>
#include "QuadTree.hpp"
#include "InputManager.hpp"
#include "BasicInputManager.hpp"
#include "ImageProcessor.hpp"
#include "CompressionAnalyzer.hpp"
#include "GifGenerator.hpp"

void printUsage() {
    cout << "Quadtree Image Compressor" << endl;
    cout << "=========================" << endl;
    cout << "Usage modes:" << endl;
    cout << "1. Basic Mode: Run with arg \"basic\"" << endl;
    cout << "   ./quadtree_compressor basic" << endl;
    cout << endl;
    cout << "2. Interactive Paging Mode: Run with arg \"page\"" << endl;
    cout << "   ./quadtree_compressor page" << endl;
    cout << endl;
}

int main(int argc, char* argv[]) {
    CompressionParams params;
    bool useBasicMode = false;
    
    if (argc > 1) {
        string arg1 = argv[1];
        if (arg1 == "-h" || arg1 == "--help" || arg1 == "help" || arg1 == "/?") {
            printUsage();
            return 0;
        } else if (arg1 == "basic") {  // basic mode
            useBasicMode = true;
        } else if (arg1 == "page") {   // paging mode
            useBasicMode = false;
        } else {
            cout << "Unknown argument: " << arg1 << endl;
            printUsage();
            return 1;
        }
    }
    
    // Get params
    if (useBasicMode) {
        cout << "=== Quadtree Image Compressor - Basic Mode ===" << endl;
        BasicInputManager basicInputManager;
        params = basicInputManager.getCompressionParams();
    } else {
        InputManager inputManager;
        params = inputManager.getCompressionParams();
    }
    
    // Process the image
    auto start = chrono::high_resolution_clock::now();
    
    ImageProcessor processor(params);
    if (!processor.loadImage(params.inputImagePath)) {
        cerr << "Failed to load image: " << params.inputImagePath << endl;
        return 1;
    }
    
    QuadTree quadTree = processor.compressImage();
    
    if (!processor.saveCompressedImage(params.outputImagePath)) {
        cerr << "Failed to save compressed image: " << params.outputImagePath << endl;
        return 1;
    }
    
    if (params.generateGif && !params.gifOutputPath.empty()) {
        GifGenerator gifGen;
        if (!gifGen.generateGif(quadTree, params.gifOutputPath)) {
            cerr << "Warning: Failed to generate GIF: " << params.gifOutputPath << endl;
        }
    }
    
    auto end = chrono::high_resolution_clock::now();
    chrono::duration<double> elapsed = end - start;
    
    // Display results
    CompressionAnalyzer analyzer(processor.getOriginalImageSize(), processor.getCompressedImageSize(), quadTree);
    analyzer.displayResults(elapsed.count());
    
    return 0;
}