#ifndef _GIF_GENERATOR_HPP
#define _GIF_GENERATOR_HPP


// include lib files
#include <string>
#include <vector>
#include <cstdlib>
#include <iomanip>
#include <sstream>
#include <iostream>

// include header file
#include "QuadTree.hpp"


// Include OpenCV
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>


// namespace
using namespace std;

class GifGenerator {
    public:
        GifGenerator(); // Ctor
        ~GifGenerator(); // Dtor
        
        // Generate GIF from QuadTree
        bool generateGif(const QuadTree& quadTree, const string& outputPath);
        
    private:
        // Internal frame storage
        struct Frame {
            vector<vector<Pixel>> pixels;
            int width;
            int height;
        };
        
        // Helper methods
        void renderTreeAtDepth(Frame& frame, const shared_ptr<QuadTreeNode>& node, int targetDepth);
        void renderTreeAtDepth(Frame& frame, const shared_ptr<QuadTreeNode>& node, int targetDepth, int currentDepth);
        void renderPartialDepth(Frame& frame, const shared_ptr<QuadTreeNode>& node, 
                            int baseDepth, int nextDepth, float progress);
        void drawNode(Frame& frame, const shared_ptr<QuadTreeNode>& node);
        int getNodeDepth(const shared_ptr<QuadTreeNode>& node);
        
        vector<Frame> frames;
        int imageWidth;
        int imageHeight;
};

#endif