

template<class T>
void memcpy(T *src, T *dst, size_t count )
{
	for(auto pEnd=src+count;src<pEnd;++src,++dst)
	{
		*dst = *src;
	}
}

