#include <cstddef>
extern "C"
{
    int strcmp(const char *p1, const char *p2)
    {
        const unsigned char *s1 = (const unsigned char *) p1;
        const unsigned char *s2 = (const unsigned char *) p2;
        unsigned char c1, c2;
        do
        {
            c1 = *s1++;
            c2 = *s2++;
            // if (c1 == '\0')
            //     return c1 - c2;
        }while (c1 != '\0' && c1 == c2);
        return c1 - c2;
    }

    // Compare two strings. Should return -1 if 
    // str1 < str2, 0 if they are equal or 1 otherwise.
    int strncmp(const char *p1, const char *p2, size_t count)
    {
        const unsigned char *s1 = (const unsigned char *) p1;
        const unsigned char *s2 = (const unsigned char *) p2;
        unsigned char c1, c2;
        do
        {
            c1 = *s1++;
            c2 = *s2++;
            // if (c1 == '\0')
            //     return c1 - c2;
        }while (c1 != '\0' && c1 == c2 && ((--count) > 0) );
        return c1 - c2;
    }

}

