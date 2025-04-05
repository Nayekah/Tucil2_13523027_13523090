#ifndef _INPUT_MANAGER_HPP
#define _INPUT_MANAGER_HPP

// include lib files
#include <string>
#include <limits>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iostream>
#include <algorithm>
#include <filesystem>

// include header files
#include "CompressionParams.hpp"

// include opencv
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


// namespace
using namespace std;
namespace fs = std::filesystem;


// Input class
class InputManager {
    public:
        InputManager(); // Ctor
        ~InputManager(); // Dtor
        
        CompressionParams getCompressionParams(); //getter
        
    private:
        // Data for compression
        enum class InputPage {
            INPUT_IMAGE_PATH = 0,
            ERROR_METHOD = 1,
            THRESHOLD = 2,
            MIN_BLOCK_SIZE = 3,
            TARGET_COMPRESSION = 4,
            OUTPUT_IMAGE_PATH = 5,
            GIF_OUTPUT_PATH = 6,
            CONFIRMATION = 7
        };
        
        CompressionParams params;
        InputPage currentPage;
        
        // Page display methods
        void displayPageHeader(const string& title);
        void displayPageFooter();
        void clearScreen();
        
        // Input Handler methods
        bool handleInputImagePathPage();
        bool handleErrorMethodPage();
        bool handleThresholdPage();
        bool handleMinBlockSizePage();
        bool handleTargetCompressionPage();
        bool handleOutputImagePathPage();
        bool handleGifOutputPathPage();
        bool handleConfirmationPage();
        
        // Validation methods
        bool fileExists(const string& filePath);
        bool isImageFile(const string& filePath);
        bool validatePath(const string& path, bool mustExist);
        bool validateErrorMethod(int method);
        bool validateThreshold(double threshold);
        bool validateMinBlockSize(int size);
        bool validateTargetCompression(double percentage);
        
        // Input getter methods
        string getStringInput(const string& prompt);
        int getIntInput(const string& prompt, int min, int max);
        double getDoubleInput(const string& prompt, double min, double max);
        bool getBoolInput(const string& prompt);
};

#endif