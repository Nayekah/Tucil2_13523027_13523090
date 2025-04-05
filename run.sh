#!/bin/bash

echo "Searching for required dependencies..."

# Try to find OpenCV installation
OPENCV_INSTALLED=false
if pkg-config --exists opencv4; then
    OPENCV_DIR=$(pkg-config --variable=prefix opencv4)
    OPENCV_INSTALLED=true
    echo "Found OpenCV 4 at: $OPENCV_DIR"
elif pkg-config --exists opencv; then
    OPENCV_DIR=$(pkg-config --variable=prefix opencv)
    OPENCV_INSTALLED=true
    echo "Found OpenCV at: $OPENCV_DIR"
else
    # Try common installation locations
    for DIR in /usr/local /usr /opt/opencv; do
        if [ -d "$DIR/include/opencv4" ] || [ -d "$DIR/include/opencv2" ]; then
            OPENCV_DIR=$DIR
            OPENCV_INSTALLED=true
            echo "Found OpenCV at: $OPENCV_DIR"
            break
        fi
    done
fi

if [ "$OPENCV_INSTALLED" = false ]; then
    echo "WARNING: OpenCV installation not found. You may need to set paths manually."
else
    export PKG_CONFIG_PATH="$PKG_CONFIG_PATH:$OPENCV_DIR/lib/pkgconfig"
    export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$OPENCV_DIR/lib"
    echo "Set OpenCV paths"
fi

# Try to find ImageMagick installation
IMAGEMAGICK_INSTALLED=false
if command -v convert &> /dev/null; then
    IMAGEMAGICK_INSTALLED=true
    IMAGEMAGICK_VERSION=$(convert --version | head -n 1)
    echo "Found ImageMagick: $IMAGEMAGICK_VERSION"
elif command -v magick &> /dev/null; then
    IMAGEMAGICK_INSTALLED=true
    IMAGEMAGICK_VERSION=$(magick --version | head -n 1)
    echo "Found ImageMagick: $IMAGEMAGICK_VERSION"
fi

if [ "$IMAGEMAGICK_INSTALLED" = false ]; then
    echo "WARNING: ImageMagick installation not found. GIF generation may not work properly."
    echo "Install with: sudo apt-get install imagemagick"
fi

# Check for ffmpeg as alternative for GIF generation
if command -v ffmpeg &> /dev/null; then
    echo "Found FFmpeg: $(ffmpeg -version | head -n 1)"
else
    echo "NOTE: FFmpeg not found. Install with: sudo apt-get install ffmpeg"
fi

echo "Environment variables set:"
echo "PKG_CONFIG_PATH = $PKG_CONFIG_PATH"
echo "LD_LIBRARY_PATH = $LD_LIBRARY_PATH"

# Move to bin directory (assuming run.sh is in project root)
cd "$(dirname "$0")/bin" || { echo "Error: bin directory not found"; exit 1; }

echo ""
echo "Running QuadTree Image Compression..."

# Process arguments
if [ "$1" = "basic" ]; then
    echo "Mode: Basic"
    ./quadtree_compression basic "${@:2}"
elif [ "$1" = "page" ]; then
    echo "Mode: Interactive Paging"
    ./quadtree_compression page "${@:2}"
elif [ "$1" = "" ]; then
    echo "Mode: Default (Interactive Paging)"
    ./quadtree_compression
else
    echo "Unknown option: $1"
    echo ""
    echo "Usage:"
    echo "  ./run.sh --basic    : Run in Basic Mode"
    echo "  ./run.sh --page     : Run in Interactive Paging Mode"
    echo "  ./run.sh            : Run in Default Interactive Mode"
    exit 1
fi

# Add execution rights to the script by running: chmod +x run.sh
