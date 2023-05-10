#include <print.h>
#include "wpng.h"

int main(int argc, char** argv)
{
    wpng::png_t im(16, 16);
    im.fill(wpng::colors::blue);
    im(10, 10) = wpng::colors::red;
    wpng::imgwrite(im, "tet.png");
    return 0;
}