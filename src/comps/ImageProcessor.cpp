#include "ImageProcessor.hpp"
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>
#include <fstream>
#include <iomanip>


ImageProcessor::ImageProcessor(const CompressionParams& params)
    : params(params), 
      imageWidth(0), 
      imageHeight(0), 
      originalImageSize(0), 
      compressedImageSize(0) {
    initializeErrorCalculator();
}

ImageProcessor::~ImageProcessor() {
    // Smart pointers handle cleanup automatically
}

size_t ImageProcessor::getFileSize(const string& filename) const {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) return 0;
    return file.tellg();
}

void ImageProcessor::initializeErrorCalculator() {
    errorCalculator = ErrorCalculator::create(params.errorMethod);
    if (!errorCalculator) {
        cerr << "Failed to create error calculator. Using default Variance method." << endl;
        errorCalculator = make_unique<VarianceErrorCalculator>();
    }
}

void ImageProcessor::adjustMinimumBlockSize() {
    if (imageWidth <= 0 || imageHeight <= 0) {
        return; // No valid image dimensions yet
    }
    
    // Calculate the maximum possible area for a block (the entire image)
    int maxPossibleArea = imageWidth * imageHeight;
    
    // Calculate a reasonable maximum minimum block size (1/4 of the image area)
    int recommendedMaxMinBlockSize = maxPossibleArea / 4;
    
    // If the user-specified minimum block size is too large, cap it
    if (params.minBlockSize > maxPossibleArea) {
        cout << "Warning: Minimum block size (" << params.minBlockSize 
             << " square pixels) is larger than the image area (" 
             << maxPossibleArea << " square pixels)." << endl;
        cout << "Adjusting to the image area." << endl;
        params.minBlockSize = maxPossibleArea;
    }
    else if (params.minBlockSize > recommendedMaxMinBlockSize) {
        cout << "Warning: Minimum block size (" << params.minBlockSize 
             << " square pixels) is very large relative to the image size." << endl;
        cout << "This may result in poor compression performance." << endl;
    }
}

bool ImageProcessor::loadImage(const string& imagePath) {
    try {
        cout << "Loading image: " << imagePath << endl;
        
        // Load image using OpenCV
        cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
        
        if (image.empty()) {
            cerr << "Failed to load image: " << imagePath << endl;
            return false;
        }
        
        // Get image dimensions
        imageWidth = image.cols;
        imageHeight = image.rows;
        
        cout << "Image loaded: " << imageWidth << "x" << imageHeight << " pixels" << endl;
        
        // Check for valid dimensions
        if (imageWidth <= 0 || imageHeight <= 0) {
            cerr << "Invalid image dimensions" << endl;
            return false;
        }
        
        // Calculate original file size
        originalImageSize = getFileSize(imagePath);
        
        // Convert to our pixel format
        pixels.clear();
        pixels.resize(imageHeight, vector<Pixel>(imageWidth));
        
        for (int y = 0; y < imageHeight; ++y) {
            for (int x = 0; x < imageWidth; ++x) {
                cv::Vec3b color = image.at<cv::Vec3b>(y, x);
                pixels[y][x] = Pixel(color[2], color[1], color[0]); // BGR to RGB
            }
        }
        
        cout << "Image converted to internal format" << endl;

        // Adjust minimum block size based on image dimensions
        adjustMinimumBlockSize();
        return true;
    } catch (const exception& e) {
        cerr << "Exception loading image: " << e.what() << endl;
        return false;
    } catch (...) {
        cerr << "Unknown exception loading image" << endl;
        return false;
    }
}

