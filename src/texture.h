#pragma once
#include <string>
#include <iostream>
#include <string>
#include <fstream>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <assert.h>
#include <cstring>
#include <bit>

namespace wpng
{
    template <typename data_t> class texture
    {
        public:
            texture()
            {
                nrow = 0;
                ncol = 0;
            }
            
            texture(const std::size_t& n_row_in, const std::size_t& n_col_in)
            {
                this->resize(n_row_in, n_col_in);
            }
            
            void resize(std::size_t n_row_in, std::size_t n_col_in)
            {
                nrow = n_row_in;
                ncol = n_col_in;
                tex_data.resize(n_row_in*n_col_in);
            }
            
            _finline_ const data_t& operator() (const std::size_t& row, const std::size_t& col) const noexcept
            {
                return tex_data[col + row*ncol];
            }
            
            _finline_ data_t& operator() (const std::size_t& row, const std::size_t& col) noexcept
            {
                return tex_data[col + row*ncol];
            }
            
            std::size_t height(void) const {return nrow;}
            std::size_t width (void) const {return ncol;}
            void* base_ptr(void) {return (void*)(&tex_data[0]);}
            std::vector<data_t>& data() {return tex_data;}
        private:
            std::vector<data_t> tex_data;
            std::size_t nrow, ncol;
    };

    template <typename data_t> void read_texture(texture<data_t>& data, const std::string& filename)
    {
        FILE* fh;
        fh = fopen(filename.c_str(), "r+b");
        int nrow, ncol;
        auto j = fread(&nrow, 1, sizeof(int), fh);
        j = fread(&ncol, 1, sizeof(int), fh);
        data.resize(nrow, ncol);
        j = fread(data.base_ptr(), nrow*ncol, sizeof(data_t), fh);
        fclose(fh);
    }

    template <typename src_t, typename dest_t, class callable_transform_t>
    void convert_texture (const texture<src_t>& src, texture<dest_t>& dest, const callable_transform_t& func)
    {
        dest.resize(src.height(), src.width());
        for (std::size_t j = 0; j < src.height(); ++j)
        {
            for (std::size_t i = 0; i < src.width(); ++i)
            {
                dest(j,i) = func(src(j,i));
            }
        }
    }
}