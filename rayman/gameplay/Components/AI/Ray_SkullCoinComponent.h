#ifndef _ITF_RAY_SKULLCOINCOMPONENT_H_
#define _ITF_RAY_SKULLCOINCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_BEZIERPATCH_H_
#include "engine/display/BezierPatch.h"
#endif //_ITF_BEZIERPATCH_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

namespace ITF
{
    class AnimLightComponent;
    class FXControllerComponent;
    class TweenComponent;

    ///////////////////////////////////////////////////////////////////////////////
    class Ray_SkullCoinComponent : public GraphicComponent
    {
    public:

        DECLARE_OBJECT_CHILD_RTTI(Ray_SkullCoinComponent, GraphicComponent,1458755036)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

        enum State_t 
        {
            State_NA,
            State_WaitingToBePickedUp,
            State_Exploding, 
            State_BackToNormalPos,
            State_FollowPlayer,
            State_Taken, //taken for good
            ENUM_FORCE_SIZE_32(0)
        };


        Ray_SkullCoinComponent();
        ~Ray_SkullCoinComponent();

        virtual bbool       needsUpdate() const     { return btrue; }
        virtual bbool       needsDraw() const       { return btrue; }
        virtual bbool       needsDraw2D() const     { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        Update( f32 _deltaTime );
        virtual void        Draw();
        virtual void        onEvent( Event* _event );

        virtual void        onCheckpointLoaded();
        
    private:
        void        pickup(ActorRef _playerRef, bbool _explodesImmediately);
        void        resetState();
        void        setState(State_t _state);
        bbool       checkIfReceivedHit();
        void        setAlwaysActive(bbool _alwaysActive);
        void        onActorRemovedFromScene(Scene* _pScene, bbool _forDeletion);
        void        updatePulsationScale(f32 _interpolator, f32 _time);
        void        backToNormalScaleProgressively();
        void        backToNormalPos(f32 _dt);
        void        setCurrentAnim(const StringID &_name);
        Vec2d       computeVisualOffsetFromPos(const Vec3d &_newPos);
        ITF_INLINE Vec3d computePosFromVisualOffset();
        ITF_INLINE f32 computeLineThickness(f32 _x, f32 _endX, f32 _radius, f32 _minThicknessPercent);

    private:
        AtlasDynamicObject  m_atlas;
        State_t				m_state;
        f32					m_timeInCurrentState;
        bbool               m_isTaken;
        ObjectRef           m_rewardReceiver;
        u32                 m_currentMonitoredHealth;
        Vec2d               m_visualOffset;
        f32                 m_linkAlpha;
        f32                 m_linkVibrationTimer;
        Vec3d               m_smoothedTargetPosSpeed;
        Vec3d               m_previousTargetPos;
        Vec2d               m_speed;
        StringID            m_currentAnim;
        Vec3d               m_pickedPosition;
        u32                 m_starFX, m_followFX;

        FXControllerComponent *m_fxControllerComponent;
        AnimLightComponent *m_animComponent;
        TweenComponent *m_tweenComponent;
        ITF_INLINE const class Ray_SkullCoinComponent_Template * getTemplate() const;

    };

    //////////////////////////////////////////////////////////////////////////
    // Template

    class TemplateAIBehavior;

