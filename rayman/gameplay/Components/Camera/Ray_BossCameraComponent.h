#ifndef _ITF_RAY_BOSSCAMERACOMPONENT_H_
#define _ITF_RAY_BOSSCAMERACOMPONENT_H_

#ifndef _ITF_BASECAMERACOMPONENT_H_
#include "engine/actors/components/Camera/BaseCameraComponent.h"
#endif //_ITF_BASECAMERACOMPONENT_H_

#ifndef _ITF_CURVE_H_
#include "engine/gameplay/Curve.h"
#endif //_ITF_CURVE_H_

namespace ITF
{
    class Ray_BossCameraComponent : public BaseCameraComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossCameraComponent, BaseCameraComponent,3307596908)
        DECLARE_SERIALIZE()

    public:

        Ray_BossCameraComponent();
        virtual ~Ray_BossCameraComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onCheckpointLoaded();
        virtual void        onResourceReady();
        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event );
        virtual void        onCameraEvent( Event* _event );

        virtual Vec3d       getCameraPosition() const;
        virtual f32         getDepth() const{return m_zOffset;}

    private:

        ITF_INLINE const class Ray_BossCameraComponent_Template* getTemplate() const;

        void                processNodeReached( Ray_EventNodeReached* _nodeReached );

        AnimLightComponent* m_animComponent;
        BoneID  m_attachBone;


        LogisticParams m_zOffsetCurve;
        f32     m_zOffsetCurveT;
        f32     m_zOffsetCurveDuration;
        f32     m_zOffset;

        LogisticParams m_offsetCurve;
        f32     m_offsetCurveT;
        f32     m_offsetCurveDuration;
        Vec2d   m_offsetCurvePrev;
        Vec2d   m_offsetCurveDir;
        Vec2d   m_offset;
    };

    //-------------------------------------------------------------------------------------
    class Ray_BossCameraComponent_Template : public BaseCameraComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossCameraComponent_Template,BaseCameraComponent_Template,2656542915);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BossCameraComponent);

    public:

        Ray_BossCameraComponent_Template();
        ~Ray_BossCameraComponent_Template() {}

        const StringID&           getAttachBone() const { return m_attachBone; }

    private:

        StringID  m_attachBone;
    };

    ITF_INLINE const Ray_BossCameraComponent_Template* Ray_BossCameraComponent::getTemplate() const
    {
        return static_cast<const Ray_BossCameraComponent_Template*>(m_template);
    }
}

#endif // _ITF_RAY_BOSSCAMERACOMPONENT_H_

