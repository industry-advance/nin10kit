#include "gbfs.hpp"

#include <algorithm>
#include <array>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <map>
#include <string>
#include <vector>
#include <fstream>

#include "export_params.hpp"
#include "exportfile.hpp"
#include "gbfsutils.hpp"
#include "image.hpp"
#include "logger.hpp"

GBFS::GBFS() {
    // 4: dir offset is 32 bit, 2: number of files is 16 bit
    archive_length = GBFS_MAGIC.size() + GBFS_RESERVED.size() + 4 + 2;
    dir_offset = GBFS_MAGIC.size() + GBFS_RESERVED.size();
    num_files = 0;
};

GBFS::GBFS(std::uint32_t archive_len, std::uint16_t dir_off,
           std::uint16_t num_f, std::vector<GBFSEntry> entr) {
    archive_length = archive_len;
    dir_offset = dir_off;
    num_files = num_f;
    entries = std::vector(entr);
};

/// Read an archive from an input stream.
GBFS GBFS::ReadArchive(std::istream& stream) {
    // First, the magic bytes
    std::array<char, 16> read_magic = {0};
    stream.read(read_magic.data(), read_magic.size());
    if (read_magic != GBFS_MAGIC) {
        FatalLog(
            "Attempt to read GBFS archive with invalid magic. Either the "
            "archive is corrupt or this is a bug.");
    }
    // Total length of archive
    const auto length = streamReadUint32(stream);
    // Offset from archive start to metadata directory
    const auto dir_offset = streamReadUint16(stream);
    // Number of files in the archive
    const auto file_amount = streamReadUint16(stream);
    // Reserved bytes
    std::array<char, 8> reserved = {0xD, 0xA};
    stream.read(reserved.data(), reserved.size());
    if (reserved != GBFS_RESERVED) {
        FatalLog(
            "Attempt to read GBFS archive with non-empty reserved bytes. "
            "Either the "
            "archive is of a new GBFS version or this is a bug.");
    }

    // Read all the file metadata entries and files
    stream.seekg(dir_offset, std::iostream::beg);
    std::vector<GBFSEntry> entries;
    for (auto i = 0; i < file_amount; i++) {
        // Read file's metadata
        char name[GBFS_FILENAME_LEN] = {0};
        stream.read(name, GBFS_FILENAME_LEN);
        const auto file_length = streamReadUint32(stream);
        // Offset to data from beginning of stream
        const auto file_data_offset = streamReadUint32(stream);

        // Read file data
        std::streampos old_pos = stream.tellg();
        stream.seekg(file_data_offset, std::iostream::beg);

        std::vector<unsigned char> file_contents;
        file_contents.reserve(file_length);
        // I'm sure this can be done much nicer, but I'm too noob at C++
        for (auto j = 0; j < file_length; j++) {
            char tmp;
            stream.read(&tmp, 1);
            file_contents.push_back(tmp);
        }

        GBFSEntry entry = GBFSEntry{file_contents, name};
        entries.push_back(entry);

        // Return to beginning of next file's metadata entry
        stream.seekg(old_pos, std::iostream::beg);
    }

    GBFS gbfs = GBFS(length, dir_offset, file_amount, entries);
    return gbfs;
};

/// Add a file to the archive. Note that the name must not be longer than
/// GBFS_FILENAME_LEN bytes.
void GBFS::AddFile(const std::string& name,
                   const std::vector<unsigned char>& contents) {
    if (name.length() > GBFS_FILENAME_LEN) {
        FatalLog("Attempt to add file with too long filename to GBFS archive");
    }
    // Calculate the new archive length
    this->archive_length += GBFS_FILE_ENTRY_SIZE + contents.size();
    this->num_files += 1;
    // Actually create a new entry
    GBFSEntry entry = GBFSEntry{
        contents,
        name,
    };
    this->entries.push_back(entry);
};

/// Write the archive to an output stream.
void GBFS::WriteArchive(std::ostream& stream) {
    // Magic
    stream.write(GBFS_MAGIC.data(), GBFS_MAGIC.size());
    // Length
    streamWriteUint32(stream, this->archive_length);
    // Offset to beginning of directory
    streamWriteUint16(stream, this->dir_offset);
    // Number of files
    streamWriteUint16(stream, this->num_files);
    // Reserved bytes
    stream.write(GBFS_RESERVED.data(), GBFS_RESERVED.size());

    // Write the metadata directory
    uint32_t next_file_offset =
        this->dir_offset * (this->num_files * GBFS_FILE_ENTRY_SIZE);
    for (auto i = 0; i < this->num_files; i++) {
    }

    // Files themselves
    for (auto i = 0; i < this->num_files; i++) {
        GBFSEntry entry = this->entries[i];

        // Write the filename into the metadata index.
        // Filenames are always exactly 24 bytes long, meaning that we may need
        // to pad. They also are not necessarily NULL-terminated.
        std::array<char, 24> name = {0};
        auto bytes_copied = 0;
        for (std::size_t j = 0; j < entry.name.length(); j++) {
            name[j] = entry.name.at(j);
            bytes_copied = j;
        }
        if (bytes_copied < 23) {
            for (int j = 0; j < (24 - bytes_copied); j++) {
                name[j] = 0;
            }
        }
        stream.write(name.data(), 24);

        // Write the file length into the metadata index.

        // Write the offset to the file's data from beginning of the file to the
        // metadata index.
        // TODO: Write the actual file contents
    }
};

void InitGBFSFile(std::ofstream& file, const std::string& name) {
    VerboseLog("Init GBFS File");
    std::string filename = name + ".gbfs";
    file.open(filename.c_str());

    if (!file.good())
        FatalLog("Could not open output file (%s) for writing", filename);
}

void GBFS::WriteAnimationFrames(
    const std::map<std::string, std::vector<Image*>>& name_frames){
    /*
for (unsigned int i = 0; i < params.names.size(); i++) {
    /// Get name of image and frames.
    const std::string& name = params.names[i];
    unsigned int frames = name_frames[name].size();
    if (frames <= 1) continue;
    std::vector<std::string> names;
    const std::string& img_type = name_frames[name][0]->GetImageType();
    for (const auto& image : name_frames[name])
        names.push_back(image->GetExportName());

    WriteAnimationArray(file, img_type, name, "_frames", names, 1);
    // If image has a palette and splitting
    if (name_frames[name][0]->HasPalette() && params.split) {
        for (auto& name : names) name += "_palette";
        WriteAnimationArray(file, "const unsigned short*", name,
                            "_palette_frames", names, 1);
    }
}

// TODO: Exportable needs method to just give us a byte vector
for (const auto& exportable : exportables) exportable->WriteData(file);
*/
};
void GBFS::WriteGraphics() {
    // Write frames of animations
    const auto name_frames = ExportFile::GetAnimatedImages();
    this->WriteAnimationFrames(name_frames);
};