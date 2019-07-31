#ifndef TEXTFRAMEBUFFER_H_INCLUDED
#define TEXTFRAMEBUFFER_H_INCLUDED

#include <cstdint>

using size_t=std::size_t;

enum class TextColors : uint8_t
{
	BLACK = 0,
	BLUE = 1,
	GREEN = 2,
	CYAN = 3,
	RED = 4,
	MAGENTA = 5,
	BROWN = 6,
	LTGRAY = 7,
	DKGRAY = 8,
	LTBLUE = 9,
	LTGREEN = 10,
	LTCYAN = 11,
	LTRED = 12,
	LTMAGENTA = 13,
	LTBROWN = 14,
	WHITE = 15,
	LAST = 16, 
	ERROR = 0xFF
};

enum class TextFlags : uint8_t
{
	ENABLE_DEBUG=(1<<0),
	ENABLE_SERIAL=(1<<1),
};

inline
uint8_t operator|(TextFlags a, TextFlags b)
{ return (static_cast<uint8_t>(a) | static_cast<uint8_t>(b) ); }
inline
uint8_t operator&(TextFlags a, TextFlags b)
{ return (static_cast<uint8_t>(a) & static_cast<uint8_t>(b) ); }	
inline
uint8_t operator~(TextFlags a)
{ return (~static_cast<uint8_t>(a) ); }
inline
bool test(uint8_t flags, TextFlags f)
{
	return ((flags & static_cast<uint8_t>(f)) == static_cast<uint8_t>(f));
}

class alignas(8) TextFrameBuffer
{
public:
	static constexpr size_t ROWS=25;
	static constexpr size_t COLS=80;
	static constexpr size_t VIDEO_MEM_COUNT=ROWS*COLS;
	
	TextColors set_foreground_color(TextColors clr);
	TextColors set_background_color(TextColors clr);

	void put(char c);
	void clear();
	void write(const char *str);

private:
	void inc_cur_line();
	uint16_t color_attribute();
	uint16_t colored_char(char c);
	void copy_buffer();
	void fast_copy_buffer();
	void blank_line(uint32_t line);
	void move_cursor();
	void copy_line(int dstLine, int srcLine);
	void scroll();
	void debug_out(char c);
	void serial_out(char c);
private:
	uint16_t back_buffer [VIDEO_MEM_COUNT] = {0};
	uint16_t *video_memory{reinterpret_cast<uint16_t *>(0x00000000000B8000)};
	TextColors backColor{TextColors::BLACK};
	TextColors foreColor {TextColors::GREEN};	
	uint32_t cur_line = 0;
	uint16_t cursor_x{0};
	uint16_t cursor_y {0};
	uint8_t	flags{TextFlags::ENABLE_SERIAL|TextFlags::ENABLE_DEBUG};
};

#endif // TEXTFRAMEBUFFER_H_INCLUDED
