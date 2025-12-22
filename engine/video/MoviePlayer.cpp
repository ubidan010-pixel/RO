#include "precompiled_engine.h"
#include "engine/video/videoHandle.h"


#ifndef ITF_MOVIEPLAYER_H_
#include "engine/video/MoviePlayer.h"
#endif //ITF_MOVIEPLAYER_H_

#ifndef _ITF_VIDEOADAPTER_H_
#include "engine/AdaptersInterfaces/VideoAdapter.h"
#endif //_ITF_VIDEOADAPTER_H_

#ifndef _ITF_MACROS_H_
#include "core/Macros.h"
#endif //_ITF_MACROS_H_

namespace  ITF
{


    void MoviePlayer::play( const Path & _movie, bbool _fromMemory )
    {
#ifndef ITF_DISABLE_VIDEO_EDITOR
        if (!_movie.isEmpty())
        {
            PathString_t pathName;
            _movie.getString(pathName);
            if (pathName == "fresco" || pathName == "fresco_rayman")
            {
                return; // disable these videos for now
            }
            m_videoHandle = VIDEO_ADAPTER->open(pathName,_fromMemory);
            if ( m_videoHandle && m_videoHandle->isOpened() )
            {
                //VIDEO_ADAPTER->registerActor(m_actor->getRef());
                m_videoTotalTime = m_videoHandle->getTotalTime();

                // Pause Game
                if (m_listener)
                    m_listener->onMovieStarted();
            }
            else
            {
                m_videoHandle = NULL;
                if (m_listener)
                    m_listener->onMovieStopped();
            }

            m_moviePlayed = btrue;


            // Alpha required ?
            if ( m_videoHandle != NULL && m_fadeInTime > 0.0f )
            {
                m_fading = btrue;
                m_fadeCurrentTime   = 0.0f;
                m_fadeTotalTime     = m_fadeInTime;
                m_fadeAlphaStart    = 0.0f;
                m_fadeAlphaEnd      = 1.0f;
            }
        }
#else
        m_moviePlayed = btrue;
        if (m_listener)
            m_listener->onMovieStarted();
#endif
    }

    void MoviePlayer::stop()
    {
#ifndef ITF_DISABLE_VIDEO_EDITOR
        if( m_videoHandle != NULL && !m_videoHandle->isStopped() )
        {
            m_videoHandle->stop();
            closeMovie();
        }
#endif
    }

    void MoviePlayer::update( f32 _dt )
    {
#ifndef ITF_DISABLE_VIDEO_EDITOR
        if ( m_videoHandle != NULL )
        {
            f32 videoCurrentTime = 0.0f;

            if ( m_videoHandle->isStopped() )
            {
                closeMovie();
                m_videoHandle = NULL;
            }
            else if( m_videoHandle->getCurrentTime(videoCurrentTime) )
            {
                // ALPHA BLENDING
                if ( m_fading )
                {
                    m_fadeCurrentTime += _dt;
                    f32 alphaRatio = f32_Clamp(m_fadeCurrentTime / m_fadeTotalTime, 0.0f, 1.0f);
                    f32 alpha = m_fadeAlphaStart + alphaRatio * (m_fadeAlphaEnd - m_fadeAlphaStart);
                    if ( m_fadeAlphaStart < m_fadeAlphaEnd )
                        alpha = f32_Clamp(alpha,m_fadeAlphaStart,m_fadeAlphaEnd);
                    else
                        alpha = f32_Clamp(alpha,m_fadeAlphaEnd,m_fadeAlphaStart);

                    m_videoHandle->setAlpha(alpha);

                    // Reached final alpha value ?
                    if ( alpha == m_fadeAlphaEnd )
                    {
                        // Stop fading
                        m_fading = bfalse;
                        m_fadeCurrentTime   = 0.0f;
                        m_fadeTotalTime     = 0.0f;
                        m_fadeAlphaStart    = 0.0f;
                        m_fadeAlphaEnd      = 0.0f;

                        if (m_listener)
                            m_listener->onFadeInEnded();
                    }
                }
                else if ( m_fadeOutTime > 0.0f
                    &&(m_videoTotalTime - videoCurrentTime) <= m_fadeOutTime )
                {
                    m_fading = btrue;
                    m_fadeCurrentTime   = m_fadeOutTime - (m_videoTotalTime - videoCurrentTime);
                    m_fadeTotalTime     = m_fadeOutTime;
                    m_fadeAlphaStart    = 1.0f;
                    m_fadeAlphaEnd      = 0.0f;

                    // unpause Game
                    if (m_listener)
                        m_listener->onMovieStopped();
                }
            }
        }
#endif
    }

    MoviePlayer::MoviePlayer(IMoviePlayerListener * _listener) 
        : m_listener(_listener)
        , m_videoHandle(NULL)
        , m_fading(bfalse)
        , m_fadeTotalTime(0.0f)
        , m_fadeCurrentTime(0.0f)
        , m_fadeAlphaStart(0.0f)
        , m_fadeAlphaEnd(0.0f)
        , m_fadeInTime(0.0f)
        , m_fadeOutTime(0.0f)
        , m_videoTotalTime(0.0f)
        , m_moviePlayed(bfalse)
    {
#ifndef ITF_DISABLE_VIDEO_EDITOR
        m_listener = _listener;
#else
        m_listener = NULL;
#endif
    }

    void MoviePlayer::closeMovie()
    {
#ifndef ITF_DISABLE_VIDEO_EDITOR
        // Close movie file
        VIDEO_ADAPTER->close(m_videoHandle);
        m_videoHandle = NULL;

        // Reset some values
        m_fading = bfalse;
        m_fadeCurrentTime   = 0.0f;
        m_fadeTotalTime     = m_fadeInTime;
        m_fadeAlphaStart    = 0.0f;
        m_fadeAlphaEnd      = 0.0f;

        // unpause Game
        if (m_listener)
            m_listener->onMovieStopped();
        //VIDEO_ADAPTER->unregisterActor();
#endif
    }

    f32 MoviePlayer::getCurrentTime()
    {
#ifndef ITF_DISABLE_VIDEO_EDITOR
        f32 currentTime = 0.0f; 
        if(!m_videoHandle) return currentTime;
        
        m_videoHandle->getCurrentTime(currentTime);
        return currentTime;
#else
        return 0.0f; 
#endif
    }

    void MoviePlayer::pause()
    {
#ifndef ITF_DISABLE_VIDEO_EDITOR
        if(m_videoHandle) 
        {
            m_videoHandle->pause();
            if(m_listener)
                m_listener->onMoviePaused();
        }
#endif
    }

    ITF::bbool MoviePlayer::isPaused() const
    {
#ifndef ITF_DISABLE_VIDEO_EDITOR
        if(m_videoHandle) return m_videoHandle->isPaused(); 
#endif
        return bfalse;
    }

}

