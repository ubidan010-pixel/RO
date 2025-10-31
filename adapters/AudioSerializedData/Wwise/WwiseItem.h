
#ifndef __WWISEITEM_H__
#define __WWISEITEM_H__




#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"
#include "core/StringID.h"
#include "core/serializer/ZSerializerObject.h"


namespace	ITF
{
	class	WwiseItem
	{


	public:
			DECLARE_SERIALIZE()

            WwiseItem() : m_id(0) {}


            inline StringID         getName() const { return m_name; }
            inline StringID         getGUID() const { return m_guid; }
            inline u32              getID() const   { return m_id; }
            inline f32              getDuration() const { return m_duration; }


	private:
        StringID	m_name;
        StringID	m_guid;
        u32    m_id = 0;
        f32    m_duration = 0.f;
	};
}

#endif // __WWISEITEM_H__
