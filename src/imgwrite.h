#pragma once
#include "png.h"
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include "zlib.h"
#include <vector>
#include <assert.h>
#include <cstring>
#include <bit>
#include "crc32.h"

namespace wpng
{
    namespace png_detail
    {
        typedef unsigned char ubyte;
        const bool machine_big_endian = (std::endian::native == std::endian::big);
        void write_signature(FILE* fh)
        {
            unsigned char png_signature[8] = {0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a};
            fwrite(&png_signature , sizeof(unsigned char), 8, fh);
        }
        
        void flip_endianness(int* input)
        {
            int j;
            ubyte byte_value;
            for (j = 0; j < sizeof(int)/2; j++)
            {
                byte_value = *(((ubyte*)input)+j);
                *(((ubyte*)input)+j) = *(((ubyte*)input)+sizeof(int)-j-1);
                *(((ubyte*)input)+sizeof(int)-j-1) = byte_value;
            }
        }

        void write_header(FILE* fh, const png_t& img)
        {
            const int width  = img.width ();
            const int height = img.height();
            int header_length = 13;
            int header_length_write = 13;
            unsigned char header[header_length+4];
            int output_width = width;
            int output_height = height;
            ubyte bit_depth = 0x8;
            ubyte color_type = 0x2;
            ubyte compression_method = 0x0;
            ubyte interlace_method = 0x0;
            ubyte filter_method = 0x0;
            if (!(machine_big_endian)) flip_endianness(&header_length_write);
            if (!(machine_big_endian)) flip_endianness(&output_width);
            if (!(machine_big_endian)) flip_endianness(&output_height);
            std::memcpy(header, "IHDR", 4);
            std::memcpy(header+4, &output_width, 4);
            std::memcpy(header+8, &output_height, 4);
            std::memcpy(header+12, &bit_depth, 1);
            std::memcpy(header+13, &color_type, 1);
            std::memcpy(header+14, &compression_method, 1);
            std::memcpy(header+15, &filter_method, 1);
            std::memcpy(header+16, &interlace_method, 1);
            fwrite(&header_length_write, 1, sizeof(int), fh);
            fwrite(header, 1, (4+header_length)*sizeof(ubyte), fh);

            int checksum = compute_checksum(&header[0], header_length+4);
            if (!(machine_big_endian)) flip_endianness(&checksum);
            fwrite(&checksum, 1, sizeof(int), fh);
        }
        
        void compress_memory(void *in_data, size_t in_data_size, std::vector<uint8_t> &out_data)
        {
            std::vector<uint8_t> buffer;

            const size_t BUFSIZE = 128 * 1024;
            uint8_t temp_buffer[BUFSIZE];

            z_stream strm;
            strm.zalloc = 0;
            strm.zfree = 0;
            strm.next_in = reinterpret_cast<uint8_t *>(in_data);
            strm.avail_in = in_data_size;
            strm.next_out = temp_buffer;
            strm.avail_out = BUFSIZE;

            deflateInit(&strm, Z_BEST_COMPRESSION);

            while (strm.avail_in != 0)
            {
                int res = deflate(&strm, Z_NO_FLUSH);
                assert(res == Z_OK);
                if (strm.avail_out == 0)
                {
                    buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE);
                    strm.next_out = temp_buffer;
                    strm.avail_out = BUFSIZE;
                }
            }

            int deflate_res = Z_OK;
            while (deflate_res == Z_OK)
            {
                if (strm.avail_out == 0)
                {
                    buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE);
                    strm.next_out = temp_buffer;
                    strm.avail_out = BUFSIZE;
                }
                deflate_res = deflate(&strm, Z_FINISH);
            }

            assert(deflate_res == Z_STREAM_END);
            buffer.insert(buffer.end(), temp_buffer, temp_buffer + BUFSIZE - strm.avail_out);
            deflateEnd(&strm);
            out_data.swap(buffer);
        }
        
        void write_data(FILE* file_writer, const png_t& img)
        {

            ubyte* buffer_in;
            const std::size_t col = img.width ();
            const std::size_t row = img.height();
            size_t input_buffer_size = ((3*col + 1) * row) * sizeof(ubyte);

            buffer_in = (ubyte*)malloc(input_buffer_size);
            //set line filter bytes

            int i, j;
            #pragma omp simd
            for (i = 0; i < row; i++) *(buffer_in + i*(3*col + 1)) = 0x00;

            //filter r g b r g b r g b ...
            #pragma omp simd
            for (i = 0; i < img.height(); i++)
            {
                for (j = 0; j < img.width(); j++)
                {
                    rgb_t px = img(i, j);
                    *(buffer_in + i*(3*col + 1) + 3*j + 1) = (ubyte)(255.0*px.r());
                    *(buffer_in + i*(3*col + 1) + 3*j + 2) = (ubyte)(255.0*px.g());
                    *(buffer_in + i*(3*col + 1) + 3*j + 3) = (ubyte)(255.0*px.b());
                }
            }

            std::vector<uint8_t> compressed_buffer_vec;
            compress_memory(buffer_in, input_buffer_size, compressed_buffer_vec);
            int comp_size = static_cast<int>(compressed_buffer_vec.size());
            int comp_size_write = comp_size;
            if (!machine_big_endian) flip_endianness(&comp_size_write);

            ubyte* comp_buffer_in = (ubyte*)malloc((4+comp_size)*sizeof(ubyte));
            std::copy(compressed_buffer_vec.begin(),compressed_buffer_vec.end(), comp_buffer_in + 4);
            *(comp_buffer_in+0) = 'I';
            *(comp_buffer_in+1) = 'D';
            *(comp_buffer_in+2) = 'A';
            *(comp_buffer_in+3) = 'T';

            fwrite(&comp_size_write, 1, sizeof(int), file_writer);
            fwrite(comp_buffer_in, 1, (4+comp_size)*sizeof(ubyte), file_writer);

            int crc32_data = compute_checksum(comp_buffer_in, 4+comp_size);
            if (!machine_big_endian) flip_endianness(&crc32_data);
            fwrite(&crc32_data, 1, sizeof(int), file_writer);
            free(comp_buffer_in);
            free(buffer_in);
        }
        
        void write_end(FILE* fh)
        {
            unsigned char endblock[12] = {0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae, 0x42, 0x60, 0x82};
            fwrite(endblock, 1, 12*sizeof(unsigned char), fh);
        }
    }

    void imgwrite(const png_t& img, const std::string& filename)
    {
        // std::endian::native == std::endian::big
        FILE* fh;
        fh = fopen(filename.c_str(), "w+b");
        png_detail::write_signature(fh);
        png_detail::write_header(fh, img);
        png_detail::write_data(fh, img);
        png_detail::write_end(fh);
        fclose(fh);
    }
}