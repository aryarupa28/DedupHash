#include "generate_dataset.h"
#include "macros.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/stat.h>
#include <dirent.h>
#include <sstream>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>
#include <sys/types.h>
#include <sys/mman.h>
#include <errno.h>

const int PAGE_SIZE_4KB = 4096;
const int PAGE_SIZE_16KB = 16384;
const int PAGE_SIZE_2MB = 2097152;

void ensure_directory_exists(const std::string& path) {
    struct stat st;
    if (stat(path.c_str(), &st) != 0) {
        mkdir(path.c_str(), 0755);
    }
}

std::vector<int> list_running_processes() {
    std::vector<int> pids;
    DIR* dir = opendir("/proc");
    if (!dir) {
        ALWAYS_LOG_ERROR("Failed to open /proc directory: " << strerror(errno));
        return pids;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (isdigit(entry->d_name[0])) {
            pids.push_back(std::atoi(entry->d_name));
        }
    }

    closedir(dir);
    return pids;
}

int detect_system_page_size() {
    long page_size = sysconf(_SC_PAGESIZE);
    if (page_size <= 0) {
        ALWAYS_LOG_WARNING("Failed to detect page size, using default 4KB");
        return PAGE_SIZE_4KB;
    }
    return static_cast<int>(page_size);
}

void capture_memory_snapshot(int pid, std::ofstream& outfile) {
    std::string maps_path = "/proc/" + std::to_string(pid) + "/maps";
    std::ifstream maps_file(maps_path);
    if (!maps_file.is_open()) {
        WARNING_LOG("Could not open memory map for PID: " << pid);
        return;
    }

    std::string mem_path = "/proc/" + std::to_string(pid) + "/mem";
    int mem_fd = open(mem_path.c_str(), O_RDONLY);
    if (mem_fd == -1) {
        ERROR_LOG("Failed to open mem file for PID " << pid << ": " << strerror(errno));
        maps_file.close();
        return;
    }

    DEBUG_PROCESS("Processing PID: " << pid);
    std::string line;
    while (std::getline(maps_file, line)) {
        std::istringstream iss(line);
        std::string address_range, perms, offset, dev, inode, pathname;
        iss >> address_range >> perms >> offset >> dev >> inode;
        std::getline(iss, pathname);  // Path may be empty

        if (perms.find('r') == std::string::npos) continue;

        size_t dash_pos = address_range.find('-');
        unsigned long start = std::stoul(address_range.substr(0, dash_pos), nullptr, 16);
        unsigned long end = std::stoul(address_range.substr(dash_pos + 1), nullptr, 16);
        size_t region_size = end - start;

        if (region_size == 0 || region_size > 100 * 1024 * 1024) continue;

        int page_size = detect_system_page_size();

        if (pathname.find("hugepage") != std::string::npos) {
            page_size = PAGE_SIZE_2MB;
        }

        for (unsigned long addr = start; addr < end; addr += page_size) {
            size_t chunk_size = std::min((unsigned long)page_size, end - addr);

            if (lseek(mem_fd, addr, SEEK_SET) == -1) continue;

            std::vector<char> buffer(chunk_size);
            ssize_t bytes_read = read(mem_fd, buffer.data(), chunk_size);

            if (bytes_read > 0) {
                outfile.write(buffer.data(), bytes_read);
                // Performance-sensitive debug that disappears in benchmark builds
                PERF_DEBUG_LOG("Read " << bytes_read << " bytes from address 0x" 
                              << std::hex << addr << std::dec);
            }
        }
    }

    close(mem_fd);
    maps_file.close();
}

void generate_dataset(const std::string& file_name) {
    ensure_directory_exists("../data");
    std::string full_path = "../data/" + file_name;

    std::ofstream outfile(full_path, std::ios::binary);
    if (!outfile.is_open()) {
        ALWAYS_LOG_ERROR("Failed to open output file: " << full_path);
        return;
    }

    std::vector<int> pids = list_running_processes();
    DEBUG_LOG("Found " << pids.size() << " running processes");

    size_t total_bytes = 0;
    for (int pid : pids) {
        size_t initial_pos = outfile.tellp();
        capture_memory_snapshot(pid, outfile);
        size_t final_pos = outfile.tellp();
        size_t bytes_written = final_pos - initial_pos;

        total_bytes += bytes_written;

        if (bytes_written > 0) {
            DEBUG_MEMORY("Wrote " << (bytes_written / 1024) << " KB from PID " << pid);
        }
    }

    outfile.close();

    if (total_bytes == 0) {
        ALWAYS_LOG_ERROR("No memory data collected");
        std::ofstream fallback(full_path);
        fallback.close();
    } else {
        SUCCESS_LOG("Dataset generated: " << full_path << " (" << total_bytes << " bytes)");
    }
}