#ifndef _ITF_FIXEDCAMERACOMPONENT_H_
#define _ITF_FIXEDCAMERACOMPONENT_H_

#ifndef _ITF_BASECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/BaseCameraComponent.h"
#endif //_ITF_BASECAMERACOMPONENT_H_

namespace ITF
{
    class FixedCameraComponent : public BaseCameraComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(FixedCameraComponent, BaseCameraComponent,1176318211);

    public:
        DECLARE_SERIALIZE()

        FixedCameraComponent();
        virtual ~FixedCameraComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual Vec3d       getCameraPosition() const ;
        virtual f32         getDepth() const;
    protected:
        const class FixedCameraComponent_Template* getTemplate() const;
    };


    class FixedCameraComponent_Template : public BaseCameraComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(FixedCameraComponent_Template,BaseCameraComponent_Template,3300170984);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(FixedCameraComponent);

    public:

        FixedCameraComponent_Template();
        virtual ~FixedCameraComponent_Template();

        f32     getZOffset() const { return m_zOffset; }

    private:
        f32     m_zOffset;
    };

    ITF_INLINE const FixedCameraComponent_Template* FixedCameraComponent::getTemplate() const
    {
        return static_cast<const FixedCameraComponent_Template*>(m_template);
    }

    ITF_INLINE f32 FixedCameraComponent::getDepth() const {return getTemplate()->getZOffset();}

}

#endif // _ITF_FIXEDCAMERACOMPONENT_H_

