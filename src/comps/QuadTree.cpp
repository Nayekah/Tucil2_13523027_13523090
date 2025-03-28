#include "QuadTree.hpp"
#include <algorithm>
#include <iostream>
#include <cassert>


QuadTreeNode::QuadTreeNode(int x, int y, int width, int height)
    : x(x), y(y), width(width), height(height), color(0, 0, 0) {

    assert(width > 0 && "Width must be positive");
    assert(height > 0 && "Height must be positive");
}

void QuadTreeNode::addChild(shared_ptr<QuadTreeNode> child) {
    if (children.size() < 4 && child) {
        children.push_back(child);
    }
}

QuadTree::QuadTree()
    : root(nullptr), depth(0), nodeCount(0) {
}

void QuadTree::setRoot(shared_ptr<QuadTreeNode> newRoot) {
    root = newRoot;
    calculateDepthAndNodeCount();
}

void QuadTree::calculateDepthAndNodeCount() {
    if (root) {
        depth = calculateDepth(root);
        nodeCount = countNodes(root);
    } else {
        depth = 0;
        nodeCount = 0;
    }
}

int QuadTree::calculateDepth(shared_ptr<QuadTreeNode> node) {
    if (!node) {
        return 0;
    }
    
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
    if (!node) {
        return 0;
    }
    
    int count = 1; // Count this node
    
    for (const auto& child : node->getChildren()) {
        if (child) {
            count += countNodes(child);
        }
    }
    
    return count;
}