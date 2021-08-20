#include "ssfn.h"

ssfn_buf_t frame_buffer;
ssfn_font_t *ssfn_src;          /* font buffer with an inflated bitmap font */
ssfn_buf_t ssfn_dst;            /* destination frame buffer */

void vga_write(const char *str)
{
    ssfn_load(nullptr,nullptr);
    while(str && *str)
    {
        ssfn_putc(*str, ssfn_src, &ssfn_dst);
    }
}
