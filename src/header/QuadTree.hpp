#ifndef _QUADTREE_HPP
#define _QUADTREE_HPP

#include <memory>
#include <vector>
#include "Pixel.hpp"

using namespace std;


class QuadTreeNode {
public:
    QuadTreeNode(int x, int y, int width, int height);
    ~QuadTreeNode() = default; // Default destructor is sufficient with shared_ptr
    
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
    int x, y;              // Top-left corner coordinates
    int width, height;     // Dimensions of this node
    Pixel color;           // Average color for this node
    vector<shared_ptr<QuadTreeNode>> children; // Child nodes (up to 4)
};

class QuadTree {
public:
    QuadTree();
    ~QuadTree() = default; // Default destructor is sufficient with shared_ptr
    
    // Getters
    shared_ptr<QuadTreeNode> getRoot() const { return root; }
    int getDepth() const { return depth; }
    int getNodeCount() const { return nodeCount; }
    
    // Setters
    void setRoot(shared_ptr<QuadTreeNode> newRoot);
    
    // Tree analysis
    void calculateDepthAndNodeCount();
    
private:
    shared_ptr<QuadTreeNode> root;
    int depth;
    int nodeCount;
    
    // Helper methods for tree analysis
    int calculateDepth(shared_ptr<QuadTreeNode> node);
    int countNodes(shared_ptr<QuadTreeNode> node);
};

#endif