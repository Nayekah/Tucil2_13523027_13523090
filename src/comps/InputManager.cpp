// include header file
#include "InputManager.hpp"


// ANSI color definitions
#define ANSI_RESET    "\033[0m"
#define ANSI_BOLD     "\033[1m"
#define ANSI_RED      "\033[31m"
#define ANSI_GREEN    "\033[32m"
#define ANSI_YELLOW   "\033[33m"
#define ANSI_BLUE     "\033[34m"
#define ANSI_MAGENTA  "\033[35m"
#define ANSI_CYAN     "\033[36m"
#define ANSI_WHITE    "\033[37m"


InputManager::InputManager() : currentPage(InputPage::INPUT_IMAGE_PATH) {
    // constructor
}

InputManager::~InputManager() {
    // dtor
}


// Page handler (untuk paging)
CompressionParams InputManager::getCompressionParams() {
    bool goNext = true;
    
    while (true) {
        switch (currentPage) {
            case InputPage::INPUT_IMAGE_PATH:
                goNext = handleInputImagePathPage();
                break;
            case InputPage::ERROR_METHOD:
                goNext = handleErrorMethodPage();
                break;
            case InputPage::THRESHOLD:
                goNext = handleThresholdPage();
                break;
            case InputPage::MIN_BLOCK_SIZE:
                goNext = handleMinBlockSizePage();
                break;
            case InputPage::TARGET_COMPRESSION:
                goNext = handleTargetCompressionPage();
                break;
            case InputPage::OUTPUT_IMAGE_PATH:
                goNext = handleOutputImagePathPage();
                break;
            case InputPage::GIF_OUTPUT_PATH:
                goNext = handleGifOutputPathPage();
                break;
            case InputPage::CONFIRMATION:
                goNext = handleConfirmationPage();
                if (goNext) {
                    return params;
                }
                break;
        }
        
        // Handle navigation
        if (goNext) {
            currentPage = static_cast<InputPage>(static_cast<int>(currentPage) + 1);
        } else {
            if (currentPage == InputPage::INPUT_IMAGE_PATH) {
                continue;
            }
            currentPage = static_cast<InputPage>(static_cast<int>(currentPage) - 1);
        }
    }
}

// Screen flusher untuk refresh
void InputManager::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// spacing
void addVerticalSpacing(int lines = 10) {
    for (int i = 0; i < lines; ++i) {
        cout << endl;
    }
}

