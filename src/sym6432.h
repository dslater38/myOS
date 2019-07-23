#ifndef SYM6432_H_INCLUDED__
#define SYM6432_H_INCLUDED__

#ifndef CONCAT
#define CONCAT(a,b) a##b
#endif


#define SYM6432(a) a

#ifdef __x86_64__
// #define SYM6432(a) a
#define STATIC32
#else
// #define SYM6432(a)  CONCAT(a,32)
#define STATIC32 static
#endif

// #define SYM6432(a) a

#endif // SYM6432_H_INCLUDED__
