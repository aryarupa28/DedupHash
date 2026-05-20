# Non-Crypto Hashing R&D: Duplicate Content Detection for Storage Memory

## 🧠 Project Overview

This project is a deep-dive R&D initiative focused on **identifying duplicate data blocks** in memory and storage using **fast, non-cryptographic hash functions**. It combines OS-level page analysis with hashing and benchmarking to simulate **real-world high-performance deduplication workflows**, especially those similar to **Kernel Samepage Merging (KSM)** systems.

The implementation supports:
- Native execution on **x86_64**
- Cross-compilation for **Android/ARM64** using the NDK
- **SIMD acceleration** on ARM using **NEON intrinsics**

---

## 🚀 Key Features

- ⚡ **Non-Cryptographic Hashing**: Integrates both `GXHash` and `XXHash`
- 📄 **Page-Aligned Chunking**: Uses dynamic system page size for chunking
- 📊 **Performance Benchmarking**: Built-in Google Benchmark support
- 🗜️ **Compression Benchmarking**: Optional `lzbench` integration
- 💻 **Multi-Architecture**: Runs on x86, ARM64, ARM-NEON
- 🧬 **SIMD Optimization**: Uses NEON intrinsics for GXHash on ARM
- 📁 **/proc Filesystem Parsing**: Leverages `/proc/self/maps`, `/meminfo`, `/statm`
- 🐳 **Full Docker Containerization**: Includes builds for both x86 & Android ARM64
- 🧪 **Unit Tests & Benchmarks**: Cleanly organized in `tests/` folder

---

## 🧩 System Architecture

### 🔹 Data Collection
- Reads system page stats using:
  - `/proc/self/statm`
  - `/proc/self/maps`
  - `/proc/meminfo`

### 🔹 Chunking Engine
- Dynamically determines system page size:
```cpp
long page_size = sysconf(_SC_PAGESIZE);
```
- Splits memory regions into **page-aligned chunks**
- Enables precise and platform-independent deduplication

### 🔹 Hashing Layer
- Applies **GXHash** or **XXHash** per chunk
- Allows dynamic switching of hashing algorithm
- Supports block-wise and multithreaded operation

### 🔹 Optional Compression
- Uses [`lzbench`](https://github.com/inikep/lzbench) to benchmark
  - Compressibility of raw vs deduplicated memory blocks

---

## ⚖️ Hashing Algorithm Comparison

### ✅ Summary

| Block Size | XXHash Time (ns) | GXHash Time (ns) | Preferred |
|------------|------------------|------------------|-----------|
| 64 B       | 101.4            | 76.3             | ✅ GXHash |
| 512 B      | 154.2            | 120.5            | ✅ GXHash |
| 4 KB       | 290.1            | 282.4            | ✅ GXHash |
| 2 MB       | 4100.2           | 5312.7           | ✅ XXHash |

- 🧠 **GXHash** is better for small to medium-size blocks  
- 🚀 **XXHash** outperforms for bulk data / high throughput cases

---

## 📈 Google Benchmark Results

Measured on `Intel i7` and `ARM Cortex-A53` (Android), below are detailed microbenchmark results using the [Google Benchmark](https://github.com/google/benchmark) suite:

### GXHash Benchmarks

| Test                   | Avg Time (ns) |
|------------------------|---------------|
| `BM_GXHash_64B`        | 76.30         |
| `BM_GXHash_512B`       | 120.50        |
| `BM_GXHash_4KB`        | 282.40        |
| `BM_GXHash_2MB`        | 5312700.00    |

### XXHash Benchmarks

| Test                   | Avg Time (ns) |
|------------------------|---------------|
| `BM_XXHash_64B`        | 101.40        |
| `BM_XXHash_512B`       | 154.20        |
| `BM_XXHash_4KB`        | 290.10        |
| `BM_XXHash_2MB`        | 4100200.00    |

---

## 📦 GitHub Release (Binary Builds)

Precompiled binaries are available for:

- ✅ Native Linux (x86_64)
- ✅ Android ARM64 (NDK-based)

🔗 Download from the **[GitHub Releases](https://github.ecodesamsung.com/SRIB-PRISM/SRM_24OD24SRM_Non-Crypto_Hashing_R_and_D_Duplicate_Content_Detection_for_storage_memory/releases/tag/v1.0.0)**  


---

### 🔧 Running Native (Linux x86_64)
```bash
cd build-x86
./hashing_system
./tests
```

### 📱 Running on Android (via ADB)
```bash
adb push build-arm64/hashing_system /data/local/tmp/
adb shell
cd /data/local/tmp
chmod +x hashing_system
./hashing_system
```

---

## 🐳 Docker Build Instructions

To regenerate both x86 and ARM64 builds using Docker:

```bash
# Build Docker image
docker build -t noncrypto-hashing .

# Run and extract builds
docker run -dit --name temp_hashing_container noncrypto-hashing
./extract_docker_builds.sh
```

✅ Outputs will be saved to:
- `host_builds/build-x86/`
- `host_builds/build-arm64/`

---

## 📂 Folder Structure

```
├── build-x86/           # Native Linux binaries
├── build-arm64/         # Android binaries (NDK-built)
├── src/                 # Core logic and hashing modules
├── tests/               # Unit tests and benchmarks
├── deps/                # Embedded dependencies (xxhash, gxhash, gtest, etc.)
├── Dockerfile           # Containerized build
├── extract_docker_builds.sh
└── CMakeLists.txt
```

---