// Header untuk setiap halaman
void InputManager::displayPageHeader(const string& title) {
    clearScreen();
    #ifdef _WIN32
        cout <<
"   ____  _    _         _____ _______ _____  ______ ______    _____ ____  __  __ _____  _____  ______  _____ _____  ____  _____  \n"
"  / __ \\| |  | |  /\\   |  __ \\__   __|  __ \\|  ____|  ____|  / ____/ __ \\|  \\/  |  __ \\|  __ \\|  ____|/ ____/ ____|/ __ \\|  __ \\ \n"
" | |  | | |  | | /  \\  | |  | | | |  | |__) | |__  | |__    | |   | |  | | \\  / | |__) | |__) | |__  | (___| (___ | |  | | |__) |\n"
" | |  | | |  | |/ /\\ \\ | |  | | | |  |  _  /|  __| |  __|   | |   | |  | | |\\/| |  ___/|  _  /|  __|  \\___ \\\\___ \\| |  | |  _  / \n"
" | |__| | |__| / ____ \\| |__| | | |  | | \\ \\| |____| |____  | |___| |__| | |  | | |    | | \\ \\| |____ ____) |___) | |__| | | \\ \\ \n"
"  \\___\\_\\\\____/_/    \\_\\_____/  |_|  |_|  \\_\\______|______|  \\_____\\____/|_|  |_|_|    |_|  \\_\\______|_____/_____/ \\____/|_|  \\_\\\n"
"                                                                                                                                 \n"
"                                                                                                                                 \n";
    #else
        // Mau pakai ini buat windows juga tapi gak support :(
        cout <<
        " ██████╗ ██╗   ██╗ █████╗ ██████╗ ████████╗██████╗ ███████╗███████╗     ██████╗ ██████╗ ███╗   ███╗██████╗ ██████╗ ███████╗███████╗███████╗ ██████╗ ██████╗ \n"
        "██╔═══██╗██║   ██║██╔══██╗██╔══██╗╚══██╔══╝██╔══██╗██╔════╝██╔════╝    ██╔════╝██╔═══██╗████╗ ████║██╔══██╗██╔══██╗██╔════╝██╔════╝██╔════╝██╔═══██╗██╔══██╗\n"
        "██║   ██║██║   ██║███████║██║  ██║   ██║   ██████╔╝█████╗  █████╗      ██║     ██║   ██║██╔████╔██║██████╔╝██████╔╝█████╗  ███████╗███████╗██║   ██║██████╔╝\n"
        "██║▄▄ ██║██║   ██║██╔══██║██║  ██║   ██║   ██╔══██╗██╔══╝  ██╔══╝      ██║     ██║   ██║██║╚██╔╝██║██╔═══╝ ██╔══██╗██╔══╝  ╚════██║╚════██║██║   ██║██╔══██╗\n"
        "╚██████╔╝╚██████╔╝██║  ██║██████╔╝   ██║   ██║  ██║███████╗███████╗    ╚██████╗╚██████╔╝██║ ╚═╝ ██║██║     ██║  ██║███████╗███████║███████║╚██████╔╝██║  ██║\n"
        " ╚══▀▀═╝  ╚═════╝ ╚═╝  ╚═╝╚═════╝    ╚═╝   ╚═╝  ╚═╝╚══════╝╚══════╝     ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝\n"
        "\n";
    #endif
    
    cout << ANSI_YELLOW << ANSI_BOLD << "  QuadTree Image Compression - " << title << ANSI_RESET << endl;
    cout << ANSI_GREEN << "Page: [ " << (static_cast<int>(currentPage) + 1) << "/8 ]" << ANSI_RESET << endl;
    addVerticalSpacing();
    cout << "type";
    cout << ANSI_MAGENTA << " 'help' " << ANSI_RESET;
    cout << "for list of commands" << endl;
    cout << endl;
}

void InputManager::displayPageFooter() {
    cout << endl;
    cout << "----------------------------------------" << endl;
    if (currentPage == InputPage::INPUT_IMAGE_PATH) {
        cout << ANSI_MAGENTA << "[Enter] Next" << ANSI_RESET << endl;
    } else if (currentPage == InputPage::CONFIRMATION) {
        cout << ANSI_MAGENTA << "[B] Back   [Enter] Confirm and Process" << ANSI_RESET << endl;
    } else {
        cout << ANSI_MAGENTA << "[B] Back   [Enter] Next" << ANSI_RESET << endl;
    }
    cout << "========================================" << endl;
}


// handle input image
bool InputManager::handleInputImagePathPage() {
    clearScreen();
    displayPageHeader("Input Image Path");
    cout << "Please enter the absolute path to the image to compress:\n\n";

    while (true) {
        string path = getStringInput(" ");
        
        if (path.empty()) {
            cout << ANSI_RED << "Error: Path cannot be empty. Please enter a valid file path." << ANSI_RESET << endl;
            continue;
        }
        
        string lowerInput = path;
        transform(lowerInput.begin(), lowerInput.end(), lowerInput.begin(), ::tolower);

        if (lowerInput == "b") {
            return false;
        }
        else if (lowerInput == "clear") {
            clearScreen();
            displayPageHeader("Input Image Path");
            cout << "Please enter the absolute path to the image to compress:\n\n";
            continue;
        }

        if (!fileExists(path)) {
            cout << ANSI_RED << "Error: File does not exist." << ANSI_RESET << endl;
            continue;
        }
        if (!isImageFile(path)) {
            cout << ANSI_RED << "Error: File is not a supported image type. Use .jpg, .png, or .jpeg." << ANSI_RESET << endl;
            continue;
        }

        params.inputImagePath = path;
        return true;
    }
}

