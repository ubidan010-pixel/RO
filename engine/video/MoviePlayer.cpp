#include "precompiled_engine.h"
#include "engine/video/videoHandle.h"


#ifndef ITF_MOVIEPLAYER_H_
#include "engine/video/MoviePlayer.h"
#endif //ITF_MOVIEPLAYER_H_

#ifndef _ITF_VIDEOADAPTER_H_
#include "engine/AdaptersInterfaces/VideoAdapter.h"
#endif //_ITF_VIDEOADAPTER_H_

namespace  ITF
{


    void MoviePlayer::play( const Path & _movie, bbool _fromMemory )
    {
        if (!_movie.isEmpty())
        {
            PathString_t pathName;
            _movie.getString(pathName);
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
    }

    void MoviePlayer::stop()
    {
        if( m_videoHandle != NULL && !m_videoHandle->isStopped() )
        {
            m_videoHandle->stop();
            closeMovie();
        }
    }

    void MoviePlayer::update( f32 _dt )
    {
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

    }

    void MoviePlayer::closeMovie()
    {
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
    }

    f32 MoviePlayer::getCurrentTime()
    {
        f32 currentTime = 0.0f; 
        if(!m_videoHandle) return currentTime;
        
        m_videoHandle->getCurrentTime(currentTime);
        return currentTime;
    }

    void MoviePlayer::pause()
    {
        if(m_videoHandle) 
        {
            m_videoHandle->pause();
            if(m_listener)
                m_listener->onMoviePaused();
        }
    }

    ITF::bbool MoviePlayer::isPaused() const
    {
        if(m_videoHandle) return m_videoHandle->isPaused(); return bfalse;
    }

}