QuadTree ImageProcessor::compressImage() {
    cout << "Starting image compression..." << endl;
    QuadTree tree;
    
    try {
        // Check if we have valid image data
        if (imageWidth <= 0 || imageHeight <= 0 || pixels.empty()) {
            cerr << "No valid image data to compress" << endl;
            return tree;
        }
        
        // If target compression is specified, find the appropriate threshold
        if (params.targetCompressionPercentage > 0.0) {
            cout << "Using target compression: " << (params.targetCompressionPercentage * 100) << "%" << endl;
            params.threshold = findThresholdForTargetCompression(params.targetCompressionPercentage);
            cout << "Adjusted threshold: " << params.threshold << endl;
        }
        
        // Build the quad tree recursively
        cout << "Building quadtree..." << endl;
        shared_ptr<QuadTreeNode> root = buildQuadTree(0, 0, imageWidth, imageHeight, 0);
        
        if (!root) {
            cerr << "Failed to build quadtree root" << endl;
            return tree;
        }
        
        // Set the root and calculate tree properties
        tree.setRoot(root);
        tree.calculateDepthAndNodeCount();
        
        cout << "QuadTree built successfully: " 
                  << "depth=" << tree.getDepth() 
                  << ", nodes=" << tree.getNodeCount() << endl;
        
        // Calculate the compressed size (estimate)
        compressedImageSize = calculateTheoricalCompressedSize(tree);
        
        quadTree = tree; // Save for later use
        
        return tree;
    } catch (const exception& e) {
        cerr << "Exception during compression: " << e.what() << endl;
        return tree;
    } catch (...) {
        cerr << "Unknown exception during compression" << endl;
        return tree;
    }
}

bool ImageProcessor::isValidRegion(int x, int y, int width, int height) const {
    return x >= 0 && y >= 0 && 
           width > 0 && height > 0 && 
           x + width <= imageWidth && 
           y + height <= imageHeight;
}

shared_ptr<QuadTreeNode> ImageProcessor::buildQuadTree(int x, int y, int width, int height, int depth) {
    // Create a new node for this region
    auto node = make_shared<QuadTreeNode>(x, y, width, height);
    
    // Set the average color for this node (always do this regardless of subdivision)
    node->setColor(calculateAverageColor(x, y, width, height));
    
    // Check if we should subdivide
    double error = 0.0;
    bool shouldDivide = shouldSubdivide(x, y, width, height, error);
    
    // Calculate the area of each sub-block if we were to divide
    int halfWidth = width / 2;
    int remainderWidth = width - halfWidth;
    int halfHeight = height / 2;
    int remainderHeight = height - halfHeight;
    int subBlockArea = halfWidth * halfHeight;
    
    // Only subdivide if the error is above threshold AND the sub-blocks' area won't be too small
    if (shouldDivide && subBlockArea >= params.minBlockSize) {
        auto topLeft = buildQuadTree(x, y, halfWidth, halfHeight, depth + 1);
        auto topRight = buildQuadTree(x + halfWidth, y, remainderWidth, halfHeight, depth + 1);
        auto bottomLeft = buildQuadTree(x, y + halfHeight, halfWidth, remainderHeight, depth + 1);
        auto bottomRight = buildQuadTree(x + halfWidth, y + halfHeight, remainderWidth, remainderHeight, depth + 1);
        
        // Only add valid children
        if (topLeft) node->addChild(topLeft);
        if (topRight) node->addChild(topRight);
        if (bottomLeft) node->addChild(bottomLeft);
        if (bottomRight) node->addChild(bottomRight);
    }
    
    return node;
}

bool ImageProcessor::shouldSubdivide(int x, int y, int width, int height, double& error) {
    // Safety check
    if (!errorCalculator) {
        cerr << "Error calculator is null" << endl;
        return false;
    }
    
    if (!isValidRegion(x, y, width, height)) {
        return false;
    }
    
    try {
        // Calculate error using the selected method
        error = errorCalculator->calculateError(pixels, x, y, width, height);
        
        // Check if error is above threshold
        return error > params.threshold;
    } catch (const exception& e) {
        cerr << "Exception in shouldSubdivide: " << e.what() << endl;
        return false;
    } catch (...) {
        cerr << "Unknown exception in shouldSubdivide" << endl;
        return false;
    }
}

Pixel ImageProcessor::calculateAverageColor(int x, int y, int width, int height) {
    // Safety check
    if (!isValidRegion(x, y, width, height)) {
        return Pixel(0, 0, 0); // Return black for invalid regions
    }
    
    long totalR = 0, totalG = 0, totalB = 0;
    int count = width * height;
    
    if (count <= 0) {
        return Pixel(0, 0, 0);
    }
    
    try {
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
    } catch (const exception& e) {
        cerr << "Exception in calculateAverageColor: " << e.what() << endl;
        return Pixel(0, 0, 0);
    }
}

