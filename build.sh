#!/bin/bash
set -e

# Tentukan direktori root project
PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"

# Buat folder bin/linux jika belum ada
BUILD_DIR="$PROJECT_ROOT/bin/linux"
mkdir -p "$BUILD_DIR"

# Pindah ke folder build
cd "$BUILD_DIR"

# Jalankan CMake dengan menunjuk ke direktori root (yang berisi CMakeLists.txt)
cmake "$PROJECT_ROOT"

# Lakukan build secara paralel (sesuaikan jumlah core dengan nproc)
make -j$(nproc)

echo "Build complete. Executable berada di $PROJECT_ROOT/bin/quadtree_compression"
