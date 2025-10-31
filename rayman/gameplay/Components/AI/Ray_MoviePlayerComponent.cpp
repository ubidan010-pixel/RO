#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_MOVIEPLAYERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MoviePlayerComponent.h"
#endif //_ITF_RAY_MOVIEPLAYERCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_


namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_MoviePlayerComponent_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_MoviePlayerComponent_Template)
        SERIALIZE_CONTAINER("videolist", m_movieList);
    SERIALIZE_MEMBER("startTime", m_startTime);
    SERIALIZE_MEMBER("timeToWaitBetweenMovies",m_TimeToWaitBetweenMovies);
    SERIALIZE_MEMBER("canSkipMovie",m_canSkipMovie);
    SERIALIZE_MEMBER("timeToWaitBeforeSkipping", m_TimeToWaitBeforeSkipping);
    END_SERIALIZATION()


        //-------------------------------------------------------------------------------------

        IMPLEMENT_OBJECT_RTTI(Ray_MoviePlayerComponent)
        BEGIN_SERIALIZATION_CHILD(Ray_MoviePlayerComponent)
        END_SERIALIZATION()


        Ray_MoviePlayerComponent::Ray_MoviePlayerComponent(): m_state(Idle)
        , m_startTimer(0.0)
        , m_timeToWait(0.0)
        , MoviePlayerComponent()
    {
    }

    Ray_MoviePlayerComponent::~Ray_MoviePlayerComponent()
    {

    }

    void Ray_MoviePlayerComponent::onMovieStarted()
    {
        if (isPlayingMovie())
        {
            // Pause Game
            //RAY_GAMEMANAGER->requestGameplayPause();
            RAY_GAMEMANAGER->onStartOfPlayingVideo();
        } 
    }

    void Ray_MoviePlayerComponent::onMovieStopped()
    {
        // unpause Game
        //RAY_GAMEMANAGER->requestGameplayUnpause();
        RAY_GAMEMANAGER->onEndOfPlayingVideo();
    }

    void Ray_MoviePlayerComponent::onMoviePaused()
    {
        RAY_GAMEMANAGER->onPauseOfPlayingVideo();
    }

    void Ray_MoviePlayerComponent::onFadeInEnded()
    {
        RAY_GAMEMANAGER->onEndOfFadeIn();
    }

    void Ray_MoviePlayerComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if(!getTemplate()) return ;

        switch (m_state)
        {
        case Idle:
            if(getTemplate()->getStartTime() == 0.0f || !getTemplate()->getVideoPath().isEmpty())    
                m_state = NormalMode;
            else 
            {
                m_state = WaitForNextMovie;
                m_timeToWait = getTemplate()->getStartTime();
            }
            break;
        case PlayingMovie:
            if(isInputChanged())
            {
                stopMovie();
                m_state = WaitForNextMovie;
                m_timeToWait = getTemplate()->getStartTime();
            }
            else if(!isPlayingMovie())
            {
                m_state = WaitForNextMovie;
                m_timeToWait = getTemplate()->getTimeToWaitBetweenMovies();
            } 
            break;
        case WaitForNextMovie:
            if(isInputChanged() || !GAMEMANAGER->canOpenMovie())
            {
                m_startTimer = 0.0;
                m_timeToWait = getTemplate()->getStartTime();
            }

            if(m_startTimer==0.0)
                m_startTimer = SYSTEM_ADAPTER->getTime();

            if(SYSTEM_ADAPTER->getTime() - m_startTimer > m_timeToWait)
            {
                m_state = PlayingMovie;
                m_startTimer = 0.0;
                playRandomMovie();            
            }
            break;
        case NormalMode:
            if(getTemplate()->getCanSkipMovie() &&
                isPlayingMovie() &&
                getCurrentTime() > getTemplate()->getTimeToWaitBeforeSkipping() && 
                isInputChanged(btrue))
            {
                stopMovie();
            }

        default:
            break;
        }
    }

    bbool Ray_MoviePlayerComponent::isInputChanged(bbool onlySkipButtons)
    {
        // Get values from INPUT_ADAPTER instead of code again the same code .. 
        InputAdapter::PressStatus buts[JOY_MAX_BUT];
        f32 axes[JOY_MAX_AXES];
        u32 i = 0 ;
        u32 maxCount = JOY_MAX_COUNT;

        if(onlySkipButtons)
        {
            i = GAMEMANAGER->getMainIndexPlayer();
            maxCount = i+1;
        }
        else
        {
#ifdef ITF_WII
            maxCount = 1;   // only main can skip
#endif //ITF_WII
        }

        for ( ; i < maxCount; i++)
        {
            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentAll, i, buts, JOY_MAX_BUT);

            if(!onlySkipButtons)
            {
                for (u32 but = 0; but < JOY_MAX_BUT; but++)
                {
                    if (buts[but] == InputAdapter::JustReleased && but != m_joyButton_Home) 
                    {
                        return btrue;
                    }
                }


                INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentAll, i, axes, JOY_MAX_AXES);
                for (u32 axe = 0; axe < JOY_MAX_AXES; axe++)
                {
                    if (f32_Abs(axes[axe]) > 0.5f) return btrue;
                }
                // Only "official skip button"
            }  else
            {
                if(buts[m_joyButton_Start] == InputAdapter::JustReleased)
                {
                    return btrue;
                }
            }
        }        
        return bfalse;
    }

    void Ray_MoviePlayerComponent::playRandomMovie()
    {
        if(m_movieToPlay.size() == 0)
        {
            for (u32 i = 0; i < getTemplate()->getMovieList().size(); i++)
                m_movieToPlay.push_back(i);

            if(m_movieToPlay.size() == 0)
            {
                m_state = NormalMode;
                return;
            }
        }

        Seeder randomGen; 
        u32 randomIndex  = randomGen.GetU32(0, m_movieToPlay.size()-1);
        u32 indexToPlay = m_movieToPlay [randomIndex];

        ITF_VECTOR<u32>::iterator it = m_movieToPlay.begin(); 
        for (; it != m_movieToPlay.end(); ++it)
        {
            if ( (*it) == indexToPlay )
            {
                m_movieToPlay.erase ( it );
                break;
            }
        }

        m_moviePlayer.play( getTemplate()->getMovieList()[indexToPlay],getTemplate()->getPlayFromMemory() );
    }
};
