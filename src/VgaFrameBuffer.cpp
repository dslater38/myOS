#include "VgaFrameBuffer.h"
#include "ssfn.h"


void VgaFrameBuffer::put(char c)
{
    ssfn_putc(c, &frame_font, &frame_buffer);
}
