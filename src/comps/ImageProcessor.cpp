#include "ImageProcessor.hpp"
#include <cmath>
#include <algorithm>
#include <limits>
#include <iostream>
#include <fstream>

// Include OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

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
        compressedImageSize = tree.getNodeCount() * (4 * sizeof(int) + 3 * sizeof(char));
        
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
    
    // Only subdivide if the error is above threshold AND the blocks won't be too small
    if (shouldDivide && width >= 2 * params.minBlockSize && height >= 2 * params.minBlockSize) {
        int halfWidth = width / 2;
        int halfHeight = height / 2;
        
        auto topLeft = buildQuadTree(x, y, halfWidth, halfHeight, depth + 1);
        auto topRight = buildQuadTree(x + halfWidth, y, halfWidth, halfHeight, depth + 1);
        auto bottomLeft = buildQuadTree(x, y + halfHeight, halfWidth, halfHeight, depth + 1);
        auto bottomRight = buildQuadTree(x + halfWidth, y + halfHeight, halfWidth, halfHeight, depth + 1);
        
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
    // Step 1: Sample a wide range of thresholds to understand the relationship
    vector<pair<double, double>> samples;
    vector<double> thresholdValues = {0.1, 0.5, 1, 5, 10, 20, 50, 100, 200, 500, 1000};
    
    for (double threshold : thresholdValues) {
        double compressionRatio = compressWithThreshold(threshold);
        samples.push_back({threshold, compressionRatio});
        cout << "Sample threshold " << threshold << " gives compression " 
                 << (compressionRatio * 100) << "%" << endl;
    }
    
    // Step 2: Find the two points that bracket our target compression
    sort(samples.begin(), samples.end(), 
              [](const auto& a, const auto& b) { return a.second < b.second; });
    
    // If target is outside our sample range, extend the range
    if (targetPercentage < samples.front().second) {
        return 10000; // Very high threshold for very low compression
    }
    if (targetPercentage > samples.back().second) {
        return 0.01;  // Very low threshold for very high compression
    }
    
    // Find bracketing points
    for (size_t i = 0; i < samples.size() - 1; i++) {
        if ((samples[i].second <= targetPercentage && samples[i+1].second >= targetPercentage) ||
            (samples[i].second >= targetPercentage && samples[i+1].second <= targetPercentage)) {
            
            double t1 = samples[i].first;
            double c1 = samples[i].second;
            double t2 = samples[i+1].first;
            double c2 = samples[i+1].second;
            
            // Linear interpolation to estimate target threshold
            double targetThreshold = t1 + (t2 - t1) * (targetPercentage - c1) / (c2 - c1);
            
            // Test the interpolated threshold
            double finalCompression = compressWithThreshold(targetThreshold);
            cout << "Interpolated threshold " << targetThreshold 
                     << " gives compression " << (finalCompression * 100) << "%" << endl;
            
            // If we're still far off, do a few more iterations of interpolation
            double currentDiff = abs(finalCompression - targetPercentage);
            if (currentDiff > 0.05) { // If more than 5% off target
                t1 = targetThreshold;
                c1 = finalCompression;
                
                // Try 3 more iterations
                for (int iter = 0; iter < 3; iter++) {
                    targetThreshold = t1 + (t2 - t1) * (targetPercentage - c1) / (c2 - c1);
                    finalCompression = compressWithThreshold(targetThreshold);
                    
                    if (abs(finalCompression - targetPercentage) < currentDiff) {
                        currentDiff = abs(finalCompression - targetPercentage);
                        t1 = targetThreshold;
                        c1 = finalCompression;
                    } else {
                        break; // Not improving
                    }
                    
                    cout << "Refined threshold " << targetThreshold 
                             << " gives compression " << (finalCompression * 100) << "%" << endl;
                }
            }
            
            return targetThreshold;
        }
    }
    
    // Fallback
    return thresholdValues[thresholdValues.size() / 2];
}

double ImageProcessor::compressWithThreshold(double threshold) {
    // Save original threshold and minimum block size
    double originalThreshold = params.threshold;
    int originalMinBlockSize = params.minBlockSize;
    
    // Set the new threshold for testing
    params.threshold = threshold;
    
    // Build a new quadtree with this threshold
    QuadTree testTree;
    auto tempRoot = buildQuadTree(0, 0, imageWidth, imageHeight, 0);
    
    if (!tempRoot) {
        // If building fails, restore parameters and return
        params.threshold = originalThreshold;
        params.minBlockSize = originalMinBlockSize;
        return 0.0;
    }
    
    testTree.setRoot(tempRoot);
    testTree.calculateDepthAndNodeCount();
    
    // Count leaf nodes only
    int leafNodes = 0;
    function<void(shared_ptr<QuadTreeNode>)> countLeaves = [&](shared_ptr<QuadTreeNode> node) {
        if (!node) return;
        
        if (node->isLeaf()) {
            leafNodes++;
        } else {
            for (const auto& child : node->getChildren()) {
                countLeaves(child);
            }
        }
    };
    
    countLeaves(tempRoot);
    
    // Calculate theoretical compressed size
    size_t nodeDataSize = leafNodes * (4 * sizeof(int) + 3);
    size_t headerSize = sizeof(int) * 2;
    size_t theoreticCompressedSize = headerSize + nodeDataSize;
    
    // Calculate compression percentage
    double compressionRatio = 1.0 - (static_cast<double>(theoreticCompressedSize) / static_cast<double>(originalImageSize));
    
    // Restore original parameters
    params.threshold = originalThreshold;
    params.minBlockSize = originalMinBlockSize;
    
    cout << "Threshold: " << threshold << " gives " << leafNodes << " leaf nodes and " 
              << fixed << setprecision(2) << (compressionRatio * 100) << "% compression" << endl;
    
    return compressionRatio;
}

size_t ImageProcessor::calculateTheoricalCompressedSize(const QuadTree& tree) const {
    // Count leaf nodes
    int leafCount = 0;
    
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
    
    // Each leaf node stores: position (2 integers), size (2 integers), and color (3 bytes)
    size_t nodeDataSize = leafCount * (4 * sizeof(int) + 3);
    
    // Add a small header size
    size_t headerSize = sizeof(int) * 2; // width and height
    
    return headerSize + nodeDataSize;
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