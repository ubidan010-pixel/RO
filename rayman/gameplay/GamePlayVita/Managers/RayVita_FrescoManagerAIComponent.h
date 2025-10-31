#ifndef _ITF_RayVita_FrescoManagerAIComponent_H_
#define _ITF_RayVita_FrescoManagerAIComponent_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef ITF_MOVIEPLAYER_H_
#include "engine/video/MoviePlayer.h"
#endif //ITF_MOVIEPLAYER_H_

namespace ITF
{

    class LinkComponent;
	class Ray_MoviePlayerComponent;
	class IMoviePlayerListener;

	class RayVita_FrescoManagerAIComponent : public Ray_AIComponent, public IMoviePlayerListener
	{
		DECLARE_OBJECT_CHILD_RTTI(RayVita_FrescoManagerAIComponent, Ray_AIComponent,1859524894);
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

    public:

        RayVita_FrescoManagerAIComponent();
        virtual ~RayVita_FrescoManagerAIComponent();

        virtual bbool   needsUpdate() const     { return btrue; }
        virtual bbool   needsDraw() const       { return bfalse; }
        virtual bbool   needsDraw2D() const     { return bfalse; }

        virtual void    onBecomeActive();

        virtual void    Update( f32 _deltaTime );
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

		virtual void    onMovieStarted();
		virtual void    onMovieStopped();
		virtual void    onMoviePaused(){}
		virtual void    onFadeInEnded(){}
    private:

        ITF_INLINE const class RayVita_FrescoManagerAIComponent_Template*  getTemplate() const;

        void init();
		void startPlayingVideo();
		void stopPlayingVideo();
    private:
     
        LinkComponent*     m_linkComponent;
        bbool    m_isInit;
      
    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class RayVita_FrescoManagerAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(RayVita_FrescoManagerAIComponent_Template, Ray_AIComponent_Template,1672407068);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(RayVita_FrescoManagerAIComponent);

        ITF_INLINE const u32   getFrescoId() const    { return m_frescoId; }

    public:

        RayVita_FrescoManagerAIComponent_Template();
        ~RayVita_FrescoManagerAIComponent_Template();

    private:
    
        u32 m_frescoId;
    };

    const RayVita_FrescoManagerAIComponent_Template*  RayVita_FrescoManagerAIComponent::getTemplate() const {return static_cast<const RayVita_FrescoManagerAIComponent_Template*>(m_template);}
}

#endif // _ITF_RayVita_FrescoManagerAIComponent_H_