// handle input error method
bool InputManager::handleErrorMethodPage() {
    clearScreen();
    displayPageHeader("Error Measurement Method");
    cout << "Select the error measurement method:" << endl;
    cout << ANSI_BLUE << "1. Variance" << ANSI_RESET << endl;
    cout << ANSI_BLUE << "2. Mean Absolute Deviation (MAD)" << ANSI_RESET << endl;
    cout << ANSI_BLUE << "3. Max Pixel Difference" << ANSI_RESET << endl;
    cout << ANSI_BLUE << "4. Entropy" << ANSI_RESET << endl;
    cout << ANSI_BLUE << "5. Structural Similarity Index (SSIM) [Bonus]" << ANSI_RESET << endl;
    
    while (true) {
        string input = getStringInput(" ");
        
        if (input.empty()) {
            cout << ANSI_RED << "Error: Selection cannot be empty. Please choose a method (1-5)." << ANSI_RESET << endl;
            continue;
        }
        
        string lower = input;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "b") {
            return false;
        }
        if (lower == "clear") {
            clearScreen();
            displayPageHeader("Error Measurement Method");
            cout << "Select the error measurement method:" << endl;
            cout << ANSI_BLUE << "1. Variance" << ANSI_RESET << endl;
            cout << ANSI_BLUE << "2. Mean Absolute Deviation (MAD)" << ANSI_RESET << endl;
            cout << ANSI_BLUE << "3. Max Pixel Difference" << ANSI_RESET << endl;
            cout << ANSI_BLUE << "4. Entropy" << ANSI_RESET << endl;
            cout << ANSI_BLUE << "5. Structural Similarity Index (SSIM) [Bonus]" << ANSI_RESET << endl;
            continue;
        }
        
        try {
            int method = stoi(input);
            if (method < 1 || method > 5) {
                cout << ANSI_RED << "Please enter a value between 1 and 5." << ANSI_RESET << endl;
                continue;
            }
            params.errorMethod = static_cast<ErrorMethod>(method);
            return true;
        } catch (const exception&) {
            cout << ANSI_RED << "Invalid input. Please enter a number." << ANSI_RESET << endl;
        }
    }
}

