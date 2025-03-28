#include "GifGenerator.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <string>

// Include OpenCV for frame generation
#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

GifGenerator::GifGenerator()
    : imageWidth(0), imageHeight(0) {
}

GifGenerator::~GifGenerator() {
}

bool GifGenerator::generateGif(const QuadTree& quadTree, const string& outputPath) {
    if (!quadTree.getRoot()) {
        return false;
    }
    
    auto root = quadTree.getRoot();
    imageWidth = root->getWidth();
    imageHeight = root->getHeight();
    
    try {
        // Create a temp directory for frames
        string tempDir = "/tmp/quadtree_frames";
        system(("mkdir -p " + tempDir).c_str());
        
        // Clear any existing frames
        frames.clear();
        
        // Generate frames incrementally (limit to avoid too many frames)
        int maxFrames = 50; // Adjust based on your needs
        int depthLimit = quadTree.getDepth();
        
        for (int depth = 0; depth <= depthLimit; ++depth) {
            // Create a frame for this depth
            Frame frame;
            frame.width = imageWidth;
            frame.height = imageHeight;
            
            // Initialize with white background
            frame.pixels.resize(imageHeight, vector<Pixel>(imageWidth, Pixel(255, 255, 255)));
            
            // Fill in the frame with nodes up to this depth
            renderTreeAtDepth(frame, quadTree.getRoot(), depth);
            
            // Add to our collection
            frames.push_back(frame);
            
            // If we have subdivided areas, add some intermediate frames
            /*
            if (depth < depthLimit && frames.size() < maxFrames) {
                // Add a few more frames showing intermediate divisions
                int subFrames = min(3, maxFrames - (int)frames.size());
                for (int i = 1; i <= subFrames; ++i) {
                    Frame subFrame = frame; // Start with the current frame
                    int nextDepth = depth + 1;
                    renderPartialDepth(subFrame, quadTree.getRoot(), depth, nextDepth, i / (float)(subFrames + 1));
                    frames.push_back(subFrame);
                }
            }
            */
        }
        
        // Save frames as individual images
        for (size_t i = 0; i < frames.size(); ++i) {
            cv::Mat cvFrame(frames[i].height, frames[i].width, CV_8UC3);
            
            for (int y = 0; y < frames[i].height; ++y) {
                for (int x = 0; x < frames[i].width; ++x) {
                    cv::Vec3b& color = cvFrame.at<cv::Vec3b>(y, x);
                    color[0] = frames[i].pixels[y][x].b; // BGR format for OpenCV
                    color[1] = frames[i].pixels[y][x].g;
                    color[2] = frames[i].pixels[y][x].r;
                }
            }
            
            stringstream ss;
            ss << tempDir << "/frame_" << setw(5) << setfill('0') << i << ".png";
            cv::imwrite(ss.str(), cvFrame);
        }
        
        // Use ImageMagick to combine frames into a GIF
        string cmd = "convert -delay 50 -loop 0 " + tempDir + "/frame_*.png " + outputPath;
        int result = system(cmd.c_str());
        
        if (result != 0) {
            cerr << "Failed to create GIF with ImageMagick. Make sure it's installed." << endl;
            cerr << "Try installing with: sudo apt-get install imagemagick" << endl;
            return false;
        }
        
        // Clean up temporary files
        system(("rm -rf " + tempDir).c_str());
        
        return true;
    } catch (const exception& e) {
        cerr << "Error generating GIF: " << e.what() << endl;
        return false;
    }
}

void GifGenerator::renderTreeAtDepth(Frame& frame, const shared_ptr<QuadTreeNode>& node, int targetDepth, int currentDepth) {
    if (!node) return;
    
    // If we've reached a leaf node or the target depth, draw this node
    if (node->isLeaf() || currentDepth == targetDepth) {
        drawNode(frame, node);
    } 
    // If we haven't reached the target depth yet, continue recursing
    else if (currentDepth < targetDepth) {
        for (const auto& child : node->getChildren()) {
            renderTreeAtDepth(frame, child, targetDepth, currentDepth + 1);
        }
    }
}

void GifGenerator::renderTreeAtDepth(Frame& frame, const shared_ptr<QuadTreeNode>& node, int targetDepth) {
    renderTreeAtDepth(frame, node, targetDepth, 0);
}

void GifGenerator::renderPartialDepth(Frame& frame, const shared_ptr<QuadTreeNode>& node, 
                                     int baseDepth, int nextDepth, float progress) {
    if (!node) return;
    
    if (node->isLeaf()) {
        drawNode(frame, node);
        return;
    }
    
    int nodeDepth = getNodeDepth(node);
    
    // Draw all nodes up to baseDepth
    if (nodeDepth <= baseDepth) {
        if (nodeDepth == baseDepth) {
            // At the boundary depth, decide if this node should be subdivided based on progress
            if (progress < 0.5f) {
                drawNode(frame, node);
            } else {
                // Start showing children
                for (const auto& child : node->getChildren()) {
                    drawNode(frame, child);
                }
            }
        } else {
            // Below base depth, recurse to children
            for (const auto& child : node->getChildren()) {
                renderPartialDepth(frame, child, baseDepth, nextDepth, progress);
            }
        }
    } else {
        // For deeper nodes, just draw them
        drawNode(frame, node);
    }
}

int GifGenerator::getNodeDepth(const shared_ptr<QuadTreeNode>& node) {
    if (!node) return 0;
    if (node->isLeaf()) return 0;
    
    int maxChildDepth = 0;
    for (const auto& child : node->getChildren()) {
        maxChildDepth = max(maxChildDepth, getNodeDepth(child));
    }
    
    return 1 + maxChildDepth;
}

void GifGenerator::drawNode(Frame& frame, const shared_ptr<QuadTreeNode>& node) {
    if (!node) return;
    
    Pixel color = node->getColor();
    int x = node->getX();
    int y = node->getY();
    int width = node->getWidth();
    int height = node->getHeight();
    
    // Fill the node area with its color
    for (int j = y; j < y + height; ++j) {
        for (int i = x; i < x + width; ++i) {
            if (j >= 0 && i >= 0 && j < frame.height && i < frame.width) {
                frame.pixels[j][i] = color;
            }
        }
    }
    
    /*
    // Draw borders
    Pixel borderColor(0, 0, 0); // Black border
    
    // Draw horizontal borders
    for (int i = x; i < x + width && i < frame.width; ++i) {
        if (i >= 0) {
            if (y >= 0 && y < frame.height) frame.pixels[y][i] = borderColor;
            if (y + height - 1 >= 0 && y + height - 1 < frame.height) frame.pixels[y + height - 1][i] = borderColor;
        }
    }
    
    // Draw vertical borders
    for (int j = y; j < y + height && j < frame.height; ++j) {
        if (j >= 0) {
            if (x >= 0 && x < frame.width) frame.pixels[j][x] = borderColor;
            if (x + width - 1 >= 0 && x + width - 1 < frame.width) frame.pixels[j][x + width - 1] = borderColor;
        }
    }
    */
}
