#include <cstdio>

#include "fileutils.hpp"
#include "logger.hpp"
#include "image32.hpp"
#include "magick_interface.hpp"

Image32Bpp::Image32Bpp(const Magick::Image& image, const std::string& name, const std::string& filename, unsigned int frame, bool animated) :
    Image(image.columns(), image.rows(), name, filename, frame, animated)
{
    CopyMagickPixels(image, pixels);
}

void Image32Bpp::WriteData(std::ostream& file) const
{
    char buffer[32];
    WriteBeginArray(file, "const unsigned char", export_name, "", pixels.size() * 4);
    for (unsigned int i = 0; i < pixels.size(); i++)
    {
        const Color& color = pixels[i];
        snprintf(buffer, 32, "0x%02x,0x%02x,0x%02x,0x%02x", color.r, color.g, color.b, color.a);
        WriteElement(file, buffer, pixels.size() * 4, i, 8);
    }
    WriteEndArray(file);
    WriteNewLine(file);
}

void Image32Bpp::WriteCommonExport(std::ostream& file) const
{
    WriteDefine(file, name, "_SIZE", pixels.size() * 4);
    WriteDefine(file, name, "_WIDTH", width);
    WriteDefine(file, name, "_HEIGHT", height);
}

void Image32Bpp::WriteExport(std::ostream& file) const
{
    WriteExtern(file, "const unsigned char", export_name, "", pixels.size() * 4);
    if (!animated)
    {
        WriteDefine(file, export_name, "_SIZE", pixels.size() * 4);
        WriteDefine(file, export_name, "_WIDTH", width);
        WriteDefine(file, export_name, "_HEIGHT", height);
    }
    WriteNewLine(file);
}
