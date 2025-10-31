#ifndef _ITF_TEXTUREOUTPUTHANDLER_H_
#define _ITF_TEXTUREOUTPUTHANDLER_H_

#include <nvcore/StrLib.h>
#include <nvcore/StdStream.h>

namespace ITF
{

	ITF_INLINE void swapBigEndianDXT(u32 value, u8* pDest)
	{
		u8* v = (u8*)&value;
		pDest[0] = v[1];
		pDest[1] = v[0];
		pDest[2] = v[3];
		pDest[3] = v[2];
	}     


	struct TextureOutputHandler : public nvtt::OutputHandler
	{
        TextureOutputHandler(const wchar_t* srcname,const wchar_t* shortname,const char * name,const wchar_t* platform, const f32 _lod_bias = 0.f, const u32 _alphaBias = 0, bbool _uniqueRGB = bfalse, u32 _anisotropy = 0);

	virtual ~TextureOutputHandler();

	void setTotal(int64 t)
	{
		total = t + 128;
	}

	u8* getBufferSize(u32 _bufferSize)
	{
		if (_bufferSize>tmpBufferSize)
		{
			delete[] tmpBuffer;
			tmpBuffer = (u8*)new u8[_bufferSize];
			tmpBufferSize = _bufferSize;
		}

		return tmpBuffer;
	}

	void swapToBigEndian()
	{
		swapBigEndian = btrue;
	}

	void setDisplayProgress(bool b)
	{
		verbose = b;
	}

	virtual void beginImage(int size, int width, int height, int depth, int face, int miplevel)
	{
		// ignore.
	}

	virtual bool writeDataBlock(const void * data, int size);
	virtual bool writeDataHeader(const void * data, int size);
	virtual bool writeDataRGBA(const void * data, int size);
	// Output data.
    virtual bool writeData(const void * data, int size);
	

	int64 total;
	int64 progress;
	int percentage;
	bool verbose;
	bbool swapBigEndian;
	const wchar_t* m_srcname;
	const wchar_t* m_shortname;
	const wchar_t* m_platform;
	u32 tmpBufferSize;
	u8* tmpBuffer;
    f32 m_lod_bias;
    u32 m_AlphaBias;
    bbool m_HasOneColor;
    u32 m_Anisotropy;
    FILE * m_fp;

	nv::StdOutputStream * stream;
	};
}

#endif //_ITF_TEXTUREOUTPUTHANDLER_H_