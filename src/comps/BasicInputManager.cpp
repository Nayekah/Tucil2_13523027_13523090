// include header file
#include "BasicInputManager.hpp"


BasicInputManager::BasicInputManager() {
    // cons
}

BasicInputManager::~BasicInputManager() {
    // dtor
}

// image checker
bool BasicInputManager::isImageFile(const string& filePath) {
    size_t pos = filePath.find_last_of(".");
    if (pos == string::npos) return false;
    
    string ext = filePath.substr(pos);
    for (auto& c : ext) c = tolower(c);
    
    return (ext == ".jpg" || ext == ".jpeg" || ext == ".png");
}

// check apakah file ada atau enggak
bool BasicInputManager::fileExists(const string& filePath) {
    return filesystem::exists(filePath);
}

CompressionParams BasicInputManager::getCompressionParams() {
    CompressionParams params;
    string input;
    
    cin.clear();
    
    // 1. Input image path
    cout << "1. Enter absolute path to image for compression: ";
    getline(cin, input);
    while (input.empty() || !fileExists(input) || !isImageFile(input)) {
        cout << ANSI_RED << "Error: File does not exist or is not a valid image file (.jpg, .jpeg, .png)." << ANSI_RESET << endl;
        cout << "Enter absolute path to image for compression: ";
        getline(cin, input);
    }
    params.inputImagePath = input;
    cout << endl << endl;
    
    // 2. Error calculation method
    cout << "2. Select error calculation method (1-5):" << endl;
    cout << "   1. Variance" << endl;
    cout << "   2. Mean Absolute Deviation (MAD)" << endl;
    cout << "   3. Max Pixel Difference" << endl;
    cout << "   4. Entropy" << endl;
    cout << "   5. Structural Similarity Index (SSIM)" << endl;
    cout << "Enter choice (1-5): ";
    getline(cin, input);
    int methodChoice;
    while (true) {
        try {
            methodChoice = stoi(input);
            if (methodChoice >= 1 && methodChoice <= 5) break;
            cout << ANSI_RED << "Error: Please enter a number between 1 and 5." << ANSI_RESET << endl;
        } catch (const exception&) {
            cout << ANSI_RED << "Error: Please enter a valid number." << ANSI_RESET << endl;
        }
        cout << "Enter choice (1-5): ";
        getline(cin, input);
    }
    params.errorMethod = static_cast<ErrorMethod>(methodChoice);
    cout << endl << endl;
    
    // 3. Threshold
    double min = 0.0, max = 0.0, defaultValue = 0.0;
    string rangeDescription;
    
    switch (params.errorMethod) {
        case ErrorMethod::VARIANCE:
            min = 0.0; max = 16256.25; defaultValue = 500.0;
            rangeDescription = "For Variance method (valid range: 0.0-16256.25)\n"
                              "* Low threshold (high detail): 50-500\n"
                              "* Medium threshold: 500-2000\n"
                              "* High threshold (less detail): 2000-5000";
            break;
        case ErrorMethod::MEAN_ABSOLUTE_DEVIATION:
            min = 0.0; max = 127.5; defaultValue = 15.0;
            rangeDescription = "For MAD method (valid range: 0.0-127.5)\n"
                              "* Low threshold (high detail): 5-15\n"
                              "* Medium threshold: 15-30\n"
                              "* High threshold (less detail): 30-50";
            break;
        case ErrorMethod::MAX_PIXEL_DIFFERENCE:
            min = 0.0; max = 255.0; defaultValue = 30.0;
            rangeDescription = "For Max Pixel Difference (valid range: 0.0-255.0)\n"
                              "* Low threshold (high detail): 10-30\n"
                              "* Medium threshold: 30-60\n"
                              "* High threshold (less detail): 60-100";
            break;
        case ErrorMethod::ENTROPY:
            min = 0.0; max = 8.0; defaultValue = 1.0;
            rangeDescription = "For Entropy method (valid range: 0.0-8.0)\n"
                              "* Low threshold (high detail): 0.1-1.0\n"
                              "* Medium threshold: 1.0-2.0\n"
                              "* High threshold (less detail): 2.0-4.0";
            break;
        case ErrorMethod::STRUCTURAL_SIMILARITY:
            min = 0.0; max = 1.0; defaultValue = 0.05;
            rangeDescription = "For SSIM method (valid range: 0.0-1.0)\n"
                              "* Low threshold (high detail): 0.01-0.05\n"
                              "* Medium threshold: 0.05-0.15\n"
                              "* High threshold (less detail): 0.15-0.3";
            break;
    }
    
    cout << "3. Enter threshold value for error calculation:" << endl;
    cout << ANSI_BOLD << ANSI_RED << "Valid threshold range: " << min << " to " << max << ANSI_RESET << endl;
    cout << rangeDescription << endl;
    cout << "Suggested value: " << defaultValue << endl;
    cout << "Enter threshold: ";
    getline(cin, input);
    
    while (true) {
        if (input.empty()) {
            cout << ANSI_RED << "Error: Threshold value cannot be empty. Please enter a value." << ANSI_RESET << endl;
            cout << "Enter threshold: ";
            getline(cin, input);
            continue;
        }
        
        try {
            double threshold = stod(input);
            if (threshold >= min && threshold <= max) {
                params.threshold = threshold;
                break;
            }
            cout << ANSI_RED << "Error: Value out of range. Please enter a value between " << min << " and " << max << "." << ANSI_RESET << endl;
        } catch (const exception&) {
            cout << ANSI_RED << "Error: Please enter a valid number." << ANSI_RESET << endl;
        }
        cout << "Enter threshold: ";
        getline(cin, input);
    }
    cout << endl << endl;
    
    // 4. Minimum block size
    cout << "4. Enter minimum block size in square pixels (area, not length)" << endl;
    cout << "   This is the smallest area (width × height) that will be used during compression." << endl;
    cout << "   Recommended values:" << endl;
    cout << "   * 4 (2×2) - High detail" << endl;
    cout << "   * 16 (4×4) - Good detail" << endl;
    cout << "   * 64 (8×8) - Medium detail" << endl;
    cout << "   * 256 (16×16) - Low detail" << endl;
    cout << "Enter minimum block size: ";
    getline(cin, input);
    
    while (true) {
        if (input.empty()) {
            cout << ANSI_RED << "Error: Minimum block size cannot be empty. Please enter a value." << ANSI_RESET << endl;
            cout << "Enter minimum block size: ";
            getline(cin, input);
            continue;
        }
        
        try {
            int blockSize = stoi(input);
            if (blockSize >= 1) {
                params.minBlockSize = blockSize;
                break;
            }
            cout << ANSI_RED << "Error: Block size must be at least 1 square pixel." << ANSI_RESET << endl;
        } catch (const exception&) {
            cout << ANSI_RED << "Error: Please enter a valid number." << ANSI_RESET << endl;
        }
        cout << "Enter minimum block size: ";
        getline(cin, input);
    }
    cout << endl << endl;
    
    // 5. Target compression percentage
    cout << "5. Enter target compression percentage (0.0-1.0)" << endl;
    cout << "   This is a bonus feature that automatically adjusts the threshold." << endl;
    cout << "   Enter 0 to disable automatic threshold adjustment." << endl;
    cout << "Enter target compression (0.0-1.0): ";
    getline(cin, input);
    
    while (true) {
        try {
            double percentage = stod(input);
            if (percentage >= 0.0 && percentage <= 1.0) {
                params.targetCompressionPercentage = percentage;
                break;
            }
            cout << ANSI_RED << "Error: Please enter a value between 0.0 and 1.0." << ANSI_RESET << endl;
        } catch (const exception&) {
            cout << ANSI_RED << "Error: Please enter a valid number." << ANSI_RESET << endl;
        }
        cout << "Enter target compression: ";
        getline(cin, input);
    }
    cout << endl << endl;
    
    // 6. Output image path
    cout << "6. Enter absolute path for output image:" << endl;
    
    filesystem::path inputPath(params.inputImagePath);
    string inputExt = inputPath.extension().string();
    for (auto& c : inputExt) c = tolower(c);
    
    cout << ANSI_BOLD << ANSI_RED << "NOTE: The output file MUST use the same extension as the input file (" 
          << inputExt << ")" << ANSI_RESET << endl;
          
    cout << "Enter output path: ";
    getline(cin, input);
    
    while (true) {
        if (input.empty()) {
            cout << ANSI_RED << "Error: Output path cannot be empty." << ANSI_RESET << endl;
            cout << "Enter output path: ";
            getline(cin, input);
            continue;
        }

        filesystem::path outputPath(input);
        string outputExt = outputPath.extension().string();
        
        if (outputExt.empty()) {
            cout << ANSI_RED << "Error: No file extension provided." << endl;
            cout << "The output file must have the same extension as the input file (" << inputExt << ")." << ANSI_RESET << endl;
            cout << "Enter output path: ";
            getline(cin, input);
            continue;
        } else {
            for (auto& c : outputExt) c = tolower(c);
            
            if (outputExt != inputExt) {
                cout << ANSI_RED << "Error: Output extension must match input extension (" << inputExt << ")." << ANSI_RESET << endl;
                cout << "Enter output path: ";
                getline(cin, input);
                continue;
            }
        }
        
        filesystem::path parentPath = outputPath.parent_path();
        if (!parentPath.empty() && !filesystem::exists(parentPath)) {
            cout << ANSI_RED << "Error: Output directory does not exist." << ANSI_RESET << endl;
            cout << "Enter output path: ";
            getline(cin, input);
            continue;
        }
        
        params.outputImagePath = input;
        break;
    }
    cout << endl << endl;
    
    // 7. GIF output path (optional)
    cout << "7. Do you want to generate a GIF showing the compression process? (y/n): ";
    getline(cin, input);

    bool generateGif = false;
    while (true) {
        if (input.empty()) {
            cout << ANSI_RED << "Error: Input cannot be empty. Please enter 'y' or 'n'." << ANSI_RESET << endl;
            cout << "Do you want to generate a GIF showing the compression process? (y/n): ";
            getline(cin, input);
            continue;
        }

        string lowerInput = input;
        for (auto& c : lowerInput) c = tolower(c);

        if (lowerInput == "y" || lowerInput == "yes") {
            generateGif = true;
            break;
        } else if (lowerInput == "n" || lowerInput == "no") {
            generateGif = false;
            break;
        } else {
            cout << ANSI_RED << "Invalid input. Please enter 'y' or 'n'." << ANSI_RESET << endl;
            cout << "Do you want to generate a GIF showing the compression process? (y/n): ";
            getline(cin, input);
        }
    }

    params.generateGif = generateGif;
    
    if (generateGif) {
        cout << "Enter absolute path for output GIF (must end with .gif): ";
        getline(cin, input);
        
        while (true) {
            if (input.empty()) {
                cout << ANSI_RED << "Error: GIF output path cannot be empty." << ANSI_RESET << endl;
                cout << "Enter absolute path for output GIF: ";
                getline(cin, input);
                continue;
            }
            
            filesystem::path gifPath(input);
            string gifExt = gifPath.extension().string();
            for (auto& c : gifExt) c = tolower(c);
            
            if (gifExt != ".gif") {
                cout << ANSI_RED << "Error: Output file must have .gif extension." << ANSI_RESET << endl;
                cout << "Enter absolute path for output GIF: ";
                getline(cin, input);
                continue;
            }
            
            filesystem::path parentPath = gifPath.parent_path();
            if (!parentPath.empty() && !filesystem::exists(parentPath)) {
                cout << ANSI_RED << "Error: Output directory does not exist." << ANSI_RESET << endl;
                cout << "Enter absolute path for output GIF: ";
                getline(cin, input);
                continue;
            }
            
            params.gifOutputPath = input;
            break;
        }
    }
    cout << endl << endl;
    
    
    // Hasil
    cout << "\nParameters summary:" << endl;
    cout << "------------------" << endl;
    cout << "Input image: " << params.inputImagePath << endl;
    cout << "Error method: ";
    switch (params.errorMethod) {
        case ErrorMethod::VARIANCE: cout << "Variance"; break;
        case ErrorMethod::MEAN_ABSOLUTE_DEVIATION: cout << "Mean Absolute Deviation"; break;
        case ErrorMethod::MAX_PIXEL_DIFFERENCE: cout << "Max Pixel Difference"; break;
        case ErrorMethod::ENTROPY: cout << "Entropy"; break;
        case ErrorMethod::STRUCTURAL_SIMILARITY: cout << "Structural Similarity Index"; break;
    }
    cout << endl;
    
    cout << "Threshold: " << params.threshold << endl;
    cout << "Minimum block size: " << params.minBlockSize << " square pixels" << endl;
    cout << "Target compression: ";
    if (params.targetCompressionPercentage > 0.0) {
        cout << (params.targetCompressionPercentage * 100) << "%" << endl;
    } else {
        cout << "Disabled" << endl;
    }
    
    cout << "Output image: " << params.outputImagePath << endl;
    if (params.generateGif) {
        cout << "GIF output: " << params.gifOutputPath << endl;
    } else {
        cout << "GIF generation: Disabled" << endl;
    }
    
    cout << "\nPress Enter to start compression or Ctrl+C to cancel..." << endl;
    getline(cin, input);
    
    return params;
}
