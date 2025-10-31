#ifndef _ITF_BOXINTERPOLATORCOMPONENT_H_
#define _ITF_BOXINTERPOLATORCOMPONENT_H_

#ifndef _ITF_INTERPOLATORCOMPONENT_H_
#include "gameplay/Components/FX/InterpolatorComponent.h"
#endif //_ITF_INTERPOLATORCOMPONENT_H_

namespace ITF
{
    class BoxInterpolatorComponent : public InterpolatorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(BoxInterpolatorComponent, InterpolatorComponent,4111687898);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        BoxInterpolatorComponent();
        virtual ~BoxInterpolatorComponent();

#ifdef ITF_SUPPORT_EDITOR
        virtual void drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

        virtual bbool canUpdate();
        virtual void Update( f32 _dt );

    protected:

        virtual f32 getInputValue( f32 _dt );

    private:
        ITF_INLINE const class BoxInterpolatorComponent_Template * getTemplate() const;

        f32      m_output;
    };

    class BoxInterpolatorComponent_Template : public InterpolatorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(BoxInterpolatorComponent_Template, InterpolatorComponent_Template,1912108630);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(BoxInterpolatorComponent);

    public:

        BoxInterpolatorComponent_Template();
        ~BoxInterpolatorComponent_Template();

        ITF_INLINE const AABB & getInnerAABB() const { return m_innerAABB; }
        ITF_INLINE const AABB & getOuterAABB() const { return m_outerAABB; }
        ITF_INLINE Angle getAABBRotation() const { return m_AABBRotation; }

    private:

        AABB m_innerAABB;
        AABB m_outerAABB;
        Angle m_AABBRotation;
    };

    ITF_INLINE const class BoxInterpolatorComponent_Template * BoxInterpolatorComponent::getTemplate() const { return static_cast<const class BoxInterpolatorComponent_Template*>(m_template); }
};

#endif //_ITF_BOXINTERPOLATORCOMPONENT_H_