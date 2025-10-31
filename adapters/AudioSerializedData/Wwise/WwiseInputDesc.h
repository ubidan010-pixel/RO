#ifndef __WWISEINPUTDESC_H__
#define __WWISEINPUTDESC_H__



#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"
#include "core/serializer/ZSerializerObject.h"

namespace	ITF
{
	class	WwiseInputDesc
	{
	public:
			DECLARE_SERIALIZE()

            WwiseInputDesc();

			void	                resolve();
            inline SoundRtpcID      getRtpcID() const       { return m_rtpcID; }
            inline const StringID & getInputName() const    { return m_inputName; }
            inline AudioRtpcType    getRtpcType() const     { return m_rtpcType; }
            inline StringID         getRtpcGUID() const      { return m_rtpcGUID; }
	private:
			SoundRtpcID		m_rtpcID;
			StringID		m_rtpcGUID;
			StringID		m_inputName;
			AudioItemType	m_type;
			bbool			m_isLocal;
            AudioRtpcType   m_rtpcType;
	};
}

#endif // __WWISEINPUTDESC_H__
