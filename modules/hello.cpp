// #include <stdio.h>

extern "C" 
{
    // extern int printf(const char* format, ... );
    typedef int (*PrintfT)( const char* format, ... );
    void hello(PrintfT printf)
    // void hello()
    {
            printf("Hello World from high half kernel.\n");
    }

}
