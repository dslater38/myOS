
extern "C" 
{
    typedef int (*PrintfT)( const char* format, ... );
    void hello(PrintfT printf)
    {
            printf("Hello World from high half kernel.\n");
    }

}