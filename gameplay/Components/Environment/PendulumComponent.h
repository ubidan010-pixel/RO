
#ifndef _ITF_PENDULUMCOMPONENT_H_
#define _ITF_PENDULUMCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class PendulumComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PendulumComponent,ActorComponent,3026488334);

    public:

        DECLARE_SERIALIZE()

        PendulumComponent();
        virtual ~PendulumComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _deltaTime );

    protected:

        ITF_INLINE const class PendulumComponent_Template*  getTemplate() const;

        f32                 m_angle;
        f32                 m_angularVel;
    };

    //---------------------------------------------------------------------------------------------------

    class PendulumComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PendulumComponent_Template,TemplateActorComponent,1909095444);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PendulumComponent);

    public:

        PendulumComponent_Template();
        ~PendulumComponent_Template() {}


        ITF_INLINE f32          getLength() const {return  m_length;}
        ITF_INLINE f32          getInitialAngle() const {return m_initialAngle;}
        ITF_INLINE f32          getInitialAngularVelocity()  const {return m_initialAngularVelocity;}
        ITF_INLINE f32          getGravityMultiplier()  const {return m_gravityMultiplier;}

    private:

        f32                 m_length;
        f32                 m_initialAngle;
        f32                 m_initialAngularVelocity;
        f32                 m_gravityMultiplier;
    };

    const PendulumComponent_Template*  PendulumComponent::getTemplate() const {return static_cast<const PendulumComponent_Template*>(m_template);}


}

#endif // _ITF_PENDULUMCOMPONENT_H_
