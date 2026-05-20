#!/bin/bash

set -e  # Exit on error

echo "📦 Updating package list..."
sudo apt-get update

echo "🔧 Installing essential development tools and libraries..."
sudo apt-get install -y \
    build-essential \
    gcc \
    clang \
    make \
    git \
    cmake \
    linux-tools-common \
    linux-tools-$(uname -r) \
    valgrind \
    libgtest-dev \
    libbenchmark-dev

echo "✅ System-level dependencies installed successfully!"

echo ""
echo "🚀 You can now build the project:"
echo "   mkdir -p build && cd build"
echo "   cmake .."
echo "   make -j$(nproc)"
echo ""
echo "🔬 To run tests:"
echo "   ctest"
