#pragma once
#include <concepts>
#include "types.h"
namespace wpng
{
    template <typename rgb_base_t> struct rgb_data_t
    {
        rgb_base_t rgb[3];
        rgb_base_t& r(void)       { return rgb[0]; }
        const rgb_base_t& r(void) const { return rgb[0]; }
        rgb_base_t& g(void)       { return rgb[1]; }
        const rgb_base_t& g(void) const { return rgb[1]; }
        rgb_base_t& b(void)       { return rgb[2]; }
        const rgb_base_t& b(void) const { return rgb[2]; }
        rgb_data_t(const rgb_base_t& r_in, const rgb_base_t& g_in, const rgb_base_t& b_in)
        {
            this->r() = r_in;
            this->g() = g_in;
            this->b() = b_in;
        }
        
        rgb_data_t(void)
        {
            this->r() = 0;
            this->g() = 0;
            this->b() = 0;
        }
        
        rgb_data_t<rgb_base_t>& operator = (const rgb_data_t<rgb_base_t>& rhs)
        {
            this->r() = rhs.r();
            this->g() = rhs.g();
            this->b() = rhs.b();
            return *this;
        }
        
        rgb_data_t<rgb_base_t> operator + (const rgb_data_t<rgb_base_t>& rhs) const
        {
            return rgb_data_t<rgb_base_t>(this->r()+rhs.r(),this->g()+rhs.g(),this->b()+rhs.b());
        }
        
        rgb_data_t<rgb_base_t> operator - (const rgb_data_t<rgb_base_t>& rhs) const
        {
            return rgb_data_t<rgb_base_t>(this->r()-rhs.r(),this->g()-rhs.g(),this->b()-rhs.b());
        }
        
        rgb_data_t<rgb_base_t> operator * (const rgb_base_t& rhs) const
        {
            return rgb_data_t<rgb_base_t>(rhs*this->r(),rhs*this->g(),rhs*this->b());
        }
        
        rgb_data_t<rgb_base_t>& operator += (const rgb_data_t<rgb_base_t>& rhs)
        {
            this->r() += rhs.r();
            this->g() += rhs.g();
            this->b() += rhs.b();
            return *this;
        }
        
        rgb_data_t<rgb_base_t>& operator -= (const rgb_data_t<rgb_base_t>& rhs)
        {
            this->r() -= rhs.r();
            this->g() -= rhs.g();
            this->b() -= rhs.b();
            return *this;
        }
    };

    template <std::floating_point f_t, typename base_t> rgb_data_t<base_t>
    operator * (const f_t lhs, const rgb_data_t<base_t>& rgb)
    {
        return rgb_data_t<base_t>(lhs*rgb.r(), lhs*rgb.g(), lhs*rgb.b());
    }

    typedef rgb_data_t<real_t> rgb_t;
    namespace colors
    {
        static rgb_t red  (1.0, 0.0, 0.0);
        static rgb_t green(0.0, 1.0, 0.0);
        static rgb_t blue (0.0, 0.0, 1.0);
    }
}