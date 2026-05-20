# android-arm64-toolchain.cmake

# Let CMake know this is an Android build
set(CMAKE_SYSTEM_NAME Android)

# Set the path to your NDK root
set(ANDROID_NDK /home/pranjal_bajpai/android-ndk-r27c)

# Required ABI and API level
set(ANDROID_ABI arm64-v8a)
set(ANDROID_PLATFORM android-24)

# Optional: choose the STL
set(ANDROID_STL c++_static)
