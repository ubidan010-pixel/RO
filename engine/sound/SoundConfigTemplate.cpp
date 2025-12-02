#include "precompiled_engine.h"

#ifndef _ITF_SOUND_H_
#include "engine/sound/SoundConfigTemplate.h"
#endif //_ITF_SOUND_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

namespace ITF
{


    BEGIN_SERIALIZATION(RtpcConfig)
    SERIALIZE_MEMBER("volumeRtpcGuid", m_volumeGUID);
    SERIALIZE_MEMBER("pitchRtpcGuid", m_pitchGUID);
    SERIALIZE_MEMBER("panRtpcGuid", m_panGUID);
    SERIALIZE_MEMBER("spanRtpcGuid", m_spanGUID);
    SERIALIZE_MEMBER("lowpassRtpcGuid", m_lowpassGUID);
    SERIALIZE_MEMBER("highpassRtpcGuid", m_highpassGUID);
    SERIALIZE_MEMBER("MenuVolumeGlobalRtpcGuid", m_MenuGlobalGUID);
    SERIALIZE_MEMBER("MenuVolumeMusicRtpcGuid", m_MenuMusicGUID);
    SERIALIZE_MEMBER("MenuVolumeSFXRtpcGuid", m_MenuSFXGUID);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(SoundConfig_Template)
    BEGIN_SERIALIZATION(SoundConfig_Template)
    SERIALIZE_CONTAINER_OBJECT("busses", m_busDefs);
    SERIALIZE_CONTAINER_OBJECT("limiters", m_limiterDefs);
    SERIALIZE_CONTAINER_OBJECT("busMixBank", m_busMixBank);
    SERIALIZE_CONTAINER_OBJECT("WWiseItemList", m_WWiseItemList);
    SERIALIZE_CONTAINER_OBJECT("SoundDialogItemList", m_SoundDialogItemList);
    SERIALIZE_OBJECT("WwiseRTPC", m_RTPC);
    SERIALIZE_CONTAINER_OBJECT("WwiseEngineEventList", m_wwiseEngineEventList);
    END_SERIALIZATION()

    void SoundConfig_Template::loadConfigBank() const
    {
#if defined(ITF_SUPPORT_WWISE)
        AUDIO_ADAPTER->loadBank("Init.bnk", this);
#if defined(ITF_SUPPORT_WWISE_DYNAMICLOADING)
        AUDIO_ADAPTER->prepareBank("PlayableCharactere.bnk");
        AUDIO_ADAPTER->prepareBank("UI.bnk");
        AUDIO_ADAPTER->prepareBank("Introduction.bnk");
        AUDIO_ADAPTER->prepareBank("Music.bnk");
        AUDIO_ADAPTER->prepareBank("MusicW.bnk");
        AUDIO_ADAPTER->prepareBank("Jungle.bnk");
        AUDIO_ADAPTER->prepareBank("Cinematic.bnk");
        AUDIO_ADAPTER->prepareBank("Global.bnk");
        AUDIO_ADAPTER->prepareBank("Dialog.bnk");
        AUDIO_ADAPTER->prepareBank("Food.bnk");
        AUDIO_ADAPTER->prepareBank("Mountain.bnk");
        AUDIO_ADAPTER->prepareBank("LandofTheDead.bnk");
        AUDIO_ADAPTER->prepareBank("Ocean.bnk");
#else
        AUDIO_ADAPTER->loadBank("PlayableCharactere.bnk", this);
        AUDIO_ADAPTER->loadBank("UI.bnk", this);
        AUDIO_ADAPTER->loadBank("Introduction.bnk", this);
        AUDIO_ADAPTER->loadBank("MusicW.bnk", this);
        AUDIO_ADAPTER->loadBank("Jungle.bnk", this);
        AUDIO_ADAPTER->loadBank("Cinematic.bnk", this);
        AUDIO_ADAPTER->loadBank("Global.bnk", this);
        AUDIO_ADAPTER->loadBank("Dialog.bnk", this);
        AUDIO_ADAPTER->loadBank("Food.bnk", this);
        AUDIO_ADAPTER->loadBank("Mountain.bnk", this);
        AUDIO_ADAPTER->loadBank("LandofTheDead.bnk", this);
        AUDIO_ADAPTER->loadBank("Ocean.bnk", this);
        AUDIO_ADAPTER->loadBank("Music.bnk", this);
#endif

#endif
    }

    void SoundConfig_Template::unloadConfigBank() const
    {
#if defined(ITF_SUPPORT_WWISE)
        AUDIO_ADAPTER->unloadBank("Init.bnk", this);
#if defined(ITF_SUPPORT_WWISE_DYNAMICLOADING)
        AUDIO_ADAPTER->unprepareBank("PlayableCharactere.bnk");
        AUDIO_ADAPTER->unprepareBank("UI.bnk");
        AUDIO_ADAPTER->unprepareBank("Introduction.bnk");
        AUDIO_ADAPTER->unprepareBank("Music.bnk");
        AUDIO_ADAPTER->unprepareBank("MusicW.bnk");
        AUDIO_ADAPTER->unprepareBank("Jungle.bnk");
        AUDIO_ADAPTER->unprepareBank("Cinematic.bnk");
        AUDIO_ADAPTER->unprepareBank("Global.bnk");
        AUDIO_ADAPTER->unprepareBank("Dialog.bnk");
        AUDIO_ADAPTER->unprepareBank("Food.bnk");
        AUDIO_ADAPTER->unprepareBank("Mountain.bnk");
        AUDIO_ADAPTER->unprepareBank("LandofTheDead.bnk");
        AUDIO_ADAPTER->unprepareBank("Ocean.bnk");
#else
        AUDIO_ADAPTER->unloadBank("PlayableCharactere.bnk", this);
        AUDIO_ADAPTER->unloadBank("UI.bnk", this);
        AUDIO_ADAPTER->unloadBank("Introduction.bnk", this);
        AUDIO_ADAPTER->unloadBank("Music.bnk", this);
        AUDIO_ADAPTER->unloadBank("MusicW.bnk", this);
        AUDIO_ADAPTER->unloadBank("Jungle.bnk", this);
        AUDIO_ADAPTER->unloadBank("Cinematic.bnk", this);
        AUDIO_ADAPTER->unloadBank("Global.bnk", this);
        AUDIO_ADAPTER->unloadBank("Dialog.bnk", this);
        AUDIO_ADAPTER->unloadBank("Food.bnk", this);
        AUDIO_ADAPTER->unloadBank("Mountain.bnk", this);
        AUDIO_ADAPTER->unloadBank("LandofTheDead.bnk", this);
        AUDIO_ADAPTER->unloadBank("Ocean.bnk", this);
#endif

#endif
    }


} // namespace ITF


