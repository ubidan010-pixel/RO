#ifndef _ITF_RAY_ELEVATORWHEELAICOMPONENT_H_
#define _ITF_RAY_ELEVATORWHEELAICOMPONENT_H_

namespace ITF
{

    class AnimLightComponent;

    class Ray_ElevatorWheelAIComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ElevatorWheelAIComponent, ActorComponent,3471739185)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ElevatorWheelAIComponent();
        virtual ~Ray_ElevatorWheelAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event * _event );
        virtual void        Update( f32 _dt );

    private:

                void        sendForce(u32 _level) const;

    private:
        ITF_INLINE const class Ray_ElevatorWheelAIComponent_Template* getTemplate() const;

        AnimLightComponent*     m_animLightComponent;
        bbool                   m_requestDisable;

    };


    //*****************************************************************************
    // Template
    //*****************************************************************************
	
    class Ray_ElevatorWheelAIComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ElevatorWheelAIComponent_Template, TemplateActorComponent,1816481909);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ElevatorWheelAIComponent);

    public:

        Ray_ElevatorWheelAIComponent_Template();
        virtual ~Ray_ElevatorWheelAIComponent_Template();

    private:

    };

    //*****************************************************************************

    ITF_INLINE const Ray_ElevatorWheelAIComponent_Template* Ray_ElevatorWheelAIComponent::getTemplate() const
    {
        return static_cast<const Ray_ElevatorWheelAIComponent_Template*>(m_template);
    }
	
}; // end namespace

#endif //_ITF_RAY_ELEVATORWHEELAICOMPONENT_H_