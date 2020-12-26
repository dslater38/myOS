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

}