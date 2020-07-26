#include <gbfsutils.hpp>

std::array<char, 4> uint32ToBytes(const std::uint32_t n) {
    std::array<char, 4> bytes = {0};
    bytes[0] = (n >> 24) & 0xFF;
    bytes[1] = (n >> 16) & 0xFF;
    bytes[2] = (n >> 8) & 0xFF;
    bytes[3] = n & 0xFF;
    return bytes;
};

std::array<char, 2> uint16ToBytes(const std::uint16_t n) {
    std::array<char, 2> bytes = {0};
    bytes[0] = (n >> 8) & 0xFF;
    bytes[1] = n & 0xFF;
    return bytes;
};

uint32_t bytesToUint32(const std::array<char, 4> b) {
    uint32_t result = 0;
    result = b[0] | (b[1] << 8) | (b[2] << 16) | (b[3] << 24);
    return result;
};

uint16_t bytesToUint16(const std::array<char, 2> b) {
    uint16_t result = 0;
    result = b[0] | (b[1] << 8);
    return result;
};

uint32_t streamReadUint32(std::istream& stream) {
    std::array<char, 4> buf = {0};
    stream.read(buf.data(), 4);
    return bytesToUint32(buf);
};

uint16_t streamReadUint16(std::istream& stream) {
    std::array<char, 2> buf = {0};
    stream.read(buf.data(), 2);
    return bytesToUint16(buf);
};

void streamWriteUint32(std::ostream& stream, const std::uint32_t n) {
    const auto buf = uint32ToBytes(n);
    stream.write(buf.data(), 4);
};

void streamWriteUint16(std::ostream& stream, const std::uint16_t n) {
    const auto buf = uint16ToBytes(n);
    stream.write(buf.data(), 2);
};