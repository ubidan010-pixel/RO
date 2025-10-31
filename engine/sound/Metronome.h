#ifndef _ITF_METRONOME_H_
#define _ITF_METRONOME_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_


namespace ITF
{
    typedef void (*metronomeCallback)(f32 _beat, f32 _barPercentage, void * _context); 

    class Metronome : public IEventListener
    {
    public:

        static const f32 HalfNote;
        static const f32 WholeNote;
        static const f32 QuarterNote;
        static const f32 EighthNote;
        static const f32 SixteenthNote;
        static const f32 Ronde;
        static const f32 Blanche;
        static const f32 Noire;
        static const f32 Croche;
        static const f32 DoubleCroche;

        Metronome();
        ~Metronome();

        void setTimeSignature(u32 _bpm,u32 _beatsPerBar,u32 _beatValue);
        void setParameters(u32 _nbBars, u32 _beatsPerBar, f32 _partDurationSeconds);
        void start();
        void addPersistentCB(metronomeCallback _cb, f32 _beat, void * _context);
        void removePersistentCB(metronomeCallback _cb,f32 _beat, void * _context);
        void addOneShotCB(metronomeCallback _cb,f32 _beat, void * _context);
        void reset();
        void update(u32 _barInPart, u32 _barSinceStart, f32 _barPercentage);
        void update(f32 _dt);

        f32 getBarPercentage(f32 _timeOffset = 0.0f, f32 _speedRatio = 1.0f, bbool _timeOffsetIsANote = btrue) const;
        f32 getNoteTime(f32 _note) const;

        ITF_INLINE u32 getBarInPart() const { return m_barInPart; }
        ITF_INLINE u32 getBpm() const { return m_bpm; }
        ITF_INLINE u32 getBars() const { return m_nbBars; }
        ITF_INLINE u32 getBeatsPerBar() const { return m_beatsPerBar; }
        ITF_INLINE f32 getOneOverBeatsPerBar() const { return m_oneOverBeatsPerBar; }
        ITF_INLINE u32 getBeatValue() const { return m_beatValue; }
        ITF_INLINE f32 getBeatDuration() const { return m_beatDuration; }
        ITF_INLINE f32 getBarDuration() const { return m_barDuration; }
        ITF_INLINE f32 getTimer() const { return m_timer; }

        ITF_INLINE void pause() { m_playing = bfalse; }
        ITF_INLINE void resume() { m_playing = btrue; }
        ITF_INLINE bbool isPlaying() const { return m_playing; }

        ITF_INLINE f32 getBpm() { return m_barDurationSeconds > 0.f ? ( 60.f * (f32) m_beatsPerBar / m_barDurationSeconds ) : 0.f; }

        virtual void onEvent( Event* _event );

    private:
        f32 m_barPercentage;
        f32 m_timer;
        u32 m_barInPart;
        u32 m_barSinceStart;
        u32 m_nbBars;
        u32 m_beatsPerBar;
        u32 m_beatValue;
        u32 m_bpm;
        f32 m_barDuration;
        f32 m_beatDuration;
        f32 m_oneOverBeatsPerBar;
        bbool m_playing;

        f32 m_barDurationSeconds;

        struct Callback
        {
            Callback(metronomeCallback _cb, f32 _beat, void * _context) :
                cb(_cb),
                beat(_beat),
                context(_context)
            {}

            metronomeCallback cb;
            f32 beat;
            void * context;
        };

        ITF_VECTOR<Callback>m_persistentCallbacks;
        ITF_VECTOR<Callback>m_oneShotCallbacks;
    };

} // namespace ITF

#endif // _ITF_METRONOME_H_
