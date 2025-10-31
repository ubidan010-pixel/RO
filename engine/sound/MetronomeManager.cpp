#include "precompiled_engine.h"

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif //_ITF_METRONOMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    //  METRONOME MANAGER TEMPLATE
    ///////////////////////////////////////////////////////////////////////////////////////////

    IMPLEMENT_OBJECT_RTTI(MetronomeManager_Template)

    BEGIN_SERIALIZATION(MetronomeManager_Template)
        SERIALIZE_MEMBER("defaultBPM",m_defaultBPM);
    END_SERIALIZATION()


    bbool MetronomeManager_Template::onTemplateLoaded()
    {
        return Super::onTemplateLoaded();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    //  METRONOME MANAGER
    ///////////////////////////////////////////////////////////////////////////////////////////

    MetronomeManager::MetronomeManager()
    {
        for (u32 i = 0 ; i < METRONOME_TYPE_COUNT ; ++i)
        {
            m_metronomes[i].reset();
        }

        // Starts virtual metronome
        setTimeSignature(METRONOME_DEFAULT_BPM,4,4,METRONOME_TYPE_DEFAULT);
    }

    MetronomeManager::~MetronomeManager()
    {
        for (u32 i = 0 ; i < METRONOME_TYPE_COUNT ; ++i)
        {
            m_metronomes[i].reset();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void MetronomeManager::start(MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/)
    {
        m_metronomes[_type].start();
    }

    void MetronomeManager::pause(MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/)
    {
        m_metronomes[_type].pause();
    }

    bbool MetronomeManager::isPaused(MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/)
    {
        return !m_metronomes[_type].isPlaying();
    }
    void MetronomeManager::resume(MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/)
    {
        m_metronomes[_type].resume();
    }

    void MetronomeManager::reset(MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/)
    {
        m_metronomes[_type].reset();
    }

    void MetronomeManager::update(u32 _barInPart, u32 _barSinceStart, f32 _barPercentage, MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/)
    {
        m_metronomes[_type].update(_barInPart, _barSinceStart, _barPercentage);
    }

    void MetronomeManager::update(f32 _dt, MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/)
    {
        m_metronomes[_type].update(_dt);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    void MetronomeManager::setTimeSignature(u32 _bpm,u32 _beatsPerBar,u32 _beatValue,MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/)
    {
        m_metronomes[_type].setTimeSignature(_bpm,_beatsPerBar,_beatValue);
    }

    void MetronomeManager::setParameters(u32 _nbBars, u32 _beatsPerBar, f32 _partDurationSeconds, MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/)
    {
        m_metronomes[_type].setParameters(_nbBars,_beatsPerBar,_partDurationSeconds);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

    f32 MetronomeManager::getBarPercentage( f32 _timeOffset /*= 0.0f*/, f32 _speedRatio /*= 1.0f*/, bbool _timeOffsetIsANote /*= btrue*/, MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/) const
    {
        if ( m_metronomes[_type].isPlaying() )
            return m_metronomes[_type].getBarPercentage(_timeOffset,_speedRatio,_timeOffsetIsANote);
        else
            return m_metronomes[METRONOME_TYPE_DEFAULT].getBarPercentage(_timeOffset,_speedRatio,_timeOffsetIsANote);
    }

    f32 MetronomeManager::getNoteTime(f32 _note, MetronomeType _type /*= METRONOME_TYPE_DEFAULT*/) const
    {
        return m_metronomes[_type].getNoteTime(_note);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////

#ifdef ITF_SUPPORT_DEBUGFEATURE
    // DEBUG
    void MetronomeManager::debugMetronomes()
    {

        for (u32 i = 0 ; i < METRONOME_TYPE_COUNT ; ++i)
        {
            //f32 barPercent = i == METRONOME_TYPE_DEFAULT ? m_metronomes[i].getBarPercentage(0.f, 1.f) : m_metronomes[i].getMeasurePercentage();
            f32 barPercent = m_metronomes[i].getBarPercentage(0.f, 1.f);
            f32 floatBeat = barPercent * ( m_metronomes[i].getBeatsPerBar() ? m_metronomes[i].getBeatsPerBar() : 4 );
            f32 beatPercent = f32_Modulo( floatBeat, 1.f );
            u32 beat = (u32) floatBeat;
            const Color& color = !m_metronomes[i].isPlaying() ? Color::grey() :
                barPercent <= 0.05f ? Color::red() :       // briefly highlight each bar in red
                beatPercent <= 0.05f ? Color::orange() :   // briefly highlight each beat in orange
                Color::white();

            DebugDraw::text(ITF_POS2D_AUTO, ITF_POS2D_AUTO, color,
                            "   (Measure: %d / %d, Beat: %d / %d) - bpm %.1f , Time : %f",
                             1 + m_metronomes[i].getBarInPart(), m_metronomes[i].getBars(), 1 + beat, m_metronomes[i].getBeatsPerBar(), m_metronomes[i].getBpm(), m_metronomes[i].getTimer()  );

            // Virtual metronome
            if ( i == 0 )
                DebugDraw::text(ITF_POS2D_AUTO, ITF_POS2D_AUTO, color, "%d : Virtual Metronome", i);
            else
                DebugDraw::text(ITF_POS2D_AUTO, ITF_POS2D_AUTO, color,
                "%d : Music: %s , Vol : %0.2f , Muted : %d"
                    , i, MUSICMANAGER->debugGetMusicName(i).getDebugString(), MUSICMANAGER->getLinearVolume(i), MUSICMANAGER->isMuted(i));
        }

    }
#endif // ITF_SUPPORT_DEBUGFEATURE
}
