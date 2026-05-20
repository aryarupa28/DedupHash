#!/bin/bash

# Build Docker image
docker build -t noncrypto-hashing .

# Run container (detached)
docker run -dit --name temp_hashing_container noncrypto-hashing

# Wait a bit (you can also inspect logs here if needed)
echo "⏳ Waiting for build to complete..."

# Extract builds
mkdir -p host_builds
docker cp temp_hashing_container:/app/build ./host_builds/build-x86
docker cp temp_hashing_container:/app/build-android-arm64 ./host_builds/build-arm64

# Cleanup
docker rm -f temp_hashing_container

echo "✅ Build outputs extracted to ./host_builds/"