// handle input threshold per error method
bool InputManager::handleThresholdPage() {
    clearScreen();
    displayPageHeader("Threshold Value");
    cout << "Enter the threshold value for error." << endl;
    cout << "This value determines when a block should be subdivided." << endl;
    
    double min = 0.0;
    double max = 255.0;
    double defaultValue = 50.0;
    string rangeDescription;
    
    switch (params.errorMethod) {
        case ErrorMethod::VARIANCE:
            min = 0.0;
            max = 16256.25; // (127.5)²
            defaultValue = 500.0;
            rangeDescription = "For Variance method (valid range: 0.0-16256.25)\n"
                              "* Low threshold (high detail): 50-500\n"
                              "* Medium threshold: 500-2000\n"
                              "* High threshold (less detail): 2000-5000++";
            break;
        case ErrorMethod::MEAN_ABSOLUTE_DEVIATION:
            min = 0.0;
            max = 127.5; // Maximum possible MAD
            defaultValue = 15.0;
            rangeDescription = "For MAD method (valid range: 0.0-127.5)\n"
                              "* Low threshold (high detail): 5-15\n"
                              "* Medium threshold: 15-30\n"
                              "* High threshold (less detail): 30-50++";
            break;
        case ErrorMethod::MAX_PIXEL_DIFFERENCE:
            min = 0.0;
            max = 255.0; // Maximum pixel difference
            defaultValue = 30.0;
            rangeDescription = "For Max Pixel Difference (valid range: 0.0-255.0)\n"
                              "* Low threshold (high detail): 10-30\n"
                              "* Medium threshold: 30-60\n"
                              "* High threshold (less detail): 60-100++";
            break;
        case ErrorMethod::ENTROPY:
            min = 0.0;
            max = 8.0; // Maximum entropy for 8-bit channels
            defaultValue = 1.0;
            rangeDescription = "For Entropy method (valid range: 0.0-8.0)\n"
                              "* Low threshold (high detail): 0.1-1.0\n"
                              "* Medium threshold: 1.0-2.0\n"
                              "* High threshold (less detail): 2.0-4.0++";
            break;
        case ErrorMethod::STRUCTURAL_SIMILARITY:
            min = 0.0;
            max = 1.0; // SSIM error range
            defaultValue = 0.05;
            rangeDescription = "For SSIM method (valid range: 0.0-1.0)\n"
                              "* Low threshold (high detail): 0.01-0.05\n"
                              "* Medium threshold: 0.05-0.15\n"
                              "* High threshold (less detail): 0.15-0.3++";
            break;
    }
    
    cout << ANSI_BOLD << ANSI_RED << "\nVALID THRESHOLD RANGE: " << min << " to " << max << ANSI_RESET << endl;
    cout << rangeDescription << endl;
    cout << "Suggested value: " << defaultValue << endl;
    
    while (true) {
        string input = getStringInput(" ");
        
        if (input.empty()) {
            cout << ANSI_RED << "Error: Threshold value cannot be empty. Please enter a value within the range " 
                 << min << " to " << max << "." << ANSI_RESET << endl;
            continue;
        }
        
        string lower = input;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "b") {
            return false;
        }
        if (lower == "clear") {
            clearScreen();
            displayPageHeader("Threshold Value");
            cout << "Enter the threshold value for error." << endl;
            cout << "This value determines when a block should be subdivided." << endl;
            cout << ANSI_BOLD << ANSI_RED << "\nVALID THRESHOLD RANGE: " << min << " to " << max << ANSI_RESET << endl;
            cout << rangeDescription << endl;
            cout << "Suggested value: " << defaultValue << endl;
            continue;
        }

        try {
            double threshold = stod(input);
            if (threshold < min || threshold > max) {
                cout << ANSI_RED << "ERROR: The value " << threshold << " is outside the valid range of " 
                     << min << " to " << max << " for the selected error method." << ANSI_RESET << endl;
                cout << "Please enter a value within the valid range." << endl;
                continue;
            }
            params.threshold = threshold;
            return true;
        } catch (const exception&) {
            cout << ANSI_RED << "Invalid input. Please enter a number." << ANSI_RESET << endl;
        }
    }
}

// handle input minimum block size sesuai dengan dimensi gambar
bool InputManager::handleMinBlockSizePage() {
    clearScreen();
    displayPageHeader("Minimum Block Size");
    cout << "Enter the minimum block size in square pixels (area, not length)" << endl;
    cout << "This is the smallest area (width x height) that will be used during compression." << endl;
    
    int minValue = 1;
    int maxValue = 65536; // 256×256 default
    
    // cari max area dari gambar (buat maksimum min block size)
    if (!params.inputImagePath.empty() && fs::exists(params.inputImagePath)) {
        try {
            cv::Mat image = cv::imread(params.inputImagePath, cv::IMREAD_COLOR);
            if (!image.empty()) {
                int imageWidth = image.cols;
                int imageHeight = image.rows;
                maxValue = imageWidth * imageHeight;
            }
        } catch (...) {
        }
    }
    
    cout << ANSI_GREEN << "\nAllowed range: " << minValue << "-" << maxValue 
         << " square pixels" << ANSI_RESET << endl;
    
    cout << "\nRecommended values (only applies if the dimensions are relatively big):" << endl;
    cout << "* 4 (2x2) - High detail" << endl;
    cout << "* 16 (4x4) - Good detail" << endl;
    cout << "* 64 (8x8) - Medium detail" << endl;
    cout << "* 256 (16x16++) - Low detail" << endl;
    
    while (true) {
        string input = getStringInput(" ");
        
        if (input.empty()) {
            cout << ANSI_RED << "Error: Minimum block size cannot be empty. Please enter a positive integer value." << ANSI_RESET << endl;
            continue;
        }
        
        string lower = input;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "b") {
            return false;
        }
        if (lower == "clear") {
            return handleMinBlockSizePage();
        }
        
        try {
            int blockSize = stoi(input);
            if (blockSize < minValue) {
                cout << ANSI_RED << "Error: Block size must be at least " << minValue << " square pixel." << ANSI_RESET << endl;
                continue;
            }
            
            if (blockSize > maxValue) {
                cout << ANSI_RED << "Error: Block size cannot exceed " << maxValue 
                     << " square pixels (the image area)." << ANSI_RESET << endl;
                continue;
            }
            
            params.minBlockSize = blockSize;
            return true;
        } catch (const exception&) {
            cout << ANSI_RED << "Invalid input. Please enter a number." << ANSI_RESET << endl;
        }
    }
}

