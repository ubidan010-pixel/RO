#ifndef _ITF_RAYSINKINGPLATFORMCOMPONENT_H_
#define _ITF_RAYSINKINGPLATFORMCOMPONENT_H_


#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_


namespace ITF
{
    class Ray_SinkingPlatformComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SinkingPlatformComponent, ActorComponent,4096536880)
        DECLARE_VALIDATE_COMPONENT()
    public:
        DECLARE_SERIALIZE()

        Ray_SinkingPlatformComponent();
        virtual ~Ray_SinkingPlatformComponent();

        virtual bbool   needsUpdate() const { return btrue; }
        virtual bbool   needsDraw() const { return bfalse; }
        virtual bbool   needsDraw2D() const { return bfalse; }
        virtual void    Update( f32 _deltaTime );
        virtual void    onResourceReady();
        virtual void    onEvent(Event * _event);
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    private:
        ITF_INLINE const class Ray_SinkingPlatformComponent_Template * getTemplate() const;
 
        class PolylineComponent * m_polylineComponent;
        class AnimLightComponent * m_animComponent;
        class TweenComponent * m_tweenComponent;
        class ObjectControllerComponent * m_objectControllerComponent;
        class FXControllerComponent * m_fxControllerComponent;
        f32 m_currentWeight;
        f32 m_currentSpeed;

        struct User
        {
            User () : m_weight(0.0f) {}
            f32 m_weight;
            ActorRef m_user;
        };

        SafeArray<User> m_users;
        u32 m_waterInfluenceBoneIndex;
        f32 m_previousWeight;
        u32 m_sinkFx;
        u32 m_floatFx;
    };

    //-------------------------------------------------------------------------------------
    class Ray_SinkingPlatformComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SinkingPlatformComponent_Template, TemplateActorComponent,2233895274);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SinkingPlatformComponent);

    public:

        Ray_SinkingPlatformComponent_Template();
        ~Ray_SinkingPlatformComponent_Template();

        ITF_INLINE f32 getWeightStep() const { return m_weightStep; }
        ITF_INLINE f32 getFactor() const { return m_factor; }
        ITF_INLINE f32 getStepMultiplier() const { return m_stepMultiplier; }
        ITF_INLINE f32 getMaxDistance() const { return m_maxDistance; }
        ITF_INLINE f32 getRestorationFactor() const { return m_restorationFactor; }
        ITF_INLINE StringID getWaterInfluenceBone() const { return m_waterInfluenceBone; }
        ITF_INLINE StringID getSinkFx() const { return m_sinkFx; }
        ITF_INLINE StringID getFloatFx() const { return m_floatFx; }


    private:
        f32 m_weightStep;
        f32 m_factor;
        f32 m_stepMultiplier;
        f32 m_maxDistance;
        f32 m_restorationFactor;
        StringID m_waterInfluenceBone;
        StringID m_sinkFx;
        StringID m_floatFx;
    };

    ITF_INLINE const class Ray_SinkingPlatformComponent_Template * Ray_SinkingPlatformComponent::getTemplate() const
    {
        return static_cast<const class Ray_SinkingPlatformComponent_Template *>(m_template);
    }

}

#endif // _ITF_RAYSINKINGPLATFORMCOMPONENT_H_
