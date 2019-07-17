#ifndef SYM6432_H_INCLUDED__
#define SYM6432_H_INCLUDED__

#ifndef CONCAT
#define CONCAT(a,b) a##b
#endif


#ifdef __x86_64__
#define SYM6432(a) a
#else
#define SYM6432(a) CONCAT(a,32)
#endif



#endif // SYM6432_H_INCLUDED__
