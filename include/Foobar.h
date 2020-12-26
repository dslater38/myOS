#ifndef FOOBAR_H_
#define FOOBAR_H_


class Foobar
{
public:
	Foobar(const char *str)
	: m_str{str}
	{}
	const char *get()const { return m_str; }
private:
	const char *m_str{nullptr};
};


#endif // FOOBAR_H_
