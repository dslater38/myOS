#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

struct IConsole
{
	virtual void put(char c)=0;
	virtual void write(const char *str)=0;
};



#endif // CONSOLE_H_INCLUDED
