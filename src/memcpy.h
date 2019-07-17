
template<class T>
void *memcpy(T *src, T *dst, size_t count )
{
	auto *retVal = dst;
	for(auto pEnd=src+count;src<pEnd;++src,++dst)
	{
		*dst = *src;
	}
	return retVal;
}

void *memset(void *dest, uint8_t val, uint32_t size);