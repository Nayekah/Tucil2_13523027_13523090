// include header file
#include "ImageProcessor.hpp"


ImageProcessor::ImageProcessor(const CompressionParams& params): params(params), imageWidth(0), imageHeight(0), originalImageSize(0), compressedImageSize(0) {
    // cons
    
    initializeErrorCalculator();
}

ImageProcessor::~ImageProcessor() {
    // dtor
}


// file size getter
size_t ImageProcessor::getFileSize(const string& filename) const {
    ifstream file(filename, ios::binary | ios::ate);
    if (!file.is_open()) return 0;
    return file.tellg();
}


// take error calculation
void ImageProcessor::initializeErrorCalculator() {
    errorCalculator = ErrorCalculator::create(params.errorMethod);
    if (!errorCalculator) {
        cerr << "Failed to create error calculator. Using default Variance method." << endl;
        errorCalculator = make_unique<VarianceErrorCalculator>();
    }
}


// adjust minimum block size
void ImageProcessor::adjustMinimumBlockSize() {
    if (imageWidth <= 0 || imageHeight <= 0) {
        return;
    }
    
    // Calculate the maximum possible area for a block (the entire image)
    int maxPossibleArea = imageWidth * imageHeight;
    
    // Calculate reasonable maximum minimum block size (1/4 of the image area)
    int recommendedMaxMinBlockSize = maxPossibleArea / 4;
    
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


// image loader
bool ImageProcessor::loadImage(const string& imagePath) {
    try {
        cout << "Loading image: " << imagePath << endl;
        
        cv::Mat image = cv::imread(imagePath, cv::IMREAD_COLOR);
        
        if (image.empty()) {
            cerr << "Failed to load image: " << imagePath << endl;
            return false;
        }
        
        // dimensions
        imageWidth = image.cols;
        imageHeight = image.rows;
        
        cout << "Image loaded: " << imageWidth << "x" << imageHeight << " pixels" << endl;
        
        if (imageWidth <= 0 || imageHeight <= 0) {
            cerr << "Invalid image dimensions" << endl;
            return false;
        }
        
        originalImageSize = getFileSize(imagePath);
        
        // Convert to pixel
        pixels.clear();
        pixels.resize(imageHeight, vector<Pixel>(imageWidth));
        
        for (int y = 0; y < imageHeight; ++y) {
            for (int x = 0; x < imageWidth; ++x) {
                cv::Vec3b color = image.at<cv::Vec3b>(y, x);
                pixels[y][x] = Pixel(color[2], color[1], color[0]);
            }
        }
        
        cout << "Image converted to internal format" << endl;

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

// compressor
QuadTree ImageProcessor::compressImage() {
    cout << "Starting image compression..." << endl;
    QuadTree tree;
    
    try {
        if (imageWidth <= 0 || imageHeight <= 0 || pixels.empty()) {
            cerr << "No valid image data to compress" << endl;
            return tree;
        }

        if (params.targetCompressionPercentage > 0.0) {
            cout << "Using target compression: " << (params.targetCompressionPercentage * 100) << "%" << endl;
            params.threshold = findThresholdForTargetCompression(params.targetCompressionPercentage);
            cout << "Adjusted threshold: " << params.threshold << endl;
        }
        
        cout << "Building quadtree..." << endl;
        shared_ptr<QuadTreeNode> root = buildQuadTree(0, 0, imageWidth, imageHeight, 0);
        
        if (!root) {
            cerr << "Failed to build quadtree root" << endl;
            return tree;
        }
        
        tree.setRoot(root);
        tree.calculateDepthAndNodeCount();
        
        cout << "QuadTree built successfully: " 
                  << "depth=" << tree.getDepth() 
                  << ", nodes=" << tree.getNodeCount() << endl;
        
        compressedImageSize = calculateTheoricalCompressedSize(tree);
        
        quadTree = tree;
        
        return tree;
    } catch (const exception& e) {
        cerr << "Exception during compression: " << e.what() << endl;
        return tree;
    } catch (...) {
        cerr << "Unknown exception during compression" << endl;
        return tree;
    }
}

// region checker
bool ImageProcessor::isValidRegion(int x, int y, int width, int height) const {
    return x >= 0 && y >= 0 && width > 0 && height > 0 && x + width <= imageWidth && y + height <= imageHeight;
}


// main algo (recursive quadtree compression)
shared_ptr<QuadTreeNode> ImageProcessor::buildQuadTree(int x, int y, int width, int height, int depth) {
    // Create a new node for this region
    auto node = make_shared<QuadTreeNode>(x, y, width, height);
    
    // Set the average color for this node ()
    node->setColor(calculateAverageColor(x, y, width, height));
    
    double error = 0.0;
    bool shouldDivide = shouldSubdivide(x, y, width, height, error); // checker for subdivide, relatif berdasarkan threshold
    
    // partisi blok
    int halfWidth = width / 2;
    int remainderWidth = width - halfWidth;
    int halfHeight = height / 2;
    int remainderHeight = height - halfHeight;
    int subBlockArea = halfWidth * halfHeight;
    
    // subdivision algo
    if (shouldDivide && subBlockArea >= params.minBlockSize) {
        auto topLeft = buildQuadTree(x, y, halfWidth, halfHeight, depth + 1);
        auto topRight = buildQuadTree(x + halfWidth, y, remainderWidth, halfHeight, depth + 1);
        auto bottomLeft = buildQuadTree(x, y + halfHeight, halfWidth, remainderHeight, depth + 1);
        auto bottomRight = buildQuadTree(x + halfWidth, y + halfHeight, remainderWidth, remainderHeight, depth + 1);
        
        // valid children
        if (topLeft) node->addChild(topLeft);
        if (topRight) node->addChild(topRight);
        if (bottomLeft) node->addChild(bottomLeft);
        if (bottomRight) node->addChild(bottomRight);
    }
    
    return node;
}

// checker if region should be subdivided
bool ImageProcessor::shouldSubdivide(int x, int y, int width, int height, double& error) {
    if (!errorCalculator) {
        cerr << "Error calculator is null" << endl;
        return false;
    }
    
    if (!isValidRegion(x, y, width, height)) {
        return false;
    }
    
    try {
        error = errorCalculator->calculateError(pixels, x, y, width, height);
        
        return error > params.threshold;
    } catch (const exception& e) {
        cerr << "Exception in shouldSubdivide: " << e.what() << endl;
        return false;
    } catch (...) {
        cerr << "Unknown exception in shouldSubdivide" << endl;
        return false;
    }
}

// color normalization
Pixel ImageProcessor::calculateAverageColor(int x, int y, int width, int height) {
    if (!isValidRegion(x, y, width, height)) {
        return Pixel(0, 0, 0);
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


// compress fuzz (for targetted compression)
double ImageProcessor::compressWithThreshold(double threshold) {
    // Save original threshold (from input)
    double originalThreshold = params.threshold;
    
    params.threshold = threshold;
    
    // Build test tree
    shared_ptr<QuadTreeNode> tempRoot = buildQuadTree(0, 0, imageWidth, imageHeight, 0);
    
    if (!tempRoot) {
        params.threshold = originalThreshold;
        return 0.0;
    }
    
    QuadTree testTree;
    testTree.setRoot(tempRoot);
    testTree.calculateDepthAndNodeCount();
    
    // Calculate the theoretical compressed size
    size_t estimatedCompressedSize = calculateTheoricalCompressedSize(testTree);
    
    const size_t MIN_FILE_SIZE = 1024;
    estimatedCompressedSize = max(estimatedCompressedSize, MIN_FILE_SIZE);
    
    // Calculate compression percentage
    double compressionRatio = 0.0;
    if (originalImageSize > 0) {
        compressionRatio = 1.0 - (static_cast<double>(estimatedCompressedSize) / static_cast<double>(originalImageSize));
    }
    
    // Apply a correction factor based on empirical observation

    double correctionFactor = 0.7;  // Reduce expected compression
    compressionRatio *= correctionFactor;
    
    // Bound the result to valid range
    compressionRatio = max(0.0, min(0.99, compressionRatio));
    
    // Restore original threshold
    params.threshold = originalThreshold;
    
    return compressionRatio;
}


// main algo for targetted compression
double ImageProcessor::findThresholdForTargetCompression(double targetPercentage) {
    // Save original threshold
    double originalThreshold = params.threshold;
    string outputPath = params.outputImagePath;
    string extension = outputPath.substr(outputPath.find_last_of("."));
    
    double lowT = 0.0, highT = 0.0;
    switch (params.errorMethod) {
        case ErrorMethod::VARIANCE:
            lowT = 0.0;   highT = 16256.25;
            break;
        case ErrorMethod::MEAN_ABSOLUTE_DEVIATION:
            lowT = 0.0;   highT = 127.5;
            break;
        case ErrorMethod::MAX_PIXEL_DIFFERENCE:
            lowT = 0.0;   highT = 255.0;
            break;
        case ErrorMethod::ENTROPY:
            lowT = 0.0;  highT = 8.0;
            break;
        case ErrorMethod::STRUCTURAL_SIMILARITY:
            lowT = 0.0;  highT = 1.0;
            break;
    }
    
    mutex mtx;
    
    unsigned int systemThreads = thread::hardware_concurrency();
    unsigned int numThreads = max(3u, systemThreads > 1 ? systemThreads - 1 : 1);
    
    auto getCompressionRatio = [this, &extension, &mtx](double thresh) -> double {
        lock_guard<mutex> lock(mtx);
        double oldThreshold = params.threshold;
        params.threshold = thresh;
        
        shared_ptr<QuadTreeNode> localRoot = buildQuadTree(0, 0, imageWidth, imageHeight, 0);
        if (!localRoot) {
            params.threshold = oldThreshold;
            return -1.0;
        }
        
        QuadTree localTree;
        localTree.setRoot(localRoot);
        localTree.calculateDepthAndNodeCount();
        this->quadTree = localTree;
        
        vector<unsigned char> buffer;
        if (!saveCompressedImageToBuffer(extension, buffer)) {
            params.threshold = oldThreshold;
            return -1.0;
        }
        
        double compressionRatio = 1.0 - (static_cast<double>(buffer.size()) / originalImageSize);
        params.threshold = oldThreshold;
        return compressionRatio;
    };
    
    map<double, double> cache;
    
    const double tolerance = 1e-6; // tolerance
    const int maxIterations = 25; // capper for the computation time
    
    vector<double> initialPoints;
    initialPoints.push_back(lowT);
    initialPoints.push_back((lowT + highT) / 2.0);
    initialPoints.push_back(highT);
    
    // Add more points based on available threads
    if (numThreads > 3) {
        initialPoints.push_back(lowT + (highT - lowT) / 4.0);
        initialPoints.push_back(lowT + 3 * (highT - lowT) / 4.0);
    }
    
    // Launch initial evaluations in parallel
    vector<future<double>> initialFutures;
    for (double point : initialPoints) {
        initialFutures.push_back(async(launch::async, getCompressionRatio, point));
    }
    
    // Collect initial results
    for (size_t i = 0; i < initialPoints.size(); i++) {
        double ratio = initialFutures[i].get();
        cache[initialPoints[i]] = ratio;
        
        /*
        cout << "Initial point: threshold = " << initialPoints[i] 
             << ", compression ratio = " << ratio << endl;
        */
    }
    
    // Checker if the target is achiaveable (dari min max)
    double minRatio = numeric_limits<double>::max();
    double maxRatio = numeric_limits<double>::lowest();
    for (const auto& entry : cache) {
        minRatio = min(minRatio, entry.second);
        maxRatio = max(maxRatio, entry.second);
    }
    
    if (targetPercentage < minRatio || targetPercentage > maxRatio) {
        cout << "Target compression of " << (targetPercentage * 100) << "% is not achievable within threshold range. Using inputted threshold..." << endl;
        return originalThreshold;
    }
    
    // Track best threshold found so far
    double bestThreshold = initialPoints[0];
    double bestDiff = abs(cache[initialPoints[0]] - targetPercentage);
    
    for (const auto& entry : cache) {
        double diff = abs(entry.second - targetPercentage);
        if (diff < bestDiff) {
            bestDiff = diff;
            bestThreshold = entry.first;
        }
    }
    
    // Binary search w/ multithread
    int iteration = 0;
    while (iteration++ < maxIterations && bestDiff > tolerance) {
        vector<pair<double, double>> points;
        for (const auto& entry : cache) {
            points.push_back({entry.first, entry.second});
        }
        sort(points.begin(), points.end());
        
        // Find the interval where our target compression ratio falls
        bool foundInterval = false;
        double leftT = lowT, rightT = highT;
        
        for (size_t i = 0; i < points.size() - 1; i++) {
            double t1 = points[i].first;
            double r1 = points[i].second;
            double t2 = points[i + 1].first;
            double r2 = points[i + 1].second;
            
            bool inRange = (r1 <= r2 && targetPercentage >= r1 && targetPercentage <= r2) ||
                           (r1 >= r2 && targetPercentage <= r1 && targetPercentage >= r2);
            
            if (inRange) {
                leftT = t1;
                rightT = t2;
                foundInterval = true;
                break;
            }
        }
        
        // Try to add more evaluation points
        if (!foundInterval) {
            vector<double> newPoints;
            for (size_t i = 0; i < points.size() - 1; i++) {
                double mid = (points[i].first + points[i + 1].first) / 2.0;
                if (cache.find(mid) == cache.end()) {
                    newPoints.push_back(mid);
                }
            }
            
            while (newPoints.size() > numThreads) {
                // remove points from the smallest intervals first :3

                size_t minSpacingIdx = 0;
                double minSpacing = numeric_limits<double>::max();
                
                for (size_t i = 0; i < newPoints.size(); i++) {
                    size_t idx = 0;
                    while (idx < points.size() && points[idx].first < newPoints[i]) {
                        idx++;
                    }
                    
                    double spacing = 0.0;
                    if (idx > 0 && idx < points.size()) {
                        spacing = points[idx].first - points[idx - 1].first;
                    }
                    
                    if (spacing < minSpacing) {
                        minSpacing = spacing;
                        minSpacingIdx = i;
                    }
                }
                
                newPoints.erase(newPoints.begin() + minSpacingIdx);
            }
            
            if (newPoints.empty()) {
                break;
            }
            
            // Evaluate new points in parallel
            vector<future<double>> futures;
            for (double point : newPoints) {
                futures.push_back(async(launch::async, getCompressionRatio, point));
            }
            
            // Collect results
            for (size_t i = 0; i < newPoints.size(); i++) {
                double ratio = futures[i].get();
                cache[newPoints[i]] = ratio;
                
                double diff = abs(ratio - targetPercentage);
                if (diff < bestDiff) {
                    bestDiff = diff;
                    bestThreshold = newPoints[i];
                }
                
                /*
                cout << "Iteration " << iteration << ", threshold = " << newPoints[i] 
                     << ", compression = " << ratio << ", diff = " << diff << endl;
                */
            }
            
            continue;
        }
        
        // Update search range (iteration)
        lowT = leftT;
        highT = rightT;
        
        // Generate test points within the current interval
        vector<double> testPoints;
        
        double midT = (lowT + highT) / 2.0;
        if (cache.find(midT) == cache.end()) {
            testPoints.push_back(midT);
        }
        
        // Add quarter points if interval is large enough
        double range = highT - lowT;
        if (range > (initialPoints.back() - initialPoints.front()) / 100.0) {
            double quarterT = lowT + range / 4.0;
            double threeQuarterT = lowT + 3.0 * range / 4.0;
            
            if (cache.find(quarterT) == cache.end()) {
                testPoints.push_back(quarterT);
            }
            
            if (cache.find(threeQuarterT) == cache.end()) {
                testPoints.push_back(threeQuarterT);
            }
        }
        
        if (testPoints.empty()) {
            break;
        }
        
        // Evaluate test points in parallel
        vector<future<double>> futures;
        for (double point : testPoints) {
            futures.push_back(async(launch::async, getCompressionRatio, point));
        }
        
        // Collect results
        for (size_t i = 0; i < testPoints.size(); i++) {
            double ratio = futures[i].get();
            cache[testPoints[i]] = ratio;
            
            double diff = abs(ratio - targetPercentage);
            if (diff < bestDiff) {
                bestDiff = diff;
                bestThreshold = testPoints[i];
            }
            
            /*
            cout << "Iteration " << iteration << ", threshold = " << testPoints[i] 
                 << ", compression = " << ratio << ", diff = " << diff << endl;
            */
        }
    }
    
    cout << "Best threshold found: " << bestThreshold 
         << " (iteration " << iteration << "/" << maxIterations 
         << ", difference: " << bestDiff << ")" << endl;
    
    return bestThreshold;
}


// calculating compressed size theoretically (helper for targetted compression - might help? hehe)
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
    
    // Each leaf node needs: (theoretically)
    // - Position (x,y): 2 integers = 8 bytes
    // - Size (width,height): 2 integers = 8 bytes
    // - Color (r,g,b): 3 bytes
    const size_t BYTES_PER_LEAF = 24;
    
    // Add header
    const size_t HEADER_SIZE = 64;
    
    // Calculate base size
    size_t baseSize = HEADER_SIZE + (leafCount * BYTES_PER_LEAF);

    string outputFormat = params.outputImagePath.substr(params.outputImagePath.find_last_of(".") + 1);
    
    double formatFactor = 1.0;
    if (outputFormat == "jpg" || outputFormat == "jpeg") {
        formatFactor = 1.5;
    } else if (outputFormat == "png") {
        formatFactor = 2.0;
    }
    
    double blockSizeFactor = 16.0 / params.minBlockSize;
    
    return static_cast<size_t>(baseSize * formatFactor * blockSizeFactor);
}


// converter from compressed to image format
bool ImageProcessor::saveCompressedImage(const string& outputPath) {
    if (!quadTree.getRoot()) {
        cerr << "No quadtree to save" << endl;
        return false;
    }
    
    // Periksa apakah outputPath adalah path temporary
    bool isTemp = (outputPath.find("/tmp/tucil_temp") != string::npos);
    
    try {
        if (!isTemp) {
            cout << "Saving compressed image to: " << outputPath << endl;
        }
        
        // Buat gambar baru dengan dimensi yang sama
        cv::Mat outputImage(imageHeight, imageWidth, CV_8UC3, cv::Scalar(0, 0, 0));
        
        // Fungsi rekursif untuk merender QuadTree ke gambar
        function<void(shared_ptr<QuadTreeNode>)> renderNode = [&](shared_ptr<QuadTreeNode> node) {
            if (!node) return;
            
            if (node->isLeaf()) {
                // Gambar blok dengan warna rata-rata
                Pixel color = node->getColor();
                cv::Scalar pixelColor(color.b, color.g, color.r);
                int x = max(0, node->getX());
                int y = max(0, node->getY());
                int width = min(node->getWidth(), imageWidth - x);
                int height = min(node->getHeight(), imageHeight - y);
                if (width > 0 && height > 0) {
                    cv::rectangle(outputImage, cv::Point(x, y), cv::Point(x + width, y + height), pixelColor, -1);
                }
            } else {
                for (const auto& child : node->getChildren()) {
                    renderNode(child);
                }
            }
        };
        
        // Render QuadTree ke dalam outputImage
        renderNode(quadTree.getRoot());
        
        // Tentukan parameter kompresi berdasarkan ekstensi file
        vector<int> compression_params;
        if (outputPath.find(".jpg") != string::npos || outputPath.find(".jpeg") != string::npos) {
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(85);
        } else if (outputPath.find(".png") != string::npos) {
            compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
            compression_params.push_back(9);
        }
        
        bool success = cv::imwrite(outputPath, outputImage, compression_params);
        if (!success) {
            if (!isTemp) {
                cerr << "Failed to save image to: " << outputPath << endl;
            }
            return false;
        }
        
        compressedImageSize = getFileSize(outputPath);
        
        if (!isTemp) {
            cout << "Image saved successfully" << endl;
        }
        return true;
        
    } catch (const exception& e) {
        if (!isTemp) {
            cerr << "Exception saving image: " << e.what() << endl;
        }
        return false;
    } catch (...) {
        if (!isTemp) {
            cerr << "Unknown exception saving image" << endl;
        }
        return false;
    }
}


// converter from compressed to buffer (helper for targetted compress)
bool ImageProcessor::saveCompressedImageToBuffer(const string& extension, vector<unsigned char>& buffer) {
    if (!quadTree.getRoot()) {
        cerr << "No quadtree to save" << endl;
        return false;
    }
    
    try {
        cv::Mat outputImage(imageHeight, imageWidth, CV_8UC3, cv::Scalar(0, 0, 0));
        
        // render quadtree to image
        function<void(shared_ptr<QuadTreeNode>)> renderNode = [&](shared_ptr<QuadTreeNode> node) {
            if (!node) return;
            if (node->isLeaf()) {
                Pixel color = node->getColor();
                cv::Scalar pixelColor(color.b, color.g, color.r);
                int x = max(0, node->getX());
                int y = max(0, node->getY());
                int width = min(node->getWidth(), imageWidth - x);
                int height = min(node->getHeight(), imageHeight - y);
                if (width > 0 && height > 0) {
                    cv::rectangle(outputImage, cv::Point(x, y), cv::Point(x + width, y + height), pixelColor, -1);
                }
            } else {
                for (const auto& child : node->getChildren())
                    renderNode(child);
            }
        };
        
        renderNode(quadTree.getRoot());
        
        // extension
        vector<int> compression_params;
        if (extension == ".jpg" || extension == ".jpeg") {
            compression_params.push_back(cv::IMWRITE_JPEG_QUALITY);
            compression_params.push_back(85);
        } else if (extension == ".png") {
            compression_params.push_back(cv::IMWRITE_PNG_COMPRESSION);
            compression_params.push_back(9);
        }
        
        bool success = cv::imencode(extension, outputImage, buffer, compression_params);
        if (!success) {
            cerr << "Failed to encode image to memory buffer" << endl;
            return false;
        }
        
        // Update file size
        compressedImageSize = buffer.size();
        return true;
        
    } catch (const exception& e) {
        cerr << "Exception in saveCompressedImageToBuffer: " << e.what() << endl;
        return false;
    } catch (...) {
        cerr << "Unknown exception in saveCompressedImageToBuffer" << endl;
        return false;
    }
}
