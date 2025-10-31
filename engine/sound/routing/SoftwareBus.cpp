#include "precompiled_engine.h"

#ifndef _ITF_SOFTWAREBUS_H_
#include "engine/sound/routing/SoftwareBus.h"
#endif // _ITF_SOFTWAREBUS_H_


#include "engine/AdaptersInterfaces/SoundAdapter.h"
#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"


namespace ITF
{


    SoftwareBus::SoftwareBus( const BusDef * _busdef )
        : m_busdef( _busdef )
    {
        ITF_ASSERT( _busdef );
        if (AUDIO_ADAPTER)
        {
            if (m_busdef->getVolumeRtpc().isValid())
            {
                const SoundRtpcID	rtpcVolumeID = AUDIO_ADAPTER->getIDFromGUID(m_busdef->getVolumeRtpc());
                if (rtpcVolumeID != ITF_INVALID_SOUND_RTPC_ID)
                    AUDIO_ADAPTER->setRtpc(rtpcVolumeID, _busdef->m_volume.ratio(), ITF_INVALID_OBJREF); //volume in db ?
                else
                    LOG("Sound : rtpcVolumeID \"%s\" is not defined.", m_busdef->getVolumeRtpc().getDebugString());

            }
        }
    }


    SoftwareBus::~SoftwareBus()
    {
    }



} // namespace ITF

