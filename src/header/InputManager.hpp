#ifndef _INPUT_MANAGER_H
#define _INPUT_MANAGER_H

#include <string>
#include <iostream>
#include <limits>
#include "CompressionParams.hpp"

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


using namespace std;


class InputManager {
public:
    InputManager();
    ~InputManager();
    
    CompressionParams getCompressionParams();
    
private:
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
    
    // Input handling methods
    void displayPageHeader(const string& title);
    void displayPageFooter();
    void clearScreen();
    
    // Page display methods
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
    
    // Input gathering methods
    string getStringInput(const string& prompt);
    int getIntInput(const string& prompt, int min, int max);
    double getDoubleInput(const string& prompt, double min, double max);
    bool getBoolInput(const string& prompt);
};

#endif
