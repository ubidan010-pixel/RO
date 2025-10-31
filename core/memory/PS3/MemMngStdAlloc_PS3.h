#ifndef ITF_MEMMNGSTDALLOC_PS3_H_
#define ITF_MEMMNGSTDALLOC_PS3_H_

namespace ITF
{

namespace MemMngStd 
{
	bool			Init();
    void            GetRange(void * & baseAddress, void * & limitAddress);
	void*			Alloc(u32 iSize, u32 uAlign);
	void			Free(void * pData);
	ux	            MSize(void * pData);
}

}

#endif // ITF_MEMMNGSTDALLOC_PS3_H_
