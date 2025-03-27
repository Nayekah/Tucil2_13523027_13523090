#ifndef _GIF_GENERATOR_H
#define _GIF_GENERATOR_H

#include <string>
#include <vector>
#include "QuadTree.hpp"

using namespace std;

class GifGenerator {
public:
    GifGenerator();
    ~GifGenerator();
    
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