double ImageProcessor::findThresholdForTargetCompression(double targetPercentage) {
    cout << "Finding threshold for target compression: " << (targetPercentage * 100) << "%" << endl;
    
    // Save original threshold
    double origThreshold = params.threshold;
    
    // Define threshold search range based on error method
    double lowT = 10.0;
    double highT = 16256.0;
    
    switch (params.errorMethod) {
        case ErrorMethod::VARIANCE:
            lowT = 10.0;
            highT = 16256.0;
            break;
        case ErrorMethod::MEAN_ABSOLUTE_DEVIATION:
            lowT = 1.0;
            highT = 127.5;
            break;
        case ErrorMethod::MAX_PIXEL_DIFFERENCE:
            lowT = 5.0;
            highT = 255.0;
            break;
        case ErrorMethod::ENTROPY:
            lowT = 0.1;
            highT = 8.0;
            break;
        case ErrorMethod::STRUCTURAL_SIMILARITY:
            lowT = 0.001;
            highT = 1.0;
            break;
    }
    
    // Test some sample thresholds to better understand the relationship
    vector<pair<double, int>> samples; // (threshold, node count)
    
    // Use logarithmic spacing for better coverage of the range
    vector<double> sampleThresholds = {lowT, highT};
    int numSamples = 5;
    
    for (int i = 1; i < numSamples; i++) {
        double factor = static_cast<double>(i) / numSamples;
        double t = exp(log(lowT) + factor * (log(highT) - log(lowT)));
        sampleThresholds.push_back(t);
    }
    
    // Sort thresholds
    sort(sampleThresholds.begin(), sampleThresholds.end());
    
    cout << "Testing sample thresholds..." << endl;
    for (double t : sampleThresholds) {
        params.threshold = t;
        QuadTree testTree;
        auto testRoot = buildQuadTree(0, 0, imageWidth, imageHeight, 0);
        testTree.setRoot(testRoot);
        testTree.calculateDepthAndNodeCount();
        
        int nodeCount = testTree.getNodeCount();
        samples.push_back(make_pair(t, nodeCount));
        
        cout << "  Threshold: " << t << ", Nodes: " << nodeCount << endl;
    }
    
    // Create a mathematical model relating threshold to node count
    // We've previously seen that threshold 16256 with ~1 node gave 80% compression
    // And threshold 2908 with ~700 nodes gave ~50% compression
    
    // Based on these data points, we can create a function to map node count to compression
    // We'll use a simple logarithmic model: Compression = a - b * log(NodeCount)
    // Using the data points (1, 0.80) and (700, 0.50), we get a ≈ 0.80, b ≈ 0.05
    
    // Let's map our target compression to an estimated node count
    double targetNodeCount;
    
    if (targetPercentage >= 0.80) {
        // For very high compression, use just 1 node
        targetNodeCount = 1;
    } else {
        // For compression below 80%, use our model
        // Solve for NodeCount: NodeCount = exp((a - Compression) / b)
        double a = 0.80;
        double b = 0.05;
        targetNodeCount = exp((a - targetPercentage) / b);
    }
    
    cout << "Target node count for " << (targetPercentage * 100) 
         << "% compression: ~" << targetNodeCount << endl;
    
    // Now we need to find the threshold that will give us approximately this node count
    // Use bisection method to converge on a threshold that gives the target node count
    
    // First, check if any of our samples already meet the criteria
    double bestT = sampleThresholds[0];
    double bestDiff = abs(samples[0].second - targetNodeCount);
    
    for (const auto& sample : samples) {
        double diff = abs(sample.second - targetNodeCount);
        if (diff < bestDiff) {
            bestDiff = diff;
            bestT = sample.first;
        }
    }
    
    // If we're already very close, just use that threshold
    if (bestDiff / targetNodeCount < 0.05) {
        cout << "Sample threshold " << bestT << " already gives close to target node count." << endl;
        params.threshold = origThreshold;
        return bestT;
    }
    
    // Otherwise, refine with bisection method
    double midT;
    int nodeCount;
    
    const int MAX_ITERATIONS = 10;
    
    cout << "Using bisection method to find optimal threshold..." << endl;
    
    for (int i = 0; i < MAX_ITERATIONS; i++) {
        // Calculate midpoint on logarithmic scale
        midT = exp((log(lowT) + log(highT)) / 2.0);
        
        // Build tree with this threshold
        params.threshold = midT;
        QuadTree testTree;
        auto testRoot = buildQuadTree(0, 0, imageWidth, imageHeight, 0);
        testTree.setRoot(testRoot);
        testTree.calculateDepthAndNodeCount();
        
        nodeCount = testTree.getNodeCount();
        double diff = abs(nodeCount - targetNodeCount);
        double relDiff = diff / targetNodeCount;
        
        cout << "Iteration " << (i+1) << ": Threshold=" << midT 
             << ", Nodes=" << nodeCount 
             << ", Target=" << targetNodeCount 
             << ", Diff=" << (relDiff * 100) << "%" << endl;
        
        // Update best result if this is closer
        if (diff < bestDiff) {
            bestDiff = diff;
            bestT = midT;
            
            // Exit if we're close enough
            if (relDiff < 0.05) {
                cout << "Found threshold within 5% of target node count." << endl;
                break;
            }
        }
        
        // Adjust search interval
        if (nodeCount > targetNodeCount) {
            // Too many nodes, increase threshold to reduce nodes
            lowT = midT;
        } else {
            // Too few nodes, decrease threshold to increase nodes
            highT = midT;
        }
        
        // Check if we've converged
        if ((highT - lowT) / highT < 0.01) {
            cout << "Search range converged." << endl;
            break;
        }
    }
    
    // Apply a correction factor based on empirical observations
    // For 45% target that resulted in 56% actual, we can use 45/56 ≈ 0.8
    if (params.errorMethod == ErrorMethod::VARIANCE) {
        double correctionFactor = 0.8;
        double correctedThreshold = bestT * correctionFactor;
        
        cout << "Applying empirical correction factor: " << correctionFactor << endl;
        cout << "Original threshold: " << bestT << ", Corrected threshold: " << correctedThreshold << endl;
        
        bestT = correctedThreshold;
    }
    
    // Restore original threshold
    params.threshold = origThreshold;
    
    return bestT;
}

