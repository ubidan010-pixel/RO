#ifndef _ITF_CIRCLEINTERPOLATORCOMPONENT_H_
#define _ITF_CIRCLEINTERPOLATORCOMPONENT_H_

#ifndef _ITF_INTERPOLATORCOMPONENT_H_
#include "gameplay/Components/FX/InterpolatorComponent.h"
#endif //_ITF_INTERPOLATORCOMPONENT_H_

namespace ITF
{
    class CircleInterpolatorComponent : public InterpolatorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(CircleInterpolatorComponent, InterpolatorComponent,3129068326);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        CircleInterpolatorComponent();
        virtual ~CircleInterpolatorComponent();

#ifdef ITF_SUPPORT_EDITOR
        virtual void drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    protected:

        virtual f32 getInputValue( f32 _dt );

    private:
        ITF_INLINE const class CircleInterpolatorComponent_Template * getTemplate() const;
    };

    class CircleInterpolatorComponent_Template : public InterpolatorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(CircleInterpolatorComponent_Template, InterpolatorComponent_Template,1785323374);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(CircleInterpolatorComponent);

    public:

        CircleInterpolatorComponent_Template();
        ~CircleInterpolatorComponent_Template();

        ITF_INLINE f32 getInnerRadius() const { return m_innerRadius; }
        ITF_INLINE f32 getOuterRadius() const { return m_outerRadius; }

    private:

        f32 m_innerRadius;
        f32 m_outerRadius;
    };
    ITF_INLINE const class CircleInterpolatorComponent_Template * CircleInterpolatorComponent::getTemplate() const { return static_cast<const class CircleInterpolatorComponent_Template*>(m_template); }
};

#endif //_ITF_CIRCLEINTERPOLATORCOMPONENT_H_