#include "precompiled_engine.h"

#ifndef _ITF_METRONOME_H_
#include "engine/sound/Metronome.h"
#endif //_ITF_METRONOME_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

namespace ITF
{
    const f32 Metronome::HalfNote = 2.0f;
    const f32 Metronome::WholeNote = 4.0f;
    const f32 Metronome::QuarterNote = 1.0f;
    const f32 Metronome::EighthNote = 0.5f;
    const f32 Metronome::SixteenthNote = 0.25f;
    const f32 Metronome::Ronde = 2.0f;
    const f32 Metronome::Blanche = 4.0f;
    const f32 Metronome::Noire = 1.0f;
    const f32 Metronome::Croche = 0.5f;
    const f32 Metronome::DoubleCroche = 0.25f;

    Metronome::Metronome()
        : m_timer(0.0f)
        , m_nbBars(0)
        , m_beatsPerBar(0)
        , m_beatValue(0)
        , m_bpm(0)
        , m_barDuration(0.0f)
        , m_beatDuration(0.0f)
        , m_oneOverBeatsPerBar(0.f)
        , m_playing(bfalse)
        , m_barDurationSeconds(0.f)
        , m_barInPart(0)
        , m_barSinceStart(0)
    {
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventMetronomeSetBPM,1145786736),this);
    }

    Metronome::~Metronome()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventMetronomeSetBPM,1145786736), this);
    }

    void Metronome::onEvent( Event* _event )
    {
        if ( EventMetronomeSetBPM* eventBPM = _event->DynamicCast<EventMetronomeSetBPM>(ITF_GET_STRINGID_CRC(EventMetronomeSetBPM,1145786736)) )
        {
            setTimeSignature(eventBPM->getBPM(), 4, 4);
        }
    }

    void Metronome::reset()
    {
        m_bpm = 0;
        m_nbBars = 0;
        m_beatsPerBar = 0;
        m_beatValue = 0;

        m_beatDuration = 0.f;
        m_barDuration = 0.f;
        m_oneOverBeatsPerBar = 0.f;
        m_barDurationSeconds = 0.0f;

        m_barInPart         = 0;
        m_barSinceStart     = 0;

        m_timer = 0.0f;
        m_playing = bfalse;

        m_barPercentage = 0.0f;

        m_persistentCallbacks.clear();
        m_oneShotCallbacks.clear();
    }

    void Metronome::setTimeSignature(u32 _bpm,u32 _beatsPerBar,u32 _beatValue)
    {
        ITF_ASSERT(_bpm != 0);
        ITF_ASSERT(_beatsPerBar != 0);
        ITF_ASSERT(_beatValue != 0);

        m_bpm = _bpm;
        m_beatsPerBar = _beatsPerBar;
        m_beatValue = _beatValue;


        m_beatDuration =  (60.0f / (f32)m_bpm) * (4.0f / (f32)m_beatValue);
        m_barDurationSeconds = m_beatDuration * m_beatsPerBar;
        m_oneOverBeatsPerBar = 1.f / (f32)m_beatsPerBar;

        m_nbBars = 4;
    }

    void Metronome::setParameters( u32 _nbBars, u32 _beatsPerBar, f32 _partDurationSeconds )
    {
        ITF_ASSERT(_nbBars != 0);
        ITF_ASSERT(_beatsPerBar != 0);

        m_beatsPerBar       = _beatsPerBar;
        m_nbBars            = _nbBars;

        m_barDurationSeconds = _partDurationSeconds / (f32) _nbBars ;
    }

    void Metronome::addPersistentCB(metronomeCallback _cb, f32 _note, void * _context)
    {
        Callback cb(_cb, _note, _context);
        m_persistentCallbacks.push_back(cb);
    }
    void Metronome::addOneShotCB(metronomeCallback _cb, f32 _note, void * _context)
    {
        Callback cb(_cb, _note, _context);
        m_oneShotCallbacks.push_back(cb);
    }

    void Metronome::removePersistentCB(ITF::metronomeCallback _cb, f32 _note, void *_context)
    {
    }

    void Metronome::update(u32 _barInPart, u32 _barSinceStart, f32 _barPercentage)
    {
        if (m_playing)
        {
            m_barInPart = _barInPart;
            m_barSinceStart = _barSinceStart;
            m_barPercentage = _barPercentage;
        }
    }

    void Metronome::update(f32 _dt)
    {
        if ( m_playing )
        {
            m_timer += _dt;

            if ( m_timer > m_barDurationSeconds )
            {
                m_barSinceStart++;
                m_barInPart = m_barInPart + 1 >= m_nbBars ? 0 : m_barInPart + 1;
                m_timer -= u32(m_timer / m_barDurationSeconds) * m_barDurationSeconds;
            }

            m_barPercentage = m_timer / m_barDurationSeconds;
        }
    }

    void Metronome::start()
    {
        ITF_ASSERT(m_timer == 0.0f && m_playing == bfalse);
        m_playing = btrue;
    }

    // _timeOffset: [0,m_beatsPerBar] range if _timeOffsetIsANote, [0,1] range otherwise
    f32 Metronome::getBarPercentage( f32 _timeOffset /*= 0.0f*/, f32 _speedRatio /*= 1.0f*/, bbool _timeOffsetIsANote /*= btrue*/ ) const
    {
        if ( /*m_playing && */( m_barDurationSeconds > 0.f ) && m_beatsPerBar )
        {
            f32 absoluteBar = m_barPercentage + (f32)m_barSinceStart;

            if ( _timeOffsetIsANote )
                return f32_Modulo( ( absoluteBar * _speedRatio + _timeOffset / (f32) m_beatsPerBar ), 1.f );
            else
                // in this case, timeOffset is a bar ratio
                return f32_Modulo( ( absoluteBar * _speedRatio + _timeOffset /*/ m_barDurationSeconds */), 1.f );
        }
        else
        {
            return 0.0f;
        }
    }

    f32 Metronome::getNoteTime(f32 _note) const
    {
        f32 noteTime = _note * m_beatDuration;
        ITF_ASSERT(noteTime < m_barDuration);
        return noteTime;
    }

} // namespace ITF

