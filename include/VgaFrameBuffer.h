#ifndef VGAFRAMEBUFFER_H_INCLUDED
#define VGAFRAMEBUFFER_H_INCLUDED

#include <cstdint>
#include "ssfn.h"

using size_t=std::size_t;


class alignas(8) VgaFrameBuffer
{
public:
    static constexpr size_t WIDTH=320;
    static constexpr size_t HEIGHT=320;
    VgaFrameBuffer()=default;

	uint16_t set_foreground_color(uint16_t clr);
	uint16_t set_background_color(uint16_t clr);


private:
    uint8_t back_buffer [WIDTH*HEIGHT] = {0};
    ssfn_buf_t  frame_buffer{
        back_buffer,
        WIDTH,
        HEIGHT,
        WIDTH,
        0,
        0,
        255,
        0
    };
    ssfn_font_t frame_font{
        .magic = {'S', 'F', 'N', '2'},
        .size = 0,
        .type = 0,
        .features = 0,
        .width = 0,
        .height = 0,
        .baseline = 0,
        .underline = 0,
        .fragments_offs = 0,
        .characters_offs = 0,
        .ligature_offs = 0,
        .kerning_offs = 0,
        .cmap_offs = 0,
    };
    void put(char c);
    void clear();
    void write(const char *str);
    void write_at(uint16_t row, uint16_t col, uint32_t fg, uint32_t bg, const char *str);
};

#endif // VGAFRAMEBUFFER_H_INCLUDED
