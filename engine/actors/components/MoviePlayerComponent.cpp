#include "precompiled_engine.h"

#ifndef _ITF_MOVIEPLAYERCOMPONENT_H_
#include "engine/actors/components/MoviePlayerComponent.h"
#endif //_ITF_MOVIEPLAYERCOMPONENT_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(MoviePlayerComponent_Template)

    BEGIN_SERIALIZATION(MoviePlayerComponent_Template)
        SERIALIZE_MEMBER("video", m_video);
        SERIALIZE_MEMBER("autoPlay", m_autoPlay);
        SERIALIZE_MEMBER("fadeInTime", m_fadeInTime);
        SERIALIZE_MEMBER("fadeOutTime", m_fadeOutTime);
        SERIALIZE_MEMBER("playFromMemory", m_playFromMemory);
        //SERIALIZE_MEMBER("pauseBeforeStop", m_pauseBeforeStop);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(MoviePlayerComponent)

    BEGIN_SERIALIZATION_CHILD(MoviePlayerComponent)
    END_SERIALIZATION()

    MoviePlayerComponent::MoviePlayerComponent()
    : m_currentFrame(U32_INVALID)
    {
        m_moviePlayer.setListener(this);
    }

    MoviePlayerComponent::~MoviePlayerComponent()
    {
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventPlayMovie,797811199), this);
    }


    void MoviePlayerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_moviePlayer.setFadeInTime(getTemplate()->getFadeInTime());
        m_moviePlayer.setFadeOutTime(getTemplate()->getFadeOutTime());

        // Register events
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventPlayMovie,797811199),this);
    }

    void MoviePlayerComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        if ( !GAMEMANAGER->isLoadingVisible() && getTemplate()->getAutoPlay() && !m_moviePlayer.getMoviePlayed() )
        {
            m_moviePlayer.play(getTemplate()->getVideoPath(),getTemplate()->getPlayFromMemory());
        }
    }



    void MoviePlayerComponent::Update( f32 _dt )
    {
        if ( m_currentFrame == CURRENTFRAME )
            return;
        
        m_currentFrame = CURRENTFRAME;

        Super::Update(_dt);
        

        if (m_moviePlayer.isPlayingMovie())
        {
            m_moviePlayer.update(_dt);
        }
        else if ( !GAMEMANAGER->isLoadingVisible() && getTemplate()->getAutoPlay() && !m_moviePlayer.getMoviePlayed() )
        {
            playMovie();
        }
    }

    void MoviePlayerComponent::playMovie()
    {
        m_moviePlayer.play(getTemplate()->getVideoPath(),getTemplate()->getPlayFromMemory());
    }

    void MoviePlayerComponent::stopMovie(bbool force)
    {
        if( getTemplate()->getPauseBeforeStop() && !force)
        {
            m_moviePlayer.pause();                
        }
        else
            m_moviePlayer.stop();
    }

    void MoviePlayerComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);


        if (EventPlayMovie* playMovieEvent = _event->DynamicCast<EventPlayMovie>(ITF_GET_STRINGID_CRC(EventPlayMovie,797811199)))
        {
            if(playMovieEvent->getPlay())
            {
                 playMovie();
            } else
            {
                stopMovie(btrue);
            }
        }
    }

};