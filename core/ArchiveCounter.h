#ifndef _ITF_ARCHIVECOUNTER_H_
#define _ITF_ARCHIVECOUNTER_H_

#ifndef _ITF_STRING_H_
#include "core/itfstring.h"
#endif //_ITF_STRING_H_

#ifndef _ITF_CORETYPES_H_
#include "core/types.h"
#endif //_ITF_CORETYPES_H_

#ifndef _ITF_ARCHIVE_H_
#include "core/Archive.h"
#endif //_ITF_ARCHIVE_H_


namespace ITF
{
	class ArchiveCounter :public ArchiveMemory
	{

	public:

		ArchiveCounter():m_memoryCounter(0) {};
		virtual void rewindForReading() {};

	private:

		void serializeInternalRead(u8* _pBuffer,const i32 _size) {};

		void serializeInternalWrite(const u8* _pBuffer,const i32 _size)
		{
			m_memoryCounter+=_size;
		}

		void	getInfo(i32& , i32& ) {};

		u32		getMemory()	const {return m_memoryCounter;}

		u32 m_memoryCounter;
	};


} //namespace ITF

#endif //_ITF_ARCHIVECOUNTER_H_