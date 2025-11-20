#ifndef _ITF_SOUNDCONFIGTEMPLATE_H_
#define _ITF_SOUNDCONFIGTEMPLATE_H_



#include "engine/AdaptersInterfaces/SoundAdapter.h"
#include "adapters/AudioSerializedData/Wwise/WwiseItem.h"
#include "adapters/AudioSerializedData/Wwise/SoundDialogItem.h"
#include "adapters/AudioSerializedData/Wwise/WwiseEngineEvent.h"



namespace ITF
{

    struct RtpcConfig
    {
        DECLARE_SERIALIZE()
        StringID m_volumeGUID;
        StringID m_pitchGUID;
        StringID m_panGUID;
        StringID m_spanGUID;
        StringID m_lowpassGUID;
        StringID m_highpassGUID;
        StringID m_MenuGlobalGUID;
        StringID m_MenuMusicGUID;
        StringID m_MenuSFXGUID;
    };

    class SoundConfig_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(SoundConfig_Template, TemplateObj, 2612383856);
        DECLARE_SERIALIZE()
        SoundConfig_Template()
            : Super()
        {
        }
        SoundConfig_Template(const Path& _path)
            : Super(_path)
        {
        }
    public:
        ITF_INLINE const ITF_VECTOR<BusDef>& getBusDefs() const { return m_busDefs; }
        ITF_INLINE const SafeArray<LimiterDef>& getLimiterDefs() const { return m_limiterDefs; }
        ITF_INLINE const ITF_VECTOR<BusMix>& getBusMixBank() const { return m_busMixBank; }

        void                                        loadConfigBank() const;
        void	                                    unloadConfigBank() const;

        //wwise stuf 
        inline const ITF_VECTOR<WwiseItem>& getWwiseItemTable() const { return m_WWiseItemList; }
        inline const ITF_VECTOR<SoundDialogItem>& getSoundDialogItemTable() const { return m_SoundDialogItemList; }

        inline const RtpcConfig& getWwiseRtpc() const { return m_RTPC; }
        inline const ITF_VECTOR<WwiseEngineEvent>& getWwiseEngineEventList() const { return m_wwiseEngineEventList; }





    private:
        ITF_VECTOR<BusDef> m_busDefs;
        SafeArray<LimiterDef> m_limiterDefs;
        ITF_VECTOR<BusMix> m_busMixBank;

        //wwise stuf
        ITF_VECTOR<WwiseItem>                   m_WWiseItemList;
        RtpcConfig                              m_RTPC;
        ITF_VECTOR<SoundDialogItem>             m_SoundDialogItemList;
        ITF_VECTOR<WwiseEngineEvent>            m_wwiseEngineEventList;


    };

}

#endif // __SOUNDCONFIGTEMPLATE_H__
