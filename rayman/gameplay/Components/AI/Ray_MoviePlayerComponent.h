#ifndef _ITF_RAY_MOVIEPLAYERCOMPONENT_H_
#define _ITF_RAY_MOVIEPLAYERCOMPONENT_H_


#ifndef _ITF_MOVIEPLAYERCOMPONENT_H_
#include "engine/actors/components/MoviePlayerComponent.h"
#endif //_ITF_MOVIEPLAYERCOMPONENT_H_

namespace ITF
{
    class Ray_MoviePlayerComponent_Template;
    class Ray_MoviePlayerComponent : public MoviePlayerComponent 
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MoviePlayerComponent, MoviePlayerComponent,1142662804)
        DECLARE_SERIALIZE()

    public:

        Ray_MoviePlayerComponent();
        virtual ~Ray_MoviePlayerComponent();

        virtual void        Update( f32 _dt );
		void		        setListener(IMoviePlayerListener * _listener) { m_moviePlayer.setListener(_listener); }

        virtual void onMovieStarted();
        virtual void onMovieStopped();
        virtual void onMoviePaused();
        virtual void onFadeInEnded();

        enum State
        {
            Idle=0,
            PlayingMovie,
            WaitForNextMovie,
            NormalMode // One video
        };
    private:
        State   m_state;
        f64     m_startTimer;
        f64     m_timeToWait;
        ITF_INLINE const class Ray_MoviePlayerComponent_Template* getTemplate() const;
        
        void playRandomMovie();
        bbool isInputChanged(bbool onlySkipButtons = bfalse);
        ITF_VECTOR<u32> m_movieToPlay;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_MoviePlayerComponent_Template : public MoviePlayerComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MoviePlayerComponent_Template, MoviePlayerComponent_Template,509384819);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_MoviePlayerComponent);

    public:
        Ray_MoviePlayerComponent_Template() 
            : m_startTime(0.0f)
            , m_TimeToWaitBetweenMovies(0.0f)
            , m_canSkipMovie(bfalse)
            , m_TimeToWaitBeforeSkipping(1.0f)
        {
        }
        virtual ~Ray_MoviePlayerComponent_Template() {}

        ITF_INLINE const f32 & getStartTime() const { return m_startTime; }
        ITF_INLINE const f32 & getTimeToWaitBetweenMovies() const { return m_TimeToWaitBetweenMovies; }
        ITF_INLINE const ITF_VECTOR<String> & getMovieList() const { return m_movieList; }
        ITF_INLINE const bbool & getCanSkipMovie() const { return m_canSkipMovie; }
        ITF_INLINE const f32 & getTimeToWaitBeforeSkipping() const { return m_TimeToWaitBeforeSkipping; }
    private:
        f32     m_startTime; 
        f32     m_TimeToWaitBetweenMovies;
        f32     m_TimeToWaitBeforeSkipping;
        ITF_VECTOR<String> m_movieList;        
        bbool   m_canSkipMovie;
    };

    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_MoviePlayerComponent_Template* Ray_MoviePlayerComponent::getTemplate() const
    {
        return static_cast<const Ray_MoviePlayerComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_MOVIEPLAYERCOMPONENT_H_