#ifndef _ITF_RAY_FORCEFIELDCOMPONENT_H_
#define _ITF_RAY_FORCEFIELDCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/GraphicComponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

namespace ITF
{
    class Ray_ForceFieldComponent : public GraphicComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ForceFieldComponent, GraphicComponent,873468901);
        DECLARE_VALIDATE_COMPONENT()
    public:
        DECLARE_SERIALIZE()

        Ray_ForceFieldComponent();
        virtual ~Ray_ForceFieldComponent();

        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return btrue; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        virtual void    Update( f32 _deltaTime );

        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent(Event * _event);
        virtual void    Draw();

        bbool isLocked() const {return m_locked;}

#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_ForceFieldComponent_Template * getTemplate() const;
        class LinkComponent * m_linkComponent;
        class FXControllerComponent * m_fxControllerComponent;
        class AnimLightComponent * m_animComponent;

        f32 getSoftCollisionRadius() const;
        void updateOpen();
        void updateLocked();
        void updateSoftCollisions();
        void startOpen();
        void startLocked();
        void getLocks(SafeArray<Actor*> & _locks);
        bbool isEnemyAlive(Actor *_enemy);
        bbool isEnemyVacuumed(Actor *_enemy);
        bbool m_locked;
        bbool m_firstUpdate;
        struct LinkEvent
        {
            LinkEvent() : m_onActivateForceFieldEvent(NULL), m_onDeactivateForceFieldEvent(NULL){}
            Event * m_onActivateForceFieldEvent;
            Event * m_onDeactivateForceFieldEvent;
        };

        struct LockData
        {
            BezierPatch m_circlePatch;
            BezierPatch m_targetPatch;
            ActorRef m_target;
            f32     m_circleOffset;
            f32     m_targetOffset;
            f32     m_uvStart;
        };

        ITF_VECTOR<LockData> m_lockData;

        ITF_MAP<StringID, LinkEvent> m_linkEvents;
        void clear();
        u32 m_fxHandle;
        ResourceID m_texture;
        f32 m_timer;
        Bezier4<Vec2d> m_circleB;
        Bezier4<Vec2d> m_targetB;
        SafeArray<ActorRef> m_enemyVacuumedList;
        SafeArray<Actor*> m_locksTmp;
        ActorList         m_allActorsTmp;
    };

    //-------------------------------------------------------------------------------------
    class Ray_ForceFieldComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ForceFieldComponent_Template, GraphicComponent_Template,900118042);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ForceFieldComponent);

    public:

        Ray_ForceFieldComponent_Template();
        ~Ray_ForceFieldComponent_Template();

        struct LinkEvent_Template
        {
            DECLARE_SERIALIZE();
            LinkEvent_Template() : m_onActivateForceFieldEvent(NULL), m_onDeactivateForceFieldEvent(NULL){}
            StringID m_tag;
            Event * m_onActivateForceFieldEvent;
            Event * m_onDeactivateForceFieldEvent;
        };

        ITF_INLINE f32 getSoftCollisionRadius() const { return m_softCollisionRadius; }
        ITF_INLINE f32 getBounceRadius() const { return m_bounceRadius; }
        ITF_INLINE const SafeArray<LinkEvent_Template> & getLinkEvents() const { return m_linkEvents; }
        ITF_INLINE f32 getSoftCollisionExitSpeed() const { return m_softCollisionExitSpeed; }
        ITF_INLINE f32 getSoftCollisionExitForce() const { return m_softCollisionExitForce; }
        ITF_INLINE StringID getLockedFx() const { return m_lockedFx; }
        ITF_INLINE f32 getPatchCenterWidth() const { return m_patchCenterWidth; }
        ITF_INLINE f32 getPatchCircleWidth() const { return m_patchCircleWidth; }
        ITF_INLINE f32 getPatchTargetWidth() const { return m_patchTargetWidth; }
        ITF_INLINE Angle getPatchCircleTangeantRotationOffset() const { return m_patchCircleTangeantRotationOffset; }
        ITF_INLINE Angle getPatchTargetTangeantRotationOffset() const { return m_patchTargetTangeantRotationOffset; }
        ITF_INLINE f32 getPatchCircleTangeantRotationFrequency() const { return m_patchCircleTangeantRotationFrequency; }
        ITF_INLINE f32 getPatchTargetTangeantRotationFrequency() const { return m_patchTargetTangeantRotationFrequency; }
        ITF_INLINE const Path & getTextureFile() const { return m_textureFile; }
        ITF_INLINE f32 getPatchTileLength() const { return m_patchTileLength; }
        ITF_INLINE f32 getPatchScrollSpeed() const { return m_patchScrollSpeed; }
        ITF_INLINE StringID getIdleAnim() const { return m_idleAnim; }
        ITF_INLINE StringID getFadeAnim() const { return m_fadeAnim; }

        ITF_INLINE GFX_BLENDMODE getBlendMode() const { return m_blendMode; }
        ITF_INLINE f32 getPatchMidPointOffset() const { return m_patchMidPointOffset; }
        ITF_INLINE f32 getTesselationLength() const { return m_tesselationLength; }
        ITF_INLINE f32 getPatchCenterOffset() const { return m_patchCenterOffset; }
        ITF_INLINE f32 getPatchMidPointPercent() const { return m_patchMidPointPercent; }


    private:
        f32         m_softCollisionRadius;
        f32         m_bounceRadius;
        f32         m_softCollisionExitSpeed;
        f32         m_softCollisionExitForce;
        SafeArray<LinkEvent_Template> m_linkEvents;
        StringID    m_lockedFx;
        f32         m_patchCenterWidth;
        f32         m_patchCircleWidth;
        f32         m_patchTargetWidth;
        Angle       m_patchCircleTangeantRotationOffset;
        Angle       m_patchTargetTangeantRotationOffset;
        f32         m_patchCircleTangeantRotationFrequency;
        f32         m_patchTargetTangeantRotationFrequency;
        Path        m_textureFile;
        f32         m_patchTileLength;
        f32         m_patchScrollSpeed;
        StringID    m_idleAnim;
        StringID    m_fadeAnim;
        f32         m_tesselationLength;
        f32         m_patchMidPointOffset;
        f32         m_patchMidPointPercent;
        f32         m_patchCenterOffset;
        GFX_BLENDMODE m_blendMode;
    };

    ITF_INLINE const class Ray_ForceFieldComponent_Template * Ray_ForceFieldComponent::getTemplate() const
    {
        return static_cast<const class Ray_ForceFieldComponent_Template *>(m_template);
    }

}

#endif // _ITF_RAY_FORCEFIELDCOMPONENT_H_
