#ifndef _ITF_RAY_WM_LOADINGSCREENCOMPONENT_H_
#define _ITF_RAY_WM_LOADINGSCREENCOMPONENT_H_

namespace ITF
{
    class Ray_WM_LoadingScreenComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_LoadingScreenComponent, ActorComponent,1852067234)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_WM_LoadingScreenComponent();
        virtual ~Ray_WM_LoadingScreenComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        Update( f32 _dt );



    protected:

    private:
        ITF_INLINE const class Ray_WM_LoadingScreenComponent_Template* getTemplate() const;

        SequencePlayerComponent * getSequence( );
        bbool setSequenceLooping(bbool _enable);

        LinkComponent * m_linkComponent;
        ObjectRef m_world;

        bbool m_playingSequence;
        bbool m_loading;
        bbool m_started;

    };


    //---------------------------------------------------------------------------------------------------

    class Ray_WM_LoadingScreenComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_LoadingScreenComponent_Template, TemplateActorComponent,3735061406);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_LoadingScreenComponent);

    public:

        Ray_WM_LoadingScreenComponent_Template()
        {}
        virtual ~Ray_WM_LoadingScreenComponent_Template() {}


    private:
        
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_WM_LoadingScreenComponent_Template* Ray_WM_LoadingScreenComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_LoadingScreenComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_WM_LOADINGSCREENCOMPONENT_H_