    class Ray_SkullCoinComponent_Template : public GraphicComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SkullCoinComponent_Template, GraphicComponent_Template,2707418192);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SkullCoinComponent);

    public:

        Ray_SkullCoinComponent_Template();
        ~Ray_SkullCoinComponent_Template();

        u32 getLumCount() const {return m_lumCount;}
        f32 getChallengeDuration() const {return m_challengeDuration;}
        f32 getMaxDistanceBeforeExploding() const {return m_maxDistanceBeforeExploding;}

        f32 getBezierUPerMeter() const {return m_bezierUPerMeter;}
        f32 getBezierStartWidth() const {return m_bezierStartWidth;}
        f32 getBezierEndWidth() const {return m_bezierEndWidth;}
        f32 getTesselationLength() const {return m_tesselationLength;}
        u32 getBlendMode() const {return m_blendMode;}
        const Path &getTexture() const {return m_texture;}
        f32 getBackToNormalSpeed() const {return m_backToNormalSpeed;}
        f32 getMinPulsation()const {return m_minPulsation;}
        f32 getMaxPulsation() const {return m_maxPulsation;}
        f32 getMinScaleAtStart() const {return m_minScaleAtStart;}
        f32 getMaxScaleAtStart() const {return m_maxScaleAtStart;}
        f32 getMinScaleAtEnd() const {return m_minScaleAtEnd;}
        f32 getMaxScaleAtEnd() const {return m_maxScaleAtEnd;}
        f32 getLinkPulsation() const {return m_linkPulsation;}
        f32 getLinkAmplitude() const {return m_linkAmplitude;}
        const Vec2d &getPlayerFollowOffset() const {return m_playerFollowOffset;}

        f32 getSpeedBlend() const { return m_speedBlend; }
        f32 getSpeedMin() const { return m_speedMin; }
        f32 getSpeedMax() const { return m_speedMax; }
        f32 getBlendAtSpeedMin() const { return m_blendAtSpeedMin; }
        f32 getBlendAtSpeedMax() const { return m_blendAtSpeedMax; }

        f32 getBackToNormalBounciness() const {return m_backToNormalBounciness;}
        f32 getBackToNormalInertia() const {return m_backToNormalInertia;}
        f32 getBackToNormalInitSpeed() const {return m_backToNormalInitSpeed;}
        f32 getMinLinkThicknessFactor() const {return m_minLinkThicknessFactor;}
        f32 getRadius() const {return m_radius;}
        f32 getLenghtForMinThickness() const {return m_lenghtForMinThickness;}
        const Vec2d& getUVAnimTrans() const {return m_uvAnimTrans;}

        f32 getStartAlphaLen() const {return m_startAlphaLen;}
        f32 getEndAlphaLen() const {return m_endAlphaLen;}
        f32 getLinkAlphaMultiplier() const {return m_linkAlphaMultiplier;}


    private:
        u32 m_lumCount;
        f32 m_challengeDuration;
        f32 m_maxDistanceBeforeExploding;

        f32 m_startAlphaLen;
        f32 m_endAlphaLen;
        f32 m_linkAlphaMultiplier;

        f32 m_bezierUPerMeter;
        f32 m_bezierStartWidth;
        f32 m_bezierEndWidth;
        f32 m_tesselationLength;
        u32 m_blendMode;
        Path m_texture;
        f32 m_backToNormalSpeed;
        f32 m_minPulsation;
        f32 m_maxPulsation;
        f32 m_minScaleAtStart;
        f32 m_maxScaleAtStart;
        f32 m_minScaleAtEnd;
        f32 m_maxScaleAtEnd;
        f32 m_linkPulsation;
        f32 m_linkAmplitude;
        Vec2d m_playerFollowOffset;

        f32 m_speedBlend;
        f32 m_speedMin;
        f32 m_speedMax;
        f32 m_blendAtSpeedMin;
        f32 m_blendAtSpeedMax;

        f32 m_backToNormalBounciness;
        f32 m_backToNormalInertia;
        f32 m_backToNormalInitSpeed;

        f32 m_minLinkThicknessFactor;
        f32 m_radius;
        f32 m_lenghtForMinThickness;
        Vec2d m_uvAnimTrans;
    };

    ITF_INLINE const class Ray_SkullCoinComponent_Template * Ray_SkullCoinComponent::getTemplate() const
    {
        return static_cast<const class Ray_SkullCoinComponent_Template *>(m_template);
    }
}

#endif // _ITF_RAY_REWARDAICOMPONENT_H_

