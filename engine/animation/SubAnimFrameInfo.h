#ifndef _ITF_SUBANIMFRAMEINFO_H_
#define _ITF_SUBANIMFRAMEINFO_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif // SERIALIZEROBJECT_H

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

namespace ITF
{
    
    class SubAnim;
    class SubAnimSet;

    class SubAnimFrameInfo
    {
    public:

        DECLARE_SERIALIZE()

        SubAnimFrameInfo(SubAnimSet* _subAnimSet = NULL)
        : m_subAnimSet(_subAnimSet)
        , m_subAnimIndex(-1)
        , m_proceduralCursor(0.f)
        , m_currentPlayRate(1.f)
        , m_currentTime(0.f)
        , m_procedural(bfalse)
        , m_looped(bfalse)
        , m_isReversed(bfalse)
        {};

        SubAnim*                        getSubAnim() const;
        ITF_INLINE f32                  getPlayRate() const                         { return m_currentPlayRate; }
        ITF_INLINE f32                  getCurrentTime() const                      { return m_currentTime; }
        ITF_INLINE f32                  getCursor() const                           { return m_proceduralCursor; }
        ITF_INLINE bbool                getIsProcedural() const                     { return m_procedural; }
        ITF_INLINE bbool                getIsLooped() const                         { return m_looped; }
        ITF_INLINE bbool                getIsReversed() const                       { return m_isReversed; }
        bbool                           isSubAnimFinished() const;
        f32                             getStart() const;

        void                            setSubAnimSet( SubAnimSet* _subAnimSet )    { m_subAnimSet = _subAnimSet;}
        void                            setSubAnimIndex( i32 _index );
        ITF_INLINE void                 setPlayRate( f32 _rate )                    { m_currentPlayRate = _rate; }
        ITF_INLINE void                 setCursor( f32 _cursor )                    { m_proceduralCursor = _cursor; }
        ITF_INLINE void                 setIsProcedural( bbool _procedural )        { m_procedural = _procedural; }
        ITF_INLINE void                 setIsLooped( bbool _looped )                { m_looped = _looped; }
        ITF_INLINE void                 setIsReversed( bbool _reversed )            { m_isReversed = _reversed; }
        void                            setCurTime( f32 _time )                     { m_currentTime = _time; }
        void                            setCurTime(f32 _time, AnimEventContainer* _eventList );
        void                            resetCurTime();
        void                            updateAnimTime( f32 _delta, AnimEventContainer* _eventList );
        f32                             calculateT() const;

    private:
        SubAnimSet*                     m_subAnimSet;
        i32                             m_subAnimIndex;
        f32                             m_proceduralCursor;
        f32                             m_currentPlayRate;
        f32                             m_currentTime;
        bbool                           m_procedural;
        bbool                           m_looped;
        bbool                           m_isReversed;
    };
}

#endif //_ITF_SUBANIMFRAMEINFO_H_
