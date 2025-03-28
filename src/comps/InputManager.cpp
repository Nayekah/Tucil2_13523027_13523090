#include "InputManager.hpp"
#include <fstream>
#include <filesystem>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <iostream>

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

namespace fs = std::filesystem;

InputManager::InputManager() : currentPage(InputPage::INPUT_IMAGE_PATH) {
}

InputManager::~InputManager() {
}
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
                continue; // Already at first page, stay there
            }
            currentPage = static_cast<InputPage>(static_cast<int>(currentPage) - 1);
        }
    }
}

void InputManager::clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

void addVerticalSpacing(int lines = 10) {
    for (int i = 0; i < lines; ++i) {
        cout << endl;
    }
}

void InputManager::displayPageHeader(const string& title) {
    clearScreen();
    // ASCII Art Header untuk "Quadtree Compressor"
    cout <<
" ██████╗ ██╗   ██╗ █████╗ ██████╗ ████████╗██████╗ ███████╗███████╗     ██████╗ ██████╗ ███╗   ███╗██████╗ ██████╗ ███████╗███████╗███████╗ ██████╗ ██████╗ \n"
"██╔═══██╗██║   ██║██╔══██╗██╔══██╗╚══██╔══╝██╔══██╗██╔════╝██╔════╝    ██╔════╝██╔═══██╗████╗ ████║██╔══██╗██╔══██╗██╔════╝██╔════╝██╔════╝██╔═══██╗██╔══██╗\n"
"██║   ██║██║   ██║███████║██║  ██║   ██║   ██████╔╝█████╗  █████╗      ██║     ██║   ██║██╔████╔██║██████╔╝██████╔╝█████╗  ███████╗███████╗██║   ██║██████╔╝\n"
"██║▄▄ ██║██║   ██║██╔══██║██║  ██║   ██║   ██╔══██╗██╔══╝  ██╔══╝      ██║     ██║   ██║██║╚██╔╝██║██╔═══╝ ██╔══██╗██╔══╝  ╚════██║╚════██║██║   ██║██╔══██╗\n"
"╚██████╔╝╚██████╔╝██║  ██║██████╔╝   ██║   ██║  ██║███████╗███████╗    ╚██████╗╚██████╔╝██║ ╚═╝ ██║██║     ██║  ██║███████╗███████║███████║╚██████╔╝██║  ██║\n"
" ╚══▀▀═╝  ╚═════╝ ╚═╝  ╚═╝╚═════╝    ╚═╝   ╚═╝  ╚═╝╚══════╝╚══════╝     ╚═════╝ ╚═════╝ ╚═╝     ╚═╝╚═╝     ╚═╝  ╚═╝╚══════╝╚══════╝╚══════╝ ╚═════╝ ╚═╝  ╚═╝\n"
"\n";
    
    // Header Title dan info halaman dengan warna
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

bool InputManager::handleInputImagePathPage() {
    // Tampilkan header dengan warna dan ASCII art
    clearScreen();
    displayPageHeader("Input Image Path");
    cout << "Please enter the absolute path to the image to compress:\n\n";

    while (true) {
        string path = getStringInput(" ");
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
            cout << ANSI_RED << "Error: File is not a supported image type. Use .jpg, .png, or .bmp." << ANSI_RESET << endl;
            continue;
        }

        params.inputImagePath = path;
        return true;
    }
}

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

bool InputManager::handleThresholdPage() {
    clearScreen();
    displayPageHeader("Threshold Value");
    cout << "Enter the threshold value for error." << endl;
    cout << "This value determines when a block should be subdivided." << endl;
    
    double min = 0.0;
    double max = 255.0;
    switch (params.errorMethod) {
        case ErrorMethod::VARIANCE:
            cout << "For Variance method, suggested range: 0-255" << endl;
            break;
        case ErrorMethod::MEAN_ABSOLUTE_DEVIATION:
            cout << "For MAD method, suggested range: 0-50" << endl;
            break;
        case ErrorMethod::MAX_PIXEL_DIFFERENCE:
            cout << "For Max Pixel Difference, suggested range: 0-100" << endl;
            break;
        case ErrorMethod::ENTROPY:
            cout << "For Entropy method, suggested range: 0-8" << endl;
            max = 8.0;
            break;
        case ErrorMethod::STRUCTURAL_SIMILARITY:
            cout << "For SSIM method, suggested range: 0.0-1.0 (lower values mean higher quality)" << endl;
            max = 1.0;
            break;
    }
    
    while (true) {
        string input = getStringInput(" ");
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
            switch (params.errorMethod) {
                case ErrorMethod::VARIANCE:
                    cout << "For Variance method, suggested range: 0-255" << endl;
                    break;
                case ErrorMethod::MEAN_ABSOLUTE_DEVIATION:
                    cout << "For MAD method, suggested range: 0-50" << endl;
                    break;
                case ErrorMethod::MAX_PIXEL_DIFFERENCE:
                    cout << "For Max Pixel Difference, suggested range: 0-100" << endl;
                    break;
                case ErrorMethod::ENTROPY:
                    cout << "For Entropy method, suggested range: 0-8" << endl;
                    break;
                case ErrorMethod::STRUCTURAL_SIMILARITY:
                    cout << "For SSIM method, suggested range: 0.0-1.0 (lower values mean higher quality)" << endl;
                    break;
            }
            continue;
        }
        
        try {
            double threshold = stod(input);
            if (threshold < min || threshold > max) {
                cout << ANSI_RED << "Please enter a value between " << min << " and " << max << "." << ANSI_RESET << endl;
                continue;
            }
            params.threshold = threshold;
            return true;
        } catch (const exception&) {
            cout << ANSI_RED << "Invalid input. Please enter a number." << ANSI_RESET << endl;
        }
    }
}

