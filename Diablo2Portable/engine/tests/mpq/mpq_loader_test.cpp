#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "utils/mpq_loader.h"
#include <fstream>
#include <filesystem>

using namespace d2portable::utils;
using namespace testing;

class MPQLoaderTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Create test directory
        test_dir = std::filesystem::temp_directory_path() / "d2portable_test";
        std::filesystem::create_directories(test_dir);
        
        // Create a mock MPQ file for testing
        test_mpq_path = test_dir / "test.mpq";
        createMockMPQFile(test_mpq_path);
    }

    void TearDown() override {
        // Clean up test files
        std::filesystem::remove_all(test_dir);
    }

    void createMockMPQFile(const std::filesystem::path& path) {
        // Create a minimal MPQ header for testing
        // Real MPQ format: 'MPQ\x1A' signature followed by header
        std::ofstream file(path, std::ios::binary);
        const char signature[] = {'M', 'P', 'Q', 0x1A};
        file.write(signature, 4);
        
        // Write mock header data (32 bytes)
        uint32_t header_size = 32;
        uint32_t archive_size = 1024;
        uint16_t format_version = 0;
        uint16_t block_size = 3; // 4096 bytes (512 * 2^3)
        uint32_t hash_table_offset = 64;
        uint32_t block_table_offset = 128;
        uint32_t hash_table_entries = 16;
        uint32_t block_table_entries = 8;
        
        file.write(reinterpret_cast<const char*>(&header_size), 4);
        file.write(reinterpret_cast<const char*>(&archive_size), 4);
        file.write(reinterpret_cast<const char*>(&format_version), 2);
        file.write(reinterpret_cast<const char*>(&block_size), 2);
        file.write(reinterpret_cast<const char*>(&hash_table_offset), 4);
        file.write(reinterpret_cast<const char*>(&block_table_offset), 4);
        file.write(reinterpret_cast<const char*>(&hash_table_entries), 4);
        file.write(reinterpret_cast<const char*>(&block_table_entries), 4);
    }

    std::filesystem::path test_dir;
    std::filesystem::path test_mpq_path;
    MPQLoader loader;
};

// Test: Opening a valid MPQ file
TEST_F(MPQLoaderTest, OpenValidMPQFile) {
    EXPECT_TRUE(loader.open(test_mpq_path.string()));
    EXPECT_TRUE(loader.isOpen());
}

// Test: Opening a non-existent file
TEST_F(MPQLoaderTest, OpenNonExistentFile) {
    EXPECT_FALSE(loader.open("/path/to/nonexistent.mpq"));
    EXPECT_FALSE(loader.isOpen());
    EXPECT_FALSE(loader.getLastError().empty());
}

// Test: Opening an invalid file (not MPQ format)
TEST_F(MPQLoaderTest, OpenInvalidFile) {
    auto invalid_file = test_dir / "invalid.mpq";
    std::ofstream file(invalid_file);
    file << "This is not an MPQ file";
    file.close();
    
    EXPECT_FALSE(loader.open(invalid_file.string()));
    EXPECT_FALSE(loader.isOpen());
    EXPECT_THAT(loader.getLastError(), HasSubstr("Invalid MPQ"));
}

// Test: Closing an MPQ file
TEST_F(MPQLoaderTest, CloseMPQFile) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    EXPECT_TRUE(loader.isOpen());
    
    loader.close();
    EXPECT_FALSE(loader.isOpen());
}

// Test: Listing files in an MPQ archive
TEST_F(MPQLoaderTest, ListFilesInArchive) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    
    auto files = loader.listFiles();
    // For our mock MPQ, we expect it to be empty initially
    EXPECT_TRUE(files.empty());
}

// Test: Checking if a file exists in the archive
TEST_F(MPQLoaderTest, CheckFileExists) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    
    // Mock MPQ doesn't contain any files yet
    EXPECT_FALSE(loader.hasFile("data\\global\\excel\\armor.txt"));
    EXPECT_FALSE(loader.hasFile("nonexistent.txt"));
}

// Test: Extracting a file from the archive
TEST_F(MPQLoaderTest, ExtractFileFromArchive) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    
    std::vector<uint8_t> output;
    // Should fail for non-existent file
    EXPECT_FALSE(loader.extractFile("data\\global\\excel\\armor.txt", output));
    EXPECT_TRUE(output.empty());
}

// Test: Getting file information
TEST_F(MPQLoaderTest, GetFileInformation) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    
    auto info = loader.getFileInfo("data\\global\\excel\\armor.txt");
    EXPECT_FALSE(info.has_value());
}

// Test: Multiple operations without opening
TEST_F(MPQLoaderTest, OperationsWithoutOpening) {
    EXPECT_FALSE(loader.isOpen());
    EXPECT_TRUE(loader.listFiles().empty());
    EXPECT_FALSE(loader.hasFile("any_file.txt"));
    
    std::vector<uint8_t> output;
    EXPECT_FALSE(loader.extractFile("any_file.txt", output));
    
    EXPECT_FALSE(loader.getFileInfo("any_file.txt").has_value());
}

// Test: Opening multiple files sequentially
TEST_F(MPQLoaderTest, OpenMultipleFilesSequentially) {
    // Create a second mock MPQ
    auto second_mpq = test_dir / "second.mpq";
    createMockMPQFile(second_mpq);
    
    // Open first file
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    EXPECT_TRUE(loader.isOpen());
    
    // Open second file (should close first)
    ASSERT_TRUE(loader.open(second_mpq.string()));
    EXPECT_TRUE(loader.isOpen());
}

// Test: Memory safety with large file names
TEST_F(MPQLoaderTest, LargeFileNameHandling) {
    ASSERT_TRUE(loader.open(test_mpq_path.string()));
    
    // Create a very long filename
    std::string long_filename(1024, 'a');
    EXPECT_FALSE(loader.hasFile(long_filename));
    
    std::vector<uint8_t> output;
    EXPECT_FALSE(loader.extractFile(long_filename, output));
}