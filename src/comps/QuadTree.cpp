// include header file
#include "QuadTree.hpp"


// Implementation
QuadTreeNode::QuadTreeNode(int x, int y, int width, int height): x(x), y(y), width(width), height(height), color(0, 0, 0) {
    
    // Make sure that the image has valid dimensions
    assert(width > 0 && "Width must be greater than 0");
    assert(height > 0 && "Height must be greater than 0");
}

void QuadTreeNode::addChild(shared_ptr<QuadTreeNode> child) {

    // Child building
    if (children.size() < 4 && child) {
        children.push_back(child);
    }
}

QuadTree::QuadTree(): root(nullptr), depth(0), nodeCount(0) {
    // default constructor
}

void QuadTree::setRoot(shared_ptr<QuadTreeNode> newRoot) {
    root = newRoot;
    calculateDepthAndNodeCount();
}

void QuadTree::calculateDepthAndNodeCount() {
    // Method to calculate the depth and node count of the QuadTree

    if (root) {
        depth = calculateDepth(root);
        nodeCount = countNodes(root);
    } else {
        depth = 0;
        nodeCount = 0;
    }
}

int QuadTree::calculateDepth(shared_ptr<QuadTreeNode> node) {
    // Method to calculate the depth of the QuadTree

    if (!node) {
        return 0;
    }
    
    // if the node is a leaf
    if (node->isLeaf()) {
        return 1;
    }
    
    int maxChildDepth = 0;
    for (const auto& child : node->getChildren()) {
        if (child) {
            maxChildDepth = max(maxChildDepth, calculateDepth(child));
        }
    }
    
    return 1 + maxChildDepth;
}

int QuadTree::countNodes(shared_ptr<QuadTreeNode> node) {
    // Method to count the number of nodes in the QuadTree

    if (!node) {
        return 0;
    }
    
    int count = 1;
    
    for (const auto& child : node->getChildren()) {
        if (child) {
            count += countNodes(child);
        }
    }
    
    return count;
}