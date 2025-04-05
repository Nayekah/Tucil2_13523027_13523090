// include header file
#include "GifGenerator.hpp"


GifGenerator::GifGenerator(): imageWidth(0), imageHeight(0) {
    // cons
}

GifGenerator::~GifGenerator() {
    // dtor
}

bool GifGenerator::generateGif(const QuadTree& quadTree, const string& outputPath) {
    if (!quadTree.getRoot()) {
        return false;
    }
    
    auto root = quadTree.getRoot();
    imageWidth = root->getWidth();
    imageHeight = root->getHeight();
    
    try {
        string tempDir;

        // simpan gambar perframe di temporary folder
        #ifdef _WIN32 // windows
            tempDir = "temp_quadtree_frames";
            system(("if exist " + tempDir + " rmdir /s /q " + tempDir).c_str());
            system(("mkdir " + tempDir).c_str());
        #else // linux
            tempDir = "/tmp/quadtree_frames";
            system(("rm -rf " + tempDir + " && mkdir -p " + tempDir).c_str());
        #endif
        
        frames.clear();
        
        int maxFrames = 80;
        int depthLimit = quadTree.getDepth();
        
        for (int depth = 0; depth <= depthLimit; ++depth) {
            // buat frame sesuai dengan depth
            Frame frame;
            frame.width = imageWidth;
            frame.height = imageHeight;
            
            frame.pixels.resize(imageHeight, vector<Pixel>(imageWidth, Pixel(255, 255, 255)));
            
            // Fill frame sesuai sama node relatif terhadap depth
            renderTreeAtDepth(frame, quadTree.getRoot(), depth);
            frames.push_back(frame);
        }
        
        // Save frames
        for (size_t i = 0; i < frames.size(); ++i) {
            cv::Mat cvFrame(frames[i].height, frames[i].width, CV_8UC3);
            
            for (int y = 0; y < frames[i].height; ++y) {
                for (int x = 0; x < frames[i].width; ++x) {
                    cv::Vec3b& color = cvFrame.at<cv::Vec3b>(y, x);
                    color[0] = frames[i].pixels[y][x].b;
                    color[1] = frames[i].pixels[y][x].g;
                    color[2] = frames[i].pixels[y][x].r;
                }
            }
            
            stringstream ss;
            ss << tempDir << "/frame_" << setw(5) << setfill('0') << i << ".png";
            cv::imwrite(ss.str(), cvFrame);
        }
        
        string cmd;
        #ifdef _WIN32 // windows
            string imCheck = "where magick >nul 2>&1";
            int imResult = system(imCheck.c_str());
            if (imResult == 0) {
                cmd = "magick -delay 50 -loop 0 " + tempDir + "\\frame_*.png \"" + outputPath + "\"";
            } else {
                string ffmpegCheck = "where ffmpeg >nul 2>&1";
                int ffmpegResult = system(ffmpegCheck.c_str());
                if (ffmpegResult == 0) {
                    cmd = "ffmpeg -y -framerate 2 -i " + tempDir + "\\frame_%05d.png -vf \"scale=trunc(iw/2)*2:trunc(ih/2)*2\" \"" + outputPath + "\"";
                } else {
                    cerr << "Neither ImageMagick nor FFmpeg found on Windows. Please install one of them to create GIFs." << endl;
                    cerr << "ImageMagick: https://imagemagick.org/script/download.php" << endl;
                    cerr << "FFmpeg: https://ffmpeg.org/download.html" << endl;
                    
                    system(("rmdir /s /q " + tempDir).c_str());
                    return false;
                }
            }
        #else // linux
            string imCheck = "which convert >/dev/null 2>&1";
            int imResult = system(imCheck.c_str());
            if (imResult == 0) {
                cmd = "convert -delay 50 -loop 0 " + tempDir + "/frame_*.png \"" + outputPath + "\"";
            } else {
                string ffmpegCheck = "which ffmpeg >/dev/null 2>&1";
                int ffmpegResult = system(ffmpegCheck.c_str());
                if (ffmpegResult == 0) {
                    cmd = "ffmpeg -y -framerate 2 -i " + tempDir + "/frame_%05d.png -vf \"scale=trunc(iw/2)*2:trunc(ih/2)*2\" \"" + outputPath + "\"";
                } else {
                    cerr << "Neither ImageMagick nor FFmpeg found on Linux. Please install one of them to create GIFs." << endl;
                    cerr << "ImageMagick: sudo apt-get install imagemagick" << endl;
                    cerr << "FFmpeg: sudo apt-get install ffmpeg" << endl;
                    
                    system(("rm -rf " + tempDir).c_str());
                    return false;
                }
            }
        #endif
        
        int result = system(cmd.c_str());
        
        if (result != 0) {
            cerr << "Failed to create GIF. Error code: " << result << endl;
            
            // Cleaning up the mess, hehe
            #ifdef _WIN32 // windows
                system(("rmdir /s /q " + tempDir).c_str());
            #else // linux
                system(("rm -rf " + tempDir).c_str());
            #endif
            
            return false;
        }
        
        #ifdef _WIN32
            system(("rmdir /s /q " + tempDir).c_str());
        #else
            system(("rm -rf " + tempDir).c_str());
        #endif
        
        cout << "GIF successfully created at: " << outputPath << endl;
        
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

    // continue recursing if not
    else if (currentDepth < targetDepth) {
        for (const auto& child : node->getChildren()) {
            renderTreeAtDepth(frame, child, targetDepth, currentDepth + 1);
        }
    }
}

void GifGenerator::renderTreeAtDepth(Frame& frame, const shared_ptr<QuadTreeNode>& node, int targetDepth) {
    renderTreeAtDepth(frame, node, targetDepth, 0);
}

void GifGenerator::renderPartialDepth(Frame& frame, const shared_ptr<QuadTreeNode>& node, int baseDepth, int nextDepth, float progress) {
    if (!node) return;
    
    if (node->isLeaf()) {
        drawNode(frame, node);
        return;
    }
    
    int nodeDepth = getNodeDepth(node);
    
    // Draw all nodes up to baseDepth
    if (nodeDepth <= baseDepth) {
        if (nodeDepth == baseDepth) {
            // At the boundary depth, decide if this node should be subdivided
            if (progress < 0.5f) {
                drawNode(frame, node);
            } else {
                // show children
                for (const auto& child : node->getChildren()) {
                    drawNode(frame, child);
                }
            }
        } else {
            // if below base depth, recurse to children
            for (const auto& child : node->getChildren()) {
                renderPartialDepth(frame, child, baseDepth, nextDepth, progress);
            }
        }
    } else {
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
    
    for (int j = y; j < y + height; ++j) {
        for (int i = x; i < x + width; ++i) {
            if (j >= 0 && i >= 0 && j < frame.height && i < frame.width) {
                frame.pixels[j][i] = color;
            }
        }
    }
}