#ifndef NIN10KIT_GBFS_H
#define NIN10KIT_GBFS_H
// Types for working on the Gameboy Filesystem (GBFS),
// a format implemented at https://pineight.com/gba/#gbfs

#include <array>
#include <cstdint>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <fstream>

#include "image.hpp"

// A GBFS archive is basically nothing more than a header section defining
// 24-byte long ASCII filenames and offsets.

// First 16 bytes of each archive are a magic value
const std::array<char, 16> GBFS_MAGIC = {'P',  'i',  'n',    'E', 'i', 'g',
                                         'h',  't',  'G',    'B', 'F', 'S',
                                         '\r', '\n', '\032', '\n'};
// Reserved bytes are unused and we'll ensure they're empty (or we may be
// dealing with a new version of the format)
const std::array<char, 8> GBFS_RESERVED = {0};
// The size of a single file directory entry in bytes.
const std::uint8_t GBFS_FILE_ENTRY_SIZE = 32;
const std::uint8_t GBFS_FILENAME_LEN = 24;

// The entry for a single file in the GBFS index.
struct GBFSEntry {
    std::vector<unsigned char> contents;
    std::string name;
};

class GBFS {
   private:
    /// After the magic value there's a uint32 giving the total remaining length
    /// of the archive
    std::uint32_t archive_length;
    /// Followed by a uint16 giving the offset to the file directory in bytes
    std::uint16_t dir_offset;
    /// Followed by the number of files in the directory
    std::uint16_t num_files;
    /// All the entries we've parsed/want to write
    std::vector<GBFSEntry> entries;

    /// This constructor is only used internally, as it performs no sanity
    /// checking.
    GBFS(std::uint32_t archive_length, std::uint16_t dir_offset,
         std::uint16_t num_files, std::vector<GBFSEntry> entries);
    /// Write the frames of the given animation to the archive.
    void WriteAnimationFrames(
        const std::map<std::string, std::vector<Image*>>& name_frames);

   public:
    /// Create an empty archive from scratch.
    GBFS();
    /// Read the archive from a file.
    GBFS ReadArchive(std::istream& stream);
    /// Write the archive to a file.
    void WriteArchive(std::ostream& stream);
    /// Add a given data blob to the archive.
    /// Note that the name must not be longer than
    /// GBFS_FILENAME_LEN bytes.
    void AddFile(const std::string& name, const std::vector<uint8_t>& data);
    /// Write processed graphics data to the archive.
    void WriteGraphics();
};

/// Create a file suitable for writing the archive into.
void InitGBFSFile(std::ofstream& file, const std::string& name);

#endif
