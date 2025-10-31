#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_


#ifndef _ITF_SUBANIMFRAMEINFO_H_
#include "engine/animation/SubAnimFrameInfo.h"
#endif //_ITF_SUBANIMFRAMEINFO_H_

#ifndef _ITF_SUBANIMLIGHTSET_H_
#include    "engine/animation/SubAnimSet.h"
#endif //_ITF_SUBANIMLIGHTSET_H_

#ifndef _ITF_METRONOMEMANAGER_H_
#include "engine/sound/MetronomeManager.h"
#endif // _ITF_METRONOMEMANAGER_H_

namespace ITF
{

    BEGIN_SERIALIZATION(SubAnimFrameInfo)
        SERIALIZE_MEMBER("subAnimIndex",m_subAnimIndex);
        SERIALIZE_MEMBER("proceduralCursor",m_proceduralCursor);
        SERIALIZE_MEMBER("currentPlayRate",m_currentPlayRate);
        SERIALIZE_MEMBER("currentTime",m_currentTime);
        SERIALIZE_MEMBER("procedural",m_procedural);
        SERIALIZE_MEMBER("looped",m_looped);
    END_SERIALIZATION()

    //////////////////////////////////////////////////////////////////////////
    void SubAnimFrameInfo::setCurTime(f32 _time, AnimEventContainer* _eventList )
                             //////////////////////////////////////////////////////////////////////////
    {
        f32 oldFrame = timeToFrame(m_currentTime,ANIMATION_FRAMERATE);
        m_currentTime = _time;
        f32 curFrame = timeToFrame(m_currentTime,ANIMATION_FRAMERATE);
 
        SubAnim* subAnim = getSubAnim();
        f32 animDuration = subAnim->getDuration();
        f32 start = subAnim->getStart();
        f32 startFrame  = timeToFrame(start,ANIMATION_FRAMERATE);
        f32 stop        = subAnim->getStop();
        f32 stopFrame   = timeToFrame(stop,ANIMATION_FRAMERATE);
        f32 frameEvent1[2]  = {-1.f, -1.f};
        f32 frameEvent2[2]  = {-1.f, -1.f};

        //forward
        if ( !getIsReversed() && curFrame >= stopFrame )
        {
            if ( getIsLooped() )
            {
                //old to end
                frameEvent1[0]  = oldFrame;
                frameEvent1[1]  = stopFrame;
                if (m_currentTime > stop)
                    m_currentTime = f32_Modulo(m_currentTime - start, animDuration) + start;
                else
                    m_currentTime = start;

                //beginning to current 
                frameEvent2[0] = startFrame;
                frameEvent2[1] = timeToFrame(m_currentTime,ANIMATION_FRAMERATE);
            }
            else
            {
                m_currentTime = stop;
                //old to end of animation
                frameEvent1[0]  = oldFrame;
                frameEvent1[1]  = stopFrame;
            }
        }
        else if (getIsReversed() && curFrame <= startFrame )
        {
            if ( getIsLooped() )
            {
                //old to beginning of anim , reversed
                frameEvent1[0]  = startFrame;
                frameEvent1[1]  = oldFrame;

                if (m_currentTime < start)
                    m_currentTime = f32_Modulo(m_currentTime + animDuration*1024.f - start, animDuration) + start;
                else
                    m_currentTime = stop;
                // end of anim to current reversed
                frameEvent2[0] = timeToFrame(m_currentTime,ANIMATION_FRAMERATE);
                frameEvent2[1] = stopFrame;
            }
            else
            {
                m_currentTime = start;
                //old to beginning of anim , reversed
                frameEvent1[0] = startFrame;
                frameEvent1[1] = oldFrame;
            }
        }
        else
        {
            if (!getIsReversed())
            {
                if (oldFrame < curFrame)
                {
                    frameEvent1[0] = oldFrame;
                    frameEvent1[1] = curFrame;
                }
                else if (oldFrame > curFrame)
                {               
                    //old to end
                    frameEvent1[0] = oldFrame;
                    frameEvent1[1] = stopFrame;
                    frameEvent2[0] = startFrame;
                    frameEvent2[1] = curFrame;
                }
            }
            else
            {
                if (oldFrame > curFrame)
                {
                    frameEvent1[0] = curFrame;
                    frameEvent1[1] = oldFrame;
                }
                else if (curFrame > oldFrame)
                {               
                    //old to end
                    frameEvent1[0] = startFrame;
                    frameEvent1[1] = oldFrame;
                    frameEvent2[0] = curFrame;
                    frameEvent2[1] = stopFrame;
                }
            }
        }

        if (_eventList)
        {
            AnimTrack* track = subAnim->getTrack();

            if (frameEvent1[0] >= 0.f && frameEvent1[1] >= 0.f)
                track->GetEvents(frameEvent1[0], frameEvent1[1], *_eventList);
            if (frameEvent2[0] >= 0.f && frameEvent2[1] >= 0.f)
                track->GetEvents(frameEvent2[0], frameEvent2[1], *_eventList);

            u32 numEvents = _eventList->size();
            const StringID& friendlyName = subAnim->getFriendlyName();

            for ( u32 i = 0; i < numEvents; i++ )
            {
                (*_eventList)[i]->setFriendly(friendlyName);
            }

        }
    }

