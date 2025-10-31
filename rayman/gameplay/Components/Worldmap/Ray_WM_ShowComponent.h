#ifndef _ITF_RAYWMSHOWCOMPONENT_H_
#define _ITF_RAYWMSHOWCOMPONENT_H_

namespace ITF
{
    class Ray_WM_ShowComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_ShowComponent, ActorComponent, 2143071717)
        DECLARE_SERIALIZE()

    public:

        Ray_WM_ShowComponent()
            : m_animLightComponent(NULL)
        {};
        virtual ~Ray_WM_ShowComponent(){};

        virtual bbool       needsUpdate()   const { return bfalse; }
        virtual bbool       needsDraw()     const { return bfalse; }
        virtual bbool       needsDraw2D()   const { return bfalse; }

        virtual void        onEvent(Event * _event);

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onActorClearComponents();

    private:

        ITF_INLINE const class Ray_WM_ShowComponent_Template* getTemplate() const;

        void                show( bbool _show );

        AnimLightComponent*      m_animLightComponent;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_WM_ShowComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_ShowComponent_Template, TemplateActorComponent,1699972322);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_ShowComponent);

    public:

        Ray_WM_ShowComponent_Template(){}
        virtual ~Ray_WM_ShowComponent_Template() {}

    private:
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_WM_ShowComponent_Template* Ray_WM_ShowComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_ShowComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAYWMSHOWCOMPONENT_H_