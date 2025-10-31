#ifndef _ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_
#define _ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_

#ifndef _ITF_GRAPHICCOMPONENT_H_
#include "engine/actors/components/GraphicComponent.h"
#endif //_ITF_GRAPHICCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

namespace ITF
{
    class AnimatedComponent;

    class Ray_ActivationSequenceVisualComponent : public GraphicComponent, IUpdateAnimInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ActivationSequenceVisualComponent, GraphicComponent,3789500893)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ActivationSequenceVisualComponent();
        virtual ~Ray_ActivationSequenceVisualComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return btrue; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        Draw();
        virtual void        onEvent( Event * _event);
        virtual void        updateAnimInput();

                bbool       isActivated() const { return m_user != ITF_INVALID_OBJREF && m_activateTime == m_activateTimeTotal; }
                ActorRef    getStone() const { return m_stoneRef; }
                ActorRef    getUser() const { return m_user; }
                bbool       isDisabled() const { return m_isDisabled; }

                void        setOccupiedState( ActorRef _stoneRef, ActorRef _user, f32 _activateTimeTotal );
                void        setActivationFinished() { m_activationFinished = btrue; }
                void        setDisabled(bbool _value) { m_isDisabled = _value; }

    private:
        void                processQueryEmpty( Ray_EventQueryEmptyStone* _queryEmpty );

    private:
        ITF_INLINE const class Ray_ActivationSequenceVisualComponent_Template* getTemplate() const;

        void                updatePatch( f32 _dt );

        AnimatedComponent*  m_animComponent;

        // link patch data
        Vec2d               m_linkPatchTarget;
        Vec2d               m_linkPatchStart;
        f32                 m_startOffset;
        f32                 m_targetOffset;
        f32                 m_uvStart;
        ResourceID          m_texture;
        BezierPatch         m_targetPatch;

        ActorRef            m_stoneRef;
        ActorRef            m_user;
        ActorRef            m_prevUser;
        f32                 m_activateTime;
        f32                 m_activateTimeTotal;
        f32                 m_timer;
        bbool               m_activationFinished;
        bbool               m_drawPatch;

        bbool               m_isDisabled;
    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_ActivationSequenceVisualComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ActivationSequenceVisualComponent_Template, GraphicComponent_Template,25680642);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ActivationSequenceVisualComponent);

    public:

        Ray_ActivationSequenceVisualComponent_Template();
        virtual ~Ray_ActivationSequenceVisualComponent_Template();

        ITF_INLINE f32          getPatchStartRadius() const { return m_patchStartRadius; }
        ITF_INLINE f32          getPatchTileLength() const { return m_patchTileLength; }
        ITF_INLINE f32          getPatchScrollSpeed() const { return m_patchScrollSpeed; }
        ITF_INLINE f32          getPatchStartWidth() const { return m_patchStartWidth; }
        ITF_INLINE f32          getPatchTargetWidth() const { return m_patchTargetWidth; }
        ITF_INLINE Angle        getPatchStartTangeantRotationOffset() const { return m_patchStartTangeantRotationOffset; }
        ITF_INLINE f32          getPatchStartTangeantRotationFrequency() const { return m_patchStartTangeantRotationFrequency; }
        ITF_INLINE Angle        getPatchTargetTangeantRotationOffset() const { return m_patchTargetTangeantRotationOffset; }
        ITF_INLINE f32          getPatchTargetTangeantRotationFrequency() const { return m_patchTargetTangeantRotationFrequency; }
        ITF_INLINE GFX_BLENDMODE getPatchBlendMode() const { return m_patchBlendMode; }
        ITF_INLINE const Path&  getPatchTextureFile() const { return m_patchTextureFile; }

    private:

        f32                     m_patchStartRadius;
        f32                     m_patchTileLength;
        f32                     m_patchScrollSpeed;
        f32                     m_patchStartWidth;
        f32                     m_patchTargetWidth;
        Angle                   m_patchStartTangeantRotationOffset;
        f32                     m_patchStartTangeantRotationFrequency;
        Angle                   m_patchTargetTangeantRotationOffset;
        f32                     m_patchTargetTangeantRotationFrequency;
        GFX_BLENDMODE           m_patchBlendMode;
        Path                    m_patchTextureFile;
    };

    //*****************************************************************************

    ITF_INLINE const Ray_ActivationSequenceVisualComponent_Template* Ray_ActivationSequenceVisualComponent::getTemplate() const
    {
        return static_cast<const Ray_ActivationSequenceVisualComponent_Template*>(m_template);
    }

}; // end namespace

#endif //_ITF_RAY_ACTIVATIONSEQUENCEVISUALCOMPONENT_H_
