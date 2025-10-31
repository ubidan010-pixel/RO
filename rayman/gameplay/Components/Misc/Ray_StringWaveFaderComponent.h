#ifndef _ITF_RAY_STRINGWAVEFADERCOMPONENT_H_
#define _ITF_RAY_STRINGWAVEFADERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{
    class AnimatedComponent;
    class LinkComponent;
    class FXControllerComponent;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_StringWaveFaderComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StringWaveFaderComponent, ActorComponent,144433532);
    public:
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

        Ray_StringWaveFaderComponent();
        ~Ray_StringWaveFaderComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual void                Update( f32 _deltaTime );
        virtual void                onEvent( Event* _event );
        virtual bbool               needsUpdate() const {return btrue;}
        virtual bbool               needsDraw() const {return bfalse;}
        virtual bbool               needsDraw2D() const {return bfalse;}

    protected:
        SafeArray<ActorRef>         m_actorsOnTop;
        f32                         m_pos, m_speed; //platform pos/speed
        AnimatedComponent           *m_animatedComponent;
        LinkComponent               *m_linkComponent;
        bbool                       m_isOn;
        FXControllerComponent       *m_fxControllerComponent;
        u32                         m_downwardFX;

        void                        resetState();
        void                        updateOnOff(bbool _fxOnChange);
        void                        updateLinkeeScale(f32 _dt);
        bbool                       hasCollision(PunchStim *_hit, const StringID &_polyline);

        ITF_INLINE const class Ray_StringWaveFaderComponent_Template * getTemplate() const;
    };

    ///////////////////////////////////////////////////////////////////////////////////////////
    class Ray_StringWaveFaderComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StringWaveFaderComponent_Template, TemplateActorComponent,1145134707);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_StringWaveFaderComponent);

    public:
        Ray_StringWaveFaderComponent_Template();

        f32             getMinBounceSpeed() const {return m_minBounceSpeed;}
        f32             getMaxUpwardSpeed() const {return m_maxUpwardSpeed;}
        f32             getMaxDownwardSpeed() const {return m_maxDownwardSpeed;}
        f32             getUpwardAcceleration() const {return m_upwardAcceleration;}
        f32             getDownwardAcceleration() const  {return m_downwardAcceleration;}
        f32             getBounceFactor() const {return m_bounceFactor;}
        f32             getThresholdPosition() const {return m_thresholdPosition;}
        f32             getLinkeeScaleForOn() const {return m_linkeeScaleForOn;}
        f32             getLinkeeScaleForOff() const {return m_linkeeScaleForOff;}
        f32             getCrushSpeed() const {return m_crushSpeed;}
        const StringID& getBounceOnEndFX() const {return m_bounceOnEndFX;}
        const StringID& getGoDownwardFX() const {return m_goDownwardFX;}
        f32             getMinSpeedForSound() const {return m_minSpeedForSound;}

    private:
        f32             m_minBounceSpeed, m_maxDownwardSpeed, m_maxUpwardSpeed;
        f32             m_upwardAcceleration, m_downwardAcceleration;
        f32             m_bounceFactor;
        f32             m_thresholdPosition;
        f32             m_linkeeScaleForOn, m_linkeeScaleForOff;
        f32             m_crushSpeed;
        StringID        m_bounceOnEndFX;
        StringID        m_goDownwardFX;
        f32             m_minSpeedForSound;

    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    ITF_INLINE const Ray_StringWaveFaderComponent_Template * Ray_StringWaveFaderComponent::getTemplate() const
    {
        return static_cast<const class Ray_StringWaveFaderComponent_Template *>(m_template);
    }

}

#endif
