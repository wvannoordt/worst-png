#pragma once
#include <vector>
#include "rgb.h"
#include "attrib.h"
#include "texture.h"

namespace wpng
{
    class png_t
    {
        public:
            png_t(const std::size_t& n_row_in, const std::size_t& n_col_in)
            {
                px_data.resize(n_row_in, n_col_in);
            }
            
            _finline_ const rgb_t& operator() (const std::size_t& row, const std::size_t& col) const noexcept
            {
                return px_data(row,col);
            }
            
            _finline_ rgb_t& operator() (const std::size_t& row, const std::size_t& col) noexcept
            {
                return px_data(row,col);
            }
            
            _finline_ void fill(const rgb_t& color)
            {
                for (auto& p: px_data.data()) p = color;
            }
            
            std::size_t height(void) const {return px_data.height();}
            std::size_t width (void) const {return px_data.width();}
            
        private:
            texture<rgb_t> px_data;
    };
}