double ImageProcessor::compressWithThreshold(double threshold) {
    // Save original threshold
    double originalThreshold = params.threshold;
    
    // Set the new threshold for testing
    params.threshold = threshold;
    
    // Build a test tree
    shared_ptr<QuadTreeNode> tempRoot = buildQuadTree(0, 0, imageWidth, imageHeight, 0);
    
    if (!tempRoot) {
        // If building fails, restore parameters and return
        params.threshold = originalThreshold;
        return 0.0;
    }
    
    // Create a temporary QuadTree
    QuadTree testTree;
    testTree.setRoot(tempRoot);
    testTree.calculateDepthAndNodeCount();
    
    // Calculate the theoretical compressed size more accurately
    size_t estimatedCompressedSize = calculateTheoricalCompressedSize(testTree);
    
    // For small images, ensure we account for minimum file size overhead
    const size_t MIN_FILE_SIZE = 1024; // 1KB minimum overhead
    estimatedCompressedSize = max(estimatedCompressedSize, MIN_FILE_SIZE);
    
    // Calculate compression percentage
    double compressionRatio = 0.0;
    if (originalImageSize > 0) {
        compressionRatio = 1.0 - (static_cast<double>(estimatedCompressedSize) / 
                                  static_cast<double>(originalImageSize));
    }
    
    // Apply a correction factor based on empirical observation
    // This helps align theoretical with actual compression
    double correctionFactor = 0.7;  // Reduce expected compression to be more realistic
    compressionRatio *= correctionFactor;
    
    // Bound the result to valid range
    compressionRatio = max(0.0, min(0.99, compressionRatio));
    
    // Restore original threshold
    params.threshold = originalThreshold;
    
    return compressionRatio;
}

