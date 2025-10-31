
#ifndef __SOUNDDIALOGITEM_H__
#define __SOUNDDIALOGITEM_H__




#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"
#include "core/StringID.h"
#include "core/serializer/ZSerializerObject.h"


namespace	ITF
{
	class	SoundDialogItem
	{


	public:
			DECLARE_SERIALIZE()

            SoundDialogItem(){}


            inline StringID         getWaveFile() const { return m_waveFile; }
            inline StringID         getGUID() const { return m_WWiseguid; }
            inline u32              getLineID() const   { return m_lineId; }
            inline f32              getVolume () const { return m_volumedB; }


	private:
        StringID	m_waveFile;
        StringID	m_WWiseguid;
        u32    m_lineId = 0;
        f32    m_volumedB = 0.f;
	};
}

#endif // __SOUNDDIALOGITEM_H__