bool InputManager::handleMinBlockSizePage() {
    clearScreen();
    displayPageHeader("Minimum Block Size");
    cout << "Enter the minimum block size (power of 2 recommended: 1, 2, 4, 8, etc.)" << endl;
    cout << "This is the smallest block size that will be used during compression." << endl;
    
    while (true) {
        string input = getStringInput(" ");
        string lower = input;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "b") {
            return false;
        }
        if (lower == "clear") {
            clearScreen();
            displayPageHeader("Minimum Block Size");
            cout << "Enter the minimum block size (power of 2 recommended: 1, 2, 4, 8, etc.)" << endl;
            cout << "This is the smallest block size that will be used during compression." << endl;
            continue;
        }
        
        try {
            int blockSize = stoi(input);
            if (blockSize < 1 || blockSize > 64) {
                cout << ANSI_RED << "Please enter a value between 1 and 64." << ANSI_RESET << endl;
                continue;
            }
            params.minBlockSize = blockSize;
            return true;
        } catch (const exception&) {
            cout << ANSI_RED << "Invalid input. Please enter a number." << ANSI_RESET << endl;
        }
    }
}

bool InputManager::handleTargetCompressionPage() {
    clearScreen();
    displayPageHeader("Target Compression Percentage");
    cout << "Enter the target compression percentage (0.0-1.0)" << endl;
    cout << "This is a bonus feature that automatically adjusts the threshold." << endl;
    cout << "Enter 0 to disable automatic threshold adjustment." << endl;
    
    while (true) {
        string input = getStringInput(" ");
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

bool InputManager::handleOutputImagePathPage() {
    clearScreen();
    displayPageHeader("Output Image Path");
    cout << "Enter the output path for the compressed image:" << endl;
    
    while (true) {
        string path = getStringInput(" ");
        string lower = path;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "b") {
            return false;
        }
        if (lower == "clear") {
            clearScreen();
            displayPageHeader("Output Image Path");
            cout << "Enter the output path for the compressed image:" << endl;
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
        if (extension != ".jpg" && extension != ".png" && extension != ".bmp") {
            cout << ANSI_RED << "Error: Output must be .jpg, .png, or .bmp. Please enter a valid path." << ANSI_RESET << endl;
            continue;
        }
        
        params.outputImagePath = path;
        return true;
    }
}

bool InputManager::handleGifOutputPathPage() {
    clearScreen();
    displayPageHeader("GIF Output Path (Bonus)");
    cout << "Do you want to generate a GIF showing the compression process? (y/n)" << endl;
    
    while (true) {
        string input = getStringInput(" ");
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
        // Input selain "B" atau "clear" dianggap sebagai konfirmasi
        return true;
    }
}

bool InputManager::fileExists(const string& filePath) {
    return fs::exists(filePath);
}

bool InputManager::isImageFile(const string& filePath) {
    fs::path path(filePath);
    string ext = path.extension().string();
    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    return (ext == ".jpg" || ext == ".jpeg" || ext == ".png" || ext == ".bmp");
}

bool InputManager::validatePath(const string& path, bool mustExist) {
    if (path.empty()) {
        return false;
    }
    
    if (mustExist && !fileExists(path)) {
        return false;
    }
    
    return true;
}

bool InputManager::validateErrorMethod(int method) {
    return method >= 1 && method <= 5;
}

bool InputManager::validateThreshold(double threshold) {
    return threshold >= 0.0;
}

bool InputManager::validateMinBlockSize(int size) {
    return size >= 1;
}

bool InputManager::validateTargetCompression(double percentage) {
    return percentage >= 0.0 && percentage <= 1.0;
}

string InputManager::getStringInput(const string& prompt) {
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
    if (!prompt.empty()) {
        cout << prompt;
    }
    
    while (true) {
        cout << "> ";
        string input;
        getline(cin, input);
        
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