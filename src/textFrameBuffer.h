#ifndef TEXTFRAMEBUFFER_H_INCLUDED
#define TEXTFRAMEBUFFER_H_INCLUDED

#include "types.h"
#include <type_traits>

enum class VideoColors : uint8_t
{
	BLACK=0,
	BLUE=1,
	GREEN=2,
	CYAN=3,
	RED=4,
	MAGENTA=5,
	BROWN=6,
	LTGRAY=7,
	DKGRAY=8,
	LTBLUE=9,
	LTGREEN=10,
	LTCYAN=11,
	LTRED=12,
	LTMAGENTA=13,
	LTBROWN=14,
	WHITE=15,
	ERROR=0xFF
};

template<class Int, typename X=std::enable_if_t<std::is_integral_v<Int>> >
Int operator<<(VideoColors clr, Int n)
{
	return (static_cast<Int>(clr) << n);
}

template<class Int, typename X=std::enable_if_t<std::is_integral_v<Int>> >
Int operator&(VideoColors clr, Int n)
{
	return (static_cast<Int>(clr) & n);
}


struct IVideo
{
	virtual void put(char)=0;
	virtual void write(const char *)=0;
	virtual void clear()=0;
	virtual VideoColors setForeground(VideoColors)=0;
	virtual VideoColors setBackground(VideoColors)=0;
};

template<const uint32_t ROWS, const uint32_t COLS, const uint32_t DEPTH>
class TextModeVideo : public IVideo
{
public:
	void put(char)override;
	void write(const char *)override;
	void clear()override;
	VideoColors setForeground(VideoColors)override;
	VideoColors setBackground(VideoColors)override;
private:
	void scroll();
	void inc_cur_line();

	uint16_t color_attribute()const;
	uint16_t colored_char(char c)const;
	void copy_buffer()const;
	void copy_buffer2()const;
	void copy_line2(int dstLine, int srcLine)const;
	void move_cursor()const;

private:
	uint8_t 		back_buffer[ROWS*COLS] = {0};
	uint16_t		*video_memory{nullptr};
	uint32_t 		cur_line{0};
	uint16_t		cursor_x{0};
	uint16_t		cursor_y{0};
	VideoColors		backColor{VideoColors::BLACK};
	VideoColors		foreColor{VideoColors::WHITE};

};

struct FrameBuffer
{
	uint16_t *video_memory;
	
	void (*put)(char c);
	void (*write)(char *c);
	void (*clear)(void);
	void (*scroll)(void);
	void (*cursor)(void);
};

#endif // TEXTFRAMEBUFFER_H_INCLUDED
