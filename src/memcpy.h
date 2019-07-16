
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

void *memset(void *dest, u8int val, u32int size);