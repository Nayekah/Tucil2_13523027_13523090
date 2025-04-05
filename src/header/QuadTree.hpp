#ifndef _QUADTREE_HPP
#define _QUADTREE_HPP


// include lib files
#include <memory>
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>

// include Pixel header file
#include "Pixel.hpp"


// namespace
using namespace std;


// Node
class QuadTreeNode {
    public:
        QuadTreeNode(int x, int y, int width, int height); //Ctor
        ~QuadTreeNode() = default; // Dtor
        
        // Getters
        int getX() const { return x; }
        int getY() const { return y; }
        int getWidth() const { return width; }
        int getHeight() const { return height; }
        bool isLeaf() const { return children.empty(); }
        const Pixel& getColor() const { return color; }
        const vector<shared_ptr<QuadTreeNode>>& getChildren() const { return children; }
        
        // Setters
        void setColor(const Pixel& newColor) { color = newColor; }
        void addChild(shared_ptr<QuadTreeNode> child);
        
    private:
        int x, y;              // Top-left corner
        int width, height;     // Dimensions of the node
        Pixel color;           // color of the node (average color for leaf nodes)
        vector<shared_ptr<QuadTreeNode>> children; // Child nodes
};


// QuadTree
class QuadTree {
    public:
        QuadTree();
        ~QuadTree() = default;
        
        // Getters
        shared_ptr<QuadTreeNode> getRoot() const { return root; }
        int getDepth() const { return depth; }
        int getNodeCount() const { return nodeCount; }
        
        // Setters
        void setRoot(shared_ptr<QuadTreeNode> newRoot);
        
        // Depth and node count calculation
        void calculateDepthAndNodeCount();
        
    private:
        shared_ptr<QuadTreeNode> root;
        int depth;
        int nodeCount;
        
        // Method
        int calculateDepth(shared_ptr<QuadTreeNode> node);
        int countNodes(shared_ptr<QuadTreeNode> node);
};

#endif