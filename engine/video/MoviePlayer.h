#ifndef ITF_MOVIEPLAYER_H_
#define ITF_MOVIEPLAYER_H_


namespace ITF
{


    class IMoviePlayerListener
    {
    public:
        virtual void onMovieStarted() = 0;
        virtual void onMovieStopped() = 0;
        virtual void onMoviePaused() = 0;
        virtual void onFadeInEnded() = 0;
    };

    class MoviePlayer
    {
    public:
        MoviePlayer(IMoviePlayerListener * _listener = NULL);
        ITF_INLINE f32 getFadeInTime() const { return m_fadeInTime; }
        ITF_INLINE void setFadeInTime(f32 _val) { m_fadeInTime = _val; }
        ITF_INLINE f32 getFadeOutTime() const { return m_fadeOutTime; }
        ITF_INLINE void setFadeOutTime(f32 _val) { m_fadeOutTime = _val; }
        ITF_INLINE bbool isPlayingMovie() const { return m_videoHandle != NULL; }
        ITF_INLINE bbool getMoviePlayed() const { return m_moviePlayed; }

        void play(const Path & _movie, bbool _fromMemory);
        void stop();
        void pause();
        bbool isPaused()const;
        void update(f32 _dt);

        void setListener(IMoviePlayerListener * _listener) { m_listener = _listener; }
        f32  getCurrentTime();
    private:
        void closeMovie();
        f32                         m_videoTotalTime;
        class videoHandle*          m_videoHandle;
        bbool                       m_fading;
        f32                         m_fadeTotalTime;
        f32                         m_fadeCurrentTime;
        f32                         m_fadeAlphaStart;
        f32                         m_fadeAlphaEnd;

        f32                         m_fadeInTime;
        f32                         m_fadeOutTime;


        bbool                       m_moviePlayed;

        IMoviePlayerListener *      m_listener;
    };

};


#endif //ITF_MOVIEPLAYER_H_
