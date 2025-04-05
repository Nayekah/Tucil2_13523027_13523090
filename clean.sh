#!/bin/bash
set -e

# Dapatkan direktori root project
PROJECT_ROOT="$(cd "$(dirname "$0")" && pwd)"

# Lokasi folder build dan executable
BUILD_DIR="$PROJECT_ROOT/bin/linux"
EXECUTABLE="$PROJECT_ROOT/bin/quadtree_compression"

echo "Membersihkan folder build: $BUILD_DIR"
if [ -d "$BUILD_DIR" ]; then
    rm -rf "$BUILD_DIR"
    echo "Folder build berhasil dihapus."
else
    echo "Folder build tidak ditemukan."
fi

echo "Membersihkan executable: $EXECUTABLE"
if [ -f "$EXECUTABLE" ]; then
    rm -f "$EXECUTABLE"
    echo "Executable berhasil dihapus."
else
    echo "Executable tidak ditemukan."
fi

echo "Proses clean selesai."