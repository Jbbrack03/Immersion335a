#include "utils/mpq_loader.h"
#include <fstream>
#include <cstring>
#include <filesystem>
#include <unordered_map>

namespace d2portable {
namespace utils {

// MPQ file header structure
struct MPQHeader {
    char signature[4];      // 'MPQ\x1A'
    uint32_t header_size;
    uint32_t archive_size;
    uint16_t format_version;
    uint16_t block_size;
    uint32_t hash_table_offset;
    uint32_t block_table_offset;
    uint32_t hash_table_entries;
    uint32_t block_table_entries;
};

// Private implementation class
class MPQLoader::Impl {
public:
    Impl() : is_open(false) {}
    
    bool is_open;
    std::string filepath;
    std::string last_error;
    std::ifstream file;
    MPQHeader header;
    std::unordered_map<std::string, MPQFileInfo> file_map;
    
    bool validateHeader() {
        return header.signature[0] == 'M' && 
               header.signature[1] == 'P' && 
               header.signature[2] == 'Q' && 
               header.signature[3] == 0x1A;
    }
};

// Constructor
MPQLoader::MPQLoader() : pImpl(std::make_unique<Impl>()) {}

// Destructor - required for pImpl idiom
MPQLoader::~MPQLoader() = default;

bool MPQLoader::open(const std::string& filepath) {
    // Close any previously opened file
    if (pImpl->is_open) {
        close();
    }
    
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        pImpl->last_error = "File not found: " + filepath;
        return false;
    }
    
    // Open the file
    pImpl->file.open(filepath, std::ios::binary);
    if (!pImpl->file.is_open()) {
        pImpl->last_error = "Failed to open file: " + filepath;
        return false;
    }
    
    // Read and validate header
    pImpl->file.read(reinterpret_cast<char*>(&pImpl->header), sizeof(MPQHeader));
    if (!pImpl->file.good() || pImpl->file.gcount() < static_cast<std::streamsize>(sizeof(MPQHeader))) {
        pImpl->file.close();
        // Check if we read enough to validate the header
        if (pImpl->file.gcount() >= 4 && !pImpl->validateHeader()) {
            pImpl->last_error = "Invalid MPQ file format";
        } else {
            pImpl->last_error = "Failed to read MPQ header";
        }
        return false;
    }
    
    // Validate MPQ signature
    if (!pImpl->validateHeader()) {
        pImpl->file.close();
        pImpl->last_error = "Invalid MPQ file format";
        return false;
    }
    
    // Mark as open
    pImpl->is_open = true;
    pImpl->filepath = filepath;
    pImpl->last_error.clear();
    
    // TODO: Read hash table and block table to populate file_map
    
    return true;
}

void MPQLoader::close() {
    if (pImpl->file.is_open()) {
        pImpl->file.close();
    }
    pImpl->is_open = false;
    pImpl->filepath.clear();
    pImpl->file_map.clear();
}

bool MPQLoader::isOpen() const {
    return pImpl->is_open;
}

std::vector<MPQFileInfo> MPQLoader::listFiles() const {
    // TODO: Implement file listing
    return {};
}

bool MPQLoader::hasFile(const std::string& filename) const {
    // TODO: Implement file checking
    return false;
}

bool MPQLoader::extractFile(const std::string& filename, std::vector<uint8_t>& output) {
    // TODO: Implement file extraction
    output.clear();
    return false;
}

std::optional<MPQFileInfo> MPQLoader::getFileInfo(const std::string& filename) const {
    // TODO: Implement file info retrieval
    return std::nullopt;
}

std::string MPQLoader::getLastError() const {
    return pImpl->last_error;
}

} // namespace utils
} // namespace d2portable