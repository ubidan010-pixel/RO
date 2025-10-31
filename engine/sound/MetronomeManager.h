#ifndef _ITF_METRONOMEMANAGER_H_
#define _ITF_METRONOMEMANAGER_H_

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter_Types.h"

#define METRONOME_DEFAULT_BPM 120

namespace ITF
{
  /*  enum MetronomeType
    {
        METRONOME_TYPE_DEFAULT = 0,
        METRONOME_TYPE_MENU,
        METRONOME_TYPE_GAMEPLAY,
        METRONOME_TYPE_LUMKING,
        METRONOME_TYPE_COUNT,
        ENUM_FORCE_SIZE_32(MetronomeType)
    };

    #define SERIALIZE_METRONOMETYPE(name,val) \
    SERIALIZE_ENUM_BEGIN(name,val); \
        SERIALIZE_ENUM_VAR(METRONOME_TYPE_DEFAULT); \
        SERIALIZE_ENUM_VAR(METRONOME_TYPE_MENU); \
        SERIALIZE_ENUM_VAR(METRONOME_TYPE_GAMEPLAY); \
        SERIALIZE_ENUM_VAR(METRONOME_TYPE_LUMKING); \
    SERIALIZE_ENUM_END();*/

    ///////////////////////////////////////////////////////////////////////////////////////////
    //  METRONOME MANAGER TEMPLATE
    ///////////////////////////////////////////////////////////////////////////////////////////

    class MetronomeManager_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(MetronomeManager_Template, TemplateObj,3007306876);
        DECLARE_SERIALIZE()

    public:
        MetronomeManager_Template()
            : Super()
            , m_defaultBPM(120)
        {}
        MetronomeManager_Template(const Path& _path)
            : Super(_path)
            , m_defaultBPM(120)
        {}

        virtual bbool onTemplateLoaded();

        //const u32       getDefaultBPM() const { return m_defaultBPM; }

    private:
        u32         m_defaultBPM;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    //  METRONOME MANAGER
    ///////////////////////////////////////////////////////////////////////////////////////////

    class MetronomeManager
    {
    public:

        DECLARE_SERIALIZE()

        MetronomeManager();
        ~MetronomeManager();

        void        init();

        //const MetronomeManager_Template *   getTemplate() { return m_template; }

        void            start(MetronomeType _type = METRONOME_TYPE_DEFAULT);
        void            pause(MetronomeType _type = METRONOME_TYPE_DEFAULT);
        bbool           isPaused(MetronomeType _type = METRONOME_TYPE_DEFAULT);
        void            resume(MetronomeType _type = METRONOME_TYPE_DEFAULT);
        void            reset(MetronomeType _type = METRONOME_TYPE_DEFAULT);
        void            update(u32 _barInPart, u32 _barSinceStart, f32 _barPercentage, MetronomeType _type = METRONOME_TYPE_DEFAULT);
        void            update(f32 _dt, MetronomeType _type = METRONOME_TYPE_DEFAULT);

        void            setTimeSignature(u32 _bpm, u32 _beatsPerBar, u32 _beatValue,MetronomeType _type = METRONOME_TYPE_DEFAULT);
        void            setParameters(u32 _nbBars, u32 _beatsPerBar, f32 _partDurationSeconds, MetronomeType _type = METRONOME_TYPE_DEFAULT);

        f32             getBarPercentage(f32 _timeOffset = 0.0f, f32 _speedRatio = 1.0f, bbool _timeOffsetIsANote = btrue, MetronomeType _type = METRONOME_TYPE_DEFAULT) const;
        f32             getNoteTime(f32 _note, MetronomeType _type = METRONOME_TYPE_DEFAULT) const;

        ITF_INLINE u32 getBpm(MetronomeType _type = METRONOME_TYPE_DEFAULT) const { return m_metronomes[_type].getBpm(); }
        ITF_INLINE u32 getBeatsPerBar(MetronomeType _type = METRONOME_TYPE_DEFAULT) const { return m_metronomes[_type].getBeatsPerBar(); }
        ITF_INLINE f32 getOneOverBeatsPerBar(MetronomeType _type = METRONOME_TYPE_DEFAULT) const { return m_metronomes[_type].getOneOverBeatsPerBar(); }
        ITF_INLINE u32 getBeatValue(MetronomeType _type = METRONOME_TYPE_DEFAULT) const { return m_metronomes[_type].getBeatValue(); }
        ITF_INLINE f32 getBeatDuration(MetronomeType _type = METRONOME_TYPE_DEFAULT) const { return m_metronomes[_type].getBeatDuration(); }
        ITF_INLINE f32 getBarDuration(MetronomeType _type = METRONOME_TYPE_DEFAULT) const { return m_metronomes[_type].getBarDuration(); }
        ITF_INLINE f32 getTimer(MetronomeType _type = METRONOME_TYPE_DEFAULT) const { return m_metronomes[_type].getTimer(); }

        //const u32       getDefaultBPM() const { return m_template->getDefaultBPM(); }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        // DEBUG
        void            debugMetronomes();
#endif // ITF_SUPPORT_DEBUGFEATURE

    private:
        //const MetronomeManager_Template*    m_template;

        Metronome       m_metronomes[METRONOME_TYPE_COUNT];
    };
}

#endif //_ITF_METRONOMEMANAGER_H_