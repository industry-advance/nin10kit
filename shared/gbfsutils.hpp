#ifndef BYTEUTILS_HPP
#define BYTEUTILS_HPP

// I can't believe this stuff isn't in the stdlib
// I mean come on, this is used all the goddamn time
#include <array>
#include <cstdint>
#include <iostream>

std::array<char, 4> uint32ToBytes(const std::uint32_t n);
std::array<char, 2> uint16ToBytes(const std::uint16_t n);
uint32_t bytesToUint32(const std::array<char, 4> b);
uint16_t bytesToUint16(const std::array<char, 2> b);
uint32_t streamReadUint32(std::istream& stream);
uint16_t streamReadUint16(std::istream& stream);
void streamWriteUint32(std::ostream& stream, const std::uint32_t n);
void streamWriteUint16(std::ostream& stream, const std::uint16_t n);
#endif