size_t ImageProcessor::calculateTheoricalCompressedSize(const QuadTree& tree) const {
    if (!tree.getRoot()) {
        return 0;
    }
    
    // Count leaf nodes
    int leafCount = 0;
    
    // Function to recursively count leaf nodes
    function<void(shared_ptr<QuadTreeNode>)> countLeaves = [&](shared_ptr<QuadTreeNode> node) {
        if (!node) return;
        
        if (node->isLeaf()) {
            leafCount++;
        } else {
            for (const auto& child : node->getChildren()) {
                countLeaves(child);
            }
        }
    };
    
    countLeaves(tree.getRoot());
    
    // Each leaf node needs:
    // - Position (x,y): 2 integers = 8 bytes
    // - Size (width,height): 2 integers = 8 bytes
    // - Color (r,g,b): 3 bytes
    const size_t BYTES_PER_LEAF = 24;
    
    // Add header overhead
    const size_t HEADER_SIZE = 64;
    
    // Calculate base size with overhead
    size_t baseSize = HEADER_SIZE + (leafCount * BYTES_PER_LEAF);
    
    // Add format-specific overhead based on output format
    string outputFormat = params.outputImagePath.substr(params.outputImagePath.find_last_of(".") + 1);
    
    double formatFactor = 1.0;
    if (outputFormat == "jpg" || outputFormat == "jpeg") {
        // JPEG has compression, but has quality overhead
        formatFactor = 1.5; // Increase overhead estimation
    } else if (outputFormat == "png") {
        // PNG has its own compression but higher overhead for small images
        formatFactor = 2.0; // Increase overhead estimation significantly
    }
    
    // Scale based on minimum block size - smaller blocks need more overhead
    double blockSizeFactor = 16.0 / params.minBlockSize;
    
    // Calculate final estimated size with all factors
    return static_cast<size_t>(baseSize * formatFactor * blockSizeFactor);
}

bool ImageProcessor::saveCompressedImage(const string& outputPath) {
    if (!quadTree.getRoot()) {
        cerr << "No quadtree to save" << endl;
        return false;
    }
    
    try {
        cout << "Saving compressed image to: " << outputPath << endl;
        
        // Create a new image with the same dimensions
        cv::Mat outputImage(imageHeight, imageWidth, CV_8UC3, cv::Scalar(0, 0, 0));
        
        // Helper function to recursively fill in the image
        function<void(shared_ptr<QuadTreeNode>)> renderNode = [&](shared_ptr<QuadTreeNode> node) {
            if (!node) return;
            
            if (node->isLeaf()) {
                // Fill this block with the average color
                Pixel color = node->getColor();
                cv::Scalar pixelColor(color.b, color.g, color.r); // RGB to BGR
                
                // Make sure coordinates are valid
                int x = max(0, node->getX());
                int y = max(0, node->getY());
                int width = min(node->getWidth(), imageWidth - x);
                int height = min(node->getHeight(), imageHeight - y);
                
                if (width > 0 && height > 0) {
                    // Draw a filled rectangle
                    cv::rectangle(
                        outputImage,
                        cv::Point(x, y),
                        cv::Point(x + width, y + height),
                        pixelColor,
                        -1 // Filled rectangle
                    );
                }
            } else {
                // Process children
                for (const auto& child : node->getChildren()) {
                    renderNode(child);
                }
            }
        };
        
        // Render the image
        renderNode(quadTree.getRoot());
        
        // Set compression parameters for JPEG/PNG
        vector<int> compression_params;
        if (outputPath.find(".jpg") != string::npos || 
            outputPath.find(".jpeg") != string::npos) {
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(85); // Quality parameter
        } else if (outputPath.find(".png") != string::npos) {
            compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
            compression_params.push_back(9); // Maximum compression
        }
        
        // Save the image with compression parameters
        bool success = cv::imwrite(outputPath, outputImage, compression_params);
        if (!success) {
            cerr << "Failed to save image to: " << outputPath << endl;
            return false;
        }
        
        // Get the actual file size after saving
        compressedImageSize = getFileSize(outputPath);
        
        cout << "Image saved successfully" << endl;
        return true;

    } catch (const exception& e) {
        cerr << "Exception saving image: " << e.what() << endl;
        return false;
    } catch (...) {
        cerr << "Unknown exception saving image" << endl;
        return false;
    }
}
