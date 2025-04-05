#ifndef _BASIC_INPUT__HPP
#define _BASIC_INPUT__HPP

// include lib files
#include <cctype>
#include <string>
#include <iostream>
#include <algorithm>
#include <filesystem>

// include header file
#include "CompressionParams.hpp"


// namespace
using namespace std;

class BasicInputManager {
    public:
        BasicInputManager(); // Ctor
        ~BasicInputManager(); // Dtor
        
        // Get compression parameters
        CompressionParams getCompressionParams();
        
    private:
        // ANSI color definitions
        const std::string ANSI_RESET    = "\033[0m";
        const std::string ANSI_BOLD     = "\033[1m";
        const std::string ANSI_RED      = "\033[31m";
        const std::string ANSI_GREEN    = "\033[32m";
        const std::string ANSI_YELLOW   = "\033[33m";
        const std::string ANSI_BLUE     = "\033[34m";
        const std::string ANSI_MAGENTA  = "\033[35m";
        const std::string ANSI_CYAN     = "\033[36m";
        const std::string ANSI_WHITE    = "\033[37m";
        
        // Helper methods for validation
        bool isImageFile(const std::string& filePath);
        bool fileExists(const std::string& filePath);
};

#endif