    void SubAnimFrameInfo::updateAnimTime( f32 _delta, AnimEventContainer* _eventList )
    {
        SubAnim* subAnim = getSubAnim();
        if (!m_procedural)
        {
            float animDelta = _delta * m_currentPlayRate * subAnim->getPlayRate();

            if ( getIsReversed() )
            {
                setCurTime(m_currentTime-animDelta, _eventList);
            }
            else
            {
                setCurTime(m_currentTime+animDelta, _eventList);
            }
        }
        else
        {
            if (subAnim->getIsSync())
            {
                f32 syncOffset  = subAnim->getSyncEigthNote() * Metronome::EighthNote *
                                  METRONOME_MANAGER->getOneOverBeatsPerBar(subAnim->getMetronome());  // normalize
                    syncOffset += subAnim->getSyncOffset();
                f32 time        = METRONOME_MANAGER->getBarPercentage(syncOffset, subAnim->getSyncRatio(),
                                                                      bfalse, subAnim->getMetronome()) * subAnim->getDuration();
                setCurTime(time + subAnim->getStart(), _eventList);
            }
            else
            {
                f32 time        = m_proceduralCursor * subAnim->getDuration();
                setCurTime(time + subAnim->getStart(), _eventList);
            }
        }
    }

    f32 SubAnimFrameInfo::getStart() const
    {
        if ( getIsReversed() )
        {
            return getSubAnim()->getStop();
        }
        else
        {
            return getSubAnim()->getStart();
        }
    }

    void SubAnimFrameInfo::resetCurTime()
    {
        m_currentTime = getStart();
    }

    //////////////////////////////////////////////////////////////////////////
    bbool SubAnimFrameInfo::isSubAnimFinished() const
    //////////////////////////////////////////////////////////////////////////
    {
        SubAnim* subAnim = getSubAnim();
        // warning is sent by the AnimLight instead so we have the actor name
        //ITF_WARNING(NULL, !subAnim->getIsLooped(), "Testing isSubAnimFinished on looping animation: %s / %s", subAnim->getFriendlyName().getDebugString(), subAnim->getTrackName().getDebugString());
        bbool isFinished;
        if ( getIsReversed() )
        {
            isFinished = timeToFrame(m_currentTime,ANIMATION_FRAMERATE) <= timeToFrame(subAnim->getStart(),ANIMATION_FRAMERATE);
        }
        else
        {
            isFinished = timeToFrame(m_currentTime,ANIMATION_FRAMERATE) >= timeToFrame(subAnim->getStop(),ANIMATION_FRAMERATE);
        }

        return isFinished;
    }

    f32 SubAnimFrameInfo::calculateT() const
    {
        SubAnim* subAnim = getSubAnim();
        f32 t = f32_Abs( getCurrentTime() - getStart() ) / subAnim->getDuration();

        return t;
    }

    void SubAnimFrameInfo::setSubAnimIndex( i32 _index )
    {
        SubAnim* _subAnim = m_subAnimSet ? m_subAnimSet->getSubAnim(_index) : NULL;

        m_subAnimIndex = _index;

        if(_subAnim)
        {
            m_procedural = _subAnim->getIsProcedural();
            m_looped = _subAnim->getIsLooped();
            m_currentPlayRate = _subAnim->getPlayRate();
            m_isReversed = _subAnim->getIsReversed();

            resetCurTime();
        }
    }

    SubAnim* SubAnimFrameInfo::getSubAnim() const
    {
        return m_subAnimSet && m_subAnimIndex >= 0 ? m_subAnimSet->getSubAnim(m_subAnimIndex) : NULL;
    }

} // namespace ITF