#pragma once
#include <iostream>
namespace wpng
{
    const std::size_t crc32_table_size = 256;
    unsigned int compute_checksum(unsigned char* bytes, std::size_t length)
    {
        unsigned int table[crc32_table_size];
        unsigned int poly = 0xedb88320;
        unsigned int temp = 0;
        unsigned int i;
        for(i = 0; i < crc32_table_size; i++)
        {
            temp = i;
            for(int j = 8; j > 0; j--)
            {
                if((temp & 1) == 1)
                {
                    temp = (unsigned int)((temp >> 1) ^ poly);
                }
                else
                {
                    temp >>= 1;
                }
            }
            table[i] = temp;
        }
        
        unsigned int crc = 0xffffffff;
        for(int i = 0; i < length; i++)
        {
            unsigned char index = (unsigned char)(((crc) & 0xff) ^ bytes[i]);
            crc = (unsigned int)((crc >> 8) ^ table[index]);
        }
        return ~crc;
    }
}
