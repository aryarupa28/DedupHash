FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive

# Install base dependencies
RUN apt-get update && apt-get install -y \
    build-essential cmake git g++ wget unzip \
    libgtest-dev python3 make ninja-build \
    && rm -rf /var/lib/apt/lists/*

# Install lzbench from GitHub
RUN git clone https://github.com/inikep/lzbench.git /tmp/lzbench && \
    cd /tmp/lzbench && make && \
    cp lzbench /usr/local/bin && \
    rm -rf /tmp/lzbench

# Set working directory
WORKDIR /app

# Copy entire project source (including embedded dependencies)
COPY . .

# === Native x86_64 Linux build ===
RUN mkdir -p build && \
    cd build && \
    cmake .. && \
    make -j$(nproc)

# === Android ARM64 build ===
ENV ANDROID_NDK_VERSION=r25c
RUN wget https://dl.google.com/android/repository/android-ndk-${ANDROID_NDK_VERSION}-linux.zip && \
    unzip android-ndk-${ANDROID_NDK_VERSION}-linux.zip && \
    mv android-ndk-${ANDROID_NDK_VERSION} /opt/android-ndk && \
    rm android-ndk-${ANDROID_NDK_VERSION}-linux.zip

ENV ANDROID_NDK=/opt/android-ndk

RUN mkdir -p build-android-arm64 && \
    cd build-android-arm64 && \
    cmake .. \
        -DCMAKE_TOOLCHAIN_FILE=android-arm64-toolchain.cmake \
        -DANDROID_ABI=arm64-v8a \
        -DANDROID_PLATFORM=android-21 \
        -DCMAKE_BUILD_TYPE=Release && \
    make -j$(nproc)

# === Entry point for native execution ===
CMD ["./build/hashing_system"]