// handle input target compression percentage (bonus)
bool InputManager::handleTargetCompressionPage() {
    clearScreen();
    displayPageHeader("Target Compression Percentage");
    cout << "Enter the target compression percentage (0.0-1.0)" << endl;
    cout << "This is a bonus feature that automatically adjusts the threshold." << endl;
    cout << "Enter 0 to disable automatic threshold adjustment." << endl;
    
    while (true) {
        string input = getStringInput(" ");
        
        if (input.empty()) {
            cout << ANSI_RED << "Error: Target compression value cannot be empty. Please enter a value between 0.0 and 1.0." << ANSI_RESET << endl;
            continue;
        }
        
        string lower = input;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "b") {
            return false;
        }
        if (lower == "clear") {
            clearScreen();
            displayPageHeader("Target Compression Percentage");
            cout << "Enter the target compression percentage (0.0-1.0)" << endl;
            cout << "This is a bonus feature that automatically adjusts the threshold." << endl;
            cout << "Enter 0 to disable automatic threshold adjustment." << endl;
            continue;
        }
        
        try {
            double percentage = stod(input);
            if (percentage < 0.0 || percentage > 1.0) {
                cout << ANSI_RED << "Please enter a value between 0.0 and 1.0." << ANSI_RESET << endl;
                continue;
            }
            params.targetCompressionPercentage = percentage;
            return true;
        } catch (const exception&) {
            cout << ANSI_RED << "Invalid input. Please enter a number." << ANSI_RESET << endl;
        }
    }
}

// handle output pathing
bool InputManager::handleOutputImagePathPage() {
    clearScreen();
    displayPageHeader("Output Image Path");
    
    fs::path inputPath(params.inputImagePath);
    string inputExt = inputPath.extension().string();
    transform(inputExt.begin(), inputExt.end(), inputExt.begin(), ::tolower);
    
    cout << "Enter the output path for the compressed image" << endl;
    cout << ANSI_BOLD << ANSI_RED << "NOTE: The output file MUST use the same extension as the input file (" 
         << inputExt << ")" << ANSI_RESET << endl;
    
    while (true) {
        string path = getStringInput(" ");
        
        if (path.empty()) {
            cout << ANSI_RED << "Error: Output path cannot be empty. Please specify a valid file path with the " 
                 << inputExt << " extension." << ANSI_RESET << endl;
            continue;
        }
        
        string lower = path;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "b") {
            return false;
        }
        if (lower == "clear") {
            clearScreen();
            displayPageHeader("Output Image Path");
            cout << "Enter the output path for the compressed image" << endl;
            cout << ANSI_BOLD << ANSI_RED << "NOTE: The output file MUST use the same extension as the input file (" 
                 << inputExt << ")" << ANSI_RESET << endl;
            continue;
        }
        
        fs::path outputPath = path;
        fs::path parentPath = outputPath.parent_path();
        if (!parentPath.empty() && !fs::exists(parentPath)) {
            cout << ANSI_RED << "Error: Output directory does not exist. Please enter a valid path." << ANSI_RESET << endl;
            continue;
        }
        
        string providedExt = "";
        size_t extPos = path.rfind('.');
        if (extPos != string::npos) {
            providedExt = path.substr(extPos);
            transform(providedExt.begin(), providedExt.end(), providedExt.begin(), ::tolower);
            
            if (providedExt != inputExt) {
                cout << ANSI_RED << "ERROR: Invalid file extension: " << providedExt << endl;
                cout << "The output file extension must match the input file extension: " << inputExt << ANSI_RESET << endl;
                cout << "Please try again with the correct extension." << endl;
                continue;
            } 
            
            params.outputImagePath = path;
        } else {
            cout << ANSI_RED << "ERROR: No file extension provided." << endl;
            cout << "The output file must have the same extension as the input file: " << inputExt << ANSI_RESET << endl;
            cout << "Please try again with the correct extension." << endl;
            continue;
        }
        
        cout << ANSI_GREEN << "Output path set to: " << params.outputImagePath << ANSI_RESET << endl;
        return true;
    }
}

