#pragma once

#include <string>
#include <fstream>
#include "ImageWriter.h"

class NetpbmImageWriter : public ImageWriter
{
public:
    void write(const std::string &filename, Scene &scene) override
    {
        std::ofstream file(filename, std::ios::binary | std::ios::trunc);
        if (!file)
        {
            throw std::runtime_error("Could not open file: " + filename);
        }

        file << "P6\n"
             << scene.getWidth() << " " << scene.getHeight() << "\n255\n";

        for (int y = 0; y < scene.getHeight(); ++y)
        {
            for (int x = 0; x < scene.getWidth(); ++x)
            {
                const auto &raw_pixel = scene.getPixel(x, y);
                unsigned char pixel[3];
                pixel[0] = static_cast<unsigned char>(raw_pixel.r);
                pixel[1] = static_cast<unsigned char>(raw_pixel.g);
                pixel[2] = static_cast<unsigned char>(raw_pixel.b);

                file.write(reinterpret_cast<const char *>(pixel), 3);
            }
        }

        file.close();
    }
};