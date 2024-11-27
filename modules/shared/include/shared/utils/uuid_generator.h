// copied from lama project
//
// Helper class to generate unique ids.
// from https://stackoverflow.com/questions/24365331/how-can-i-generate-uuid-in-c-without-using-boost-library
#pragma once

#include <random>
#include <sstream>

class UuidGenerator
{

private:
public:
    static std::string generateUuidV4()
    {
        // made those static to not seed a new engine for every call
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<int> hexDist(0, 15);
        static std::uniform_int_distribution<int> variantDist(8, 11);

        // stuff below is by chatgpt
        std::array<int, 16> uuidData;

        // Generate random data for UUID
        for ( auto &byte : uuidData ) {
            byte = hexDist(gen);
        }

        uuidData[6] = 0x4; // Version field (4 for UUID v4)
        uuidData[8] = variantDist(gen); // Variant field (8-B for RFC4122 compliance)

        // Format as string
        std::ostringstream ss;
        ss << std::hex;

        for ( size_t i = 0; i < uuidData.size(); ++i ) {
            ss << uuidData[i];
            // Add hyphens at specific positions
            if ( i == 3 || i == 5 || i == 7 || i == 9 ) {
                ss << "-";
            }
        }

        return ss.str();
    }
};