// handle gif output path (bonus)
bool InputManager::handleGifOutputPathPage() {
    clearScreen();
    displayPageHeader("GIF Output Path (Bonus)");
    cout << "Do you want to generate a GIF showing the compression process? (y/n)" << endl;
    
    while (true) {
        string input = getStringInput(" ");
        
        if (input.empty()) {
            cout << ANSI_RED << "Error: Selection cannot be empty. Please enter 'y' or 'n'." << ANSI_RESET << endl;
            continue;
        }
        
        string lower = input;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "b") {
            return false;
        }
        if (lower == "clear") {
            clearScreen();
            displayPageHeader("GIF Output Path (Bonus)");
            cout << "Do you want to generate a GIF showing the compression process? (y/n)" << endl;
            continue;
        }
        
        if (lower == "y" || lower == "yes") {
            params.generateGif = true;
            break;
        } else if (lower == "n" || lower == "no") {
            params.generateGif = false;
            return true;
        } else {
            cout << ANSI_RED << "Please enter 'y' or 'n'." << ANSI_RESET << endl;
        }
    }
    
    clearScreen();
    displayPageHeader("GIF Output Path (Bonus)");
    cout << "Enter the output path for the GIF file:" << endl;
    
    while (true) {
        string path = getStringInput(" ");
        
        if (path.empty()) {
            cout << ANSI_RED << "Error: GIF output path cannot be empty. Please specify a valid file path with .gif extension." << ANSI_RESET << endl;
            continue;
        }
        
        string lower = path;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "b") {
            return false;
        }
        if (lower == "clear") {
            clearScreen();
            displayPageHeader("GIF Output Path (Bonus)");
            cout << "Enter the output path for the GIF file:" << endl;
            continue;
        }
        
        fs::path outputPath = path;
        fs::path parentPath = outputPath.parent_path();
        if (!parentPath.empty() && !fs::exists(parentPath)) {
            cout << ANSI_RED << "Error: Output directory does not exist. Please enter a valid path." << ANSI_RESET << endl;
            continue;
        }
        string extension = outputPath.extension().string();
        transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
        if (extension != ".gif") {
            cout << ANSI_RED << "Error: Output must be .gif. Please enter a valid path." << ANSI_RESET << endl;
            continue;
        }
        
        params.gifOutputPath = path;
        return true;
    }
}

// handle konfirmasi sebelum proses kompresi
bool InputManager::handleConfirmationPage() {
    clearScreen();
    displayPageHeader("Confirmation");
    
    cout << "Please review your settings:" << endl;
    cout << ANSI_BOLD << "Input Image Path: " << ANSI_RESET << params.inputImagePath << endl;
    
    cout << ANSI_BOLD << "Error Method: " << ANSI_RESET;
    switch (params.errorMethod) {
        case ErrorMethod::VARIANCE:
            cout << "Variance";
            break;
        case ErrorMethod::MEAN_ABSOLUTE_DEVIATION:
            cout << "Mean Absolute Deviation (MAD)";
            break;
        case ErrorMethod::MAX_PIXEL_DIFFERENCE:
            cout << "Max Pixel Difference";
            break;
        case ErrorMethod::ENTROPY:
            cout << "Entropy";
            break;
        case ErrorMethod::STRUCTURAL_SIMILARITY:
            cout << "Structural Similarity Index (SSIM)";
            break;
    }
    cout << endl;
    
    cout << ANSI_BOLD << "Threshold: " << ANSI_RESET << params.threshold << endl;
    cout << ANSI_BOLD << "Minimum Block Size: " << ANSI_RESET << params.minBlockSize << endl;
    
    if (params.targetCompressionPercentage > 0.0) {
        cout << ANSI_BOLD << "Target Compression: " << ANSI_RESET << (params.targetCompressionPercentage * 100) << "%" << endl;
    } else {
        cout << ANSI_BOLD << "Target Compression: " << ANSI_RESET << "Disabled" << endl;
    }
    
    cout << ANSI_BOLD << "Output Image Path: " << ANSI_RESET << params.outputImagePath << endl;
    
    if (params.generateGif) {
        cout << ANSI_BOLD << "GIF Output Path: " << ANSI_RESET << params.gifOutputPath << endl;
    } else {
        cout << ANSI_BOLD << "GIF Generation: " << ANSI_RESET << "Disabled" << endl;
    }
    
    cout << endl << "Press ENTER to confirm and start compression, or type 'B' to go back." << endl;
    
    while (true) {
        string input = getStringInput(" ");
        
        if (input.empty()) {
            return true;
        }
        
        string lower = input;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "b") {
            return false;
        }
        if (lower == "clear") {
            clearScreen();
            displayPageHeader("Confirmation");
            cout << "Please review your settings:" << endl;
            cout << ANSI_BOLD << "Input Image Path: " << ANSI_RESET << params.inputImagePath << endl;
            
            cout << ANSI_BOLD << "Error Method: " << ANSI_RESET;
            switch (params.errorMethod) {
                case ErrorMethod::VARIANCE:
                    cout << "Variance";
                    break;
                case ErrorMethod::MEAN_ABSOLUTE_DEVIATION:
                    cout << "Mean Absolute Deviation (MAD)";
                    break;
                case ErrorMethod::MAX_PIXEL_DIFFERENCE:
                    cout << "Max Pixel Difference";
                    break;
                case ErrorMethod::ENTROPY:
                    cout << "Entropy";
                    break;
                case ErrorMethod::STRUCTURAL_SIMILARITY:
                    cout << "Structural Similarity Index (SSIM)";
                    break;
            }
            cout << endl;
            cout << ANSI_BOLD << "Threshold: " << ANSI_RESET << params.threshold << endl;
            cout << ANSI_BOLD << "Minimum Block Size: " << ANSI_RESET << params.minBlockSize << endl;
            
            if (params.targetCompressionPercentage > 0.0) {
                cout << ANSI_BOLD << "Target Compression: " << ANSI_RESET << (params.targetCompressionPercentage * 100) << "%" << endl;
            } else {
                cout << ANSI_BOLD << "Target Compression: " << ANSI_RESET << "Disabled" << endl;
            }
            
            cout << ANSI_BOLD << "Output Image Path: " << ANSI_RESET << params.outputImagePath << endl;
            
            if (params.generateGif) {
                cout << ANSI_BOLD << "GIF Output Path: " << ANSI_RESET << params.gifOutputPath << endl;
            } else {
                cout << ANSI_BOLD << "GIF Generation: " << ANSI_RESET << "Disabled" << endl;
            }
            
            cout << endl << "Press ENTER to confirm and start compression, or type 'B' to go back." << endl;
            continue;
        }
        return true;
    }
}

bool InputManager::fileExists(const string& filePath) {
    // checker buat mastiin file ada atau enggak

    return fs::exists(filePath);
}

bool InputManager::isImageFile(const string& filePath) {
    // checker buat mastiin file image atau bukan

    fs::path path(filePath);
    string ext = path.extension().string();
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return (ext == ".jpg" || ext == ".jpeg" || ext == ".png"); // .jpg, .jpeg, .png
}


bool InputManager::validatePath(const string& path, bool mustExist) {
    // checker untuk absolute path

    if (path.empty()) {
        return false;
    }
    
    if (mustExist && !fileExists(path)) {
        return false;
    }
    
    return true;
}

bool InputManager::validateErrorMethod(int method) {
    // checker untuk error method (1-5)

    return method >= 1 && method <= 5;
}

