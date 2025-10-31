#ifndef _ITF_PROCEDURALSOFTPLATFORMCOMPONENT_H_
#define _ITF_PROCEDURALSOFTPLATFORMCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/graphiccomponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_BEZIERCURVE_H_
#include "engine/gameplay/BezierCurve.h"
#endif // _ITF_BEZIERCURVE_H_

namespace ITF
{
    class ProceduralSoftPlatformComponent : public GraphicComponent
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(ProceduralSoftPlatformComponent,GraphicComponent,2575753601);
        DECLARE_SERIALIZE()

        ProceduralSoftPlatformComponent();
        virtual ~ProceduralSoftPlatformComponent();

        virtual bbool               needsUpdate() const { return btrue; }
        virtual bbool               needsDraw() const { return btrue; }
        virtual bbool               needsDraw2D() const { return bfalse; }
        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);


        virtual void                Update( f32 _deltaTime );

        virtual void                onBecomeActive();
        virtual void                onBecomeInactive();
        virtual void                onEvent( Event * _event );
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( class ActorDrawEditInterface* drawInterface, u32 _flags  ) const;
        virtual void                onEditorMove(bbool _modifyInitialPos = btrue);
        void                        editor_setPos(ITF::Vec3d val);
#endif // ITF_SUPPORT_EDITOR
        virtual void                Draw();
        ITF_INLINE ITF::Vec3d getEndPos() const { return m_endPos; }

    protected:

        void setEndPos(ITF::Vec3d val);
        void init();
        ITF_INLINE const class ProceduralSoftPlatformComponent_Template* getTemplate() const;
        class SoftPlatform * m_softPlatform;
        BezierCurve m_curve;
        BezierCurveRenderer m_bezierRenderer;
        Vec3d m_endPos;
        ResourceID m_beginTextureID;
        ResourceID m_endTextureID;
        GFX_3DQUAD m_beginQuad;
        GFX_3DQUAD m_endQuad;
        bbool m_useBeginTexture;
        bbool m_useEndTexture;
        bbool m_initIteration;
    };

    //---------------------------------------------------------------------------------------------------

    class ProceduralSoftPlatformComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(ProceduralSoftPlatformComponent_Template,GraphicComponent_Template,2432053287);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(ProceduralSoftPlatformComponent);

    public:
        ProceduralSoftPlatformComponent_Template();
        ITF_INLINE ITF::bbool getSwing() const { return m_isSwing; }
        ITF_INLINE const ITF::Path &getGameMaterial() const { return m_gameMaterial; }
        ITF_INLINE ITF::bbool getLiana() const { return m_isLiana; }
        ITF_INLINE f32 getMovingPolyForce() const { return m_movingPolyForce; }
        ITF_INLINE f32 getWeightMultiplier() const { return m_weightMultiplier; }
        ITF_INLINE f32 getLandSpeedMultiplier() const { return m_landSpeedMultiplier; }
        ITF_INLINE f32 getHitForceMultiplier() const { return m_hitForceMultiplier; }
        ITF_INLINE f32 getImpulseMultiplier() const { return m_impulseMultiplier; }
        ITF_INLINE ITF::bbool getPhantom() const { return m_phantom; }
        ITF_INLINE f32 getBodyWindMultiplier() const { return m_bodyWindMultiplier; }
        ITF_INLINE f32 getBodyGravityMultiplier() const { return m_bodyGravityMultiplier; }
        ITF_INLINE ITF::Angle getConstraintMinAngle() const { return m_constraintMinAngle; }
        ITF_INLINE ITF::Angle getConstraintMaxAngle() const { return m_constraintMaxAngle; }
        ITF_INLINE f32 getConstraintMinLength() const { return m_constraintMinLength; }
        ITF_INLINE f32 getConstraintMaxLength() const { return m_constraintMaxLength; }
        ITF_INLINE f32 getConstraintStiff() const { return m_constraintStiff; }
        ITF_INLINE f32 getConstraintDamp() const { return m_constraintDamp; }
        ITF_INLINE ITF::bbool getConstraintLimitAngle() const { return m_constraintLimitAngle; }
        ITF_INLINE ITF::bbool getConstraintRelaxLength() const { return m_constraintRelaxLength; }
        ITF_INLINE const ITF::BezierCurveRenderer_Template & getBezierRenderer() const { return m_bezierRenderer; }
        ITF_INLINE f32 getLengthFactor() const { return m_lengthFactor; }
        ITF_INLINE f32 getEdgeLength() const { return m_edgeLength; }
        ITF_INLINE const Path & getBeginTextureFile() const { return m_beginTextureFile; }
        ITF_INLINE const Path & getEndTextureFile() const { return m_endTextureFile; }
        ITF_INLINE u32 getBezierSampling() const { return m_bezierSampling; }

 
    private:

        bbool m_isSwing;
        bbool m_isLiana;
        Path m_gameMaterial;
        f32 m_movingPolyForce;
        f32 m_weightMultiplier;
        f32 m_landSpeedMultiplier;
        f32 m_hitForceMultiplier;
        f32 m_impulseMultiplier;
        bbool m_phantom;
        f32 m_bodyWindMultiplier;
        f32 m_bodyGravityMultiplier;
        Angle m_constraintMinAngle;
        Angle m_constraintMaxAngle;
        f32   m_constraintMinLength;
        f32   m_constraintMaxLength;
        f32   m_constraintStiff;
        f32   m_constraintDamp;
        bbool m_constraintLimitAngle;
        bbool m_constraintRelaxLength;
        BezierCurveRenderer_Template m_bezierRenderer;
        f32 m_lengthFactor;
        f32 m_edgeLength;
        Path m_beginTextureFile;
        Path m_endTextureFile;
        u32 m_bezierSampling;
    };

    ITF_INLINE const ProceduralSoftPlatformComponent_Template* ProceduralSoftPlatformComponent::getTemplate() const
    {
        return static_cast<const ProceduralSoftPlatformComponent_Template*>(m_template);
    }
}

#endif // _ITF_PROCEDURALProceduralSoftPlatformComponent_H_