bool InputManager::validateThreshold(double threshold) {
    // checker untuk threshold (relatif dengan error method)

    return threshold >= 0.0;
}

bool InputManager::validateMinBlockSize(int size) {
    // checker untuk minimum block size (relatif dengan dimensi gambar)

    return size >= 1;
}

bool InputManager::validateTargetCompression(double percentage) {
    // checker untuk target compression percentage (0%-100%)

    return percentage >= 0.0 && percentage <= 1.0;
}

string InputManager::getStringInput(const string& prompt) {
    // string input handler

    if (!prompt.empty()) {
        cout << prompt;
    }
    cout << "> ";
    string input;
    getline(cin, input);

    if (input == "exit") {
        exit(0);
    }

    if (input == "help") {
        cout << "Available commands: clear - clear the input line, help - show available commands, b - go back, exit - exit program" << endl;
        return getStringInput(prompt);
    }
    
    return input;
}

int InputManager::getIntInput(const string& prompt, int min, int max) {
    // integer input handler

    if (!prompt.empty()) {
        cout << prompt;
    }
    cout << "> ";
    string input;
    getline(cin, input);

    if (input.empty()) {
        cout << ANSI_RED << "Error: Input cannot be empty. Please enter a number between " << min << " and " << max << "." << ANSI_RESET << endl;
        return getIntInput(prompt, min, max);
    }

    if (input == "exit") {
        exit(0);
    }

    if (input == "help") {
        cout << "Available commands: clear - clear the input line, help - show available commands, b - go back, exit - exit program" << endl;
        return getIntInput(prompt, min, max);
    }
    
    if (input == "B" || input == "b") {
        return -1;
    }
    
    try {
        int value = stoi(input);
        if (value < min || value > max) {
            cout << "Please enter a value between " << min << " and " << max << "." << endl;
            return getIntInput("", min, max);
        }
        return value;
    } catch (const exception&) {
        cout << "Invalid input. Please enter a number." << endl;
        return getIntInput("", min, max);
    }
}

double InputManager::getDoubleInput(const string& prompt, double min, double max) {
    // double input handler

    if (!prompt.empty()) {
        cout << prompt;
    }
    cout << "> ";
    string input;
    getline(cin, input);

    if (input.empty()) {
        cout << ANSI_RED << "Error: Input cannot be empty. Please enter a number between " << min << " and " << max << "." << ANSI_RESET << endl;
        return getDoubleInput(prompt, min, max);
    }

    if (input == "exit") {
        exit(0);
    }
    
    if (input == "help") {
        cout << "Available commands: clear - clear the input line, help - show available commands, b - go back, exit - exit program" << endl;
        return getDoubleInput(prompt, min, max);
    }
    
    if (input == "B" || input == "b") {
        return -1.0;
    }
    
    try {
        double value = stod(input);
        if (value < min || value > max) {
            cout << "Please enter a value between " << min << " and " << max << "." << endl;
            return getDoubleInput("", min, max);
        }
        return value;
    } catch (const exception&) {
        cout << "Invalid input. Please enter a number." << endl;
        return getDoubleInput("", min, max);
    }
}

bool InputManager::getBoolInput(const string& prompt) {
    // boolean input handler
    
    if (!prompt.empty()) {
        cout << prompt;
    }
    
    while (true) {
        cout << "> ";
        string input;
        getline(cin, input);
        
        if (input.empty()) {
            cout << ANSI_RED << "Error: Input cannot be empty. Please enter 'y' for Yes or 'n' for No." << ANSI_RESET << endl;
            continue;
        }
        
        if (input == "exit") {
            exit(0);
        }

        if (input == "help") {
            cout << "Available commands: clear - clear the input line, help - show available commands, b - go back, exit - exit program" << endl;
            continue;
        }
        
        if (input == "B" || input == "b") {
            return false;
        }
        
        if (input == "Y" || input == "y" || input == "yes" || input == "Yes") {
            return true;
        } else if (input == "N" || input == "n" || input == "no" || input == "No") {
            return false;
        } else {
            cout << "Please enter 'y' for Yes or 'n' for No: ";
        }
    }
}