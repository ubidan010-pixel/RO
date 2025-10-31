#ifndef _ITF_RAY_JALAPENOKINGAICOMPONENT_H_
#define _ITF_RAY_JALAPENOKINGAICOMPONENT_H_


#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_


namespace ITF
{

    class Ray_JalapenoKingAiComponent : public AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_JalapenoKingAiComponent, AIComponent,2116341111)
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

    public:

        Ray_JalapenoKingAiComponent();
        virtual ~Ray_JalapenoKingAiComponent();


        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
        virtual void        onEvent( Event* _event );
        virtual void        onCheckpointLoaded();

    private:

                void        startFire();
                void        stopFire();

        ITF_INLINE const class Ray_JalapenoKingAiComponent_Template * getTemplate() const;

        AnimLightComponent* m_animComponent;
        LinkComponent * m_linkComponent;
        class FXControllerComponent * m_fxController;
        bbool   m_spewing;
        bbool m_dead;
        bbool m_hit;
        f32 m_rehitTimer;
        Ray_EventSpawnReward * m_deathReward;


    };

    //-------------------------------------------------------------------------------------
    class Ray_JalapenoKingAiComponent_Template : public AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_JalapenoKingAiComponent_Template, AIComponent_Template,405634581);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_JalapenoKingAiComponent);

    public:

        Ray_JalapenoKingAiComponent_Template();
        virtual ~Ray_JalapenoKingAiComponent_Template();
        ITF_INLINE ITF::StringID getIdleAnim() const { return m_idleAnim; }
        ITF_INLINE ITF::StringID getHitAnim() const { return m_hitAnim; }
        ITF_INLINE Ray_EventSpawnReward* getDeathReward() const { return m_deathReward; }

    private:

        StringID m_idleAnim;
        StringID m_hitAnim;
        Ray_EventSpawnReward * m_deathReward;
    };

    ITF_INLINE const Ray_JalapenoKingAiComponent_Template* Ray_JalapenoKingAiComponent::getTemplate() const
    {
        return static_cast<const Ray_JalapenoKingAiComponent_Template*>(m_template);
    }

};

#endif //_ITF_RAY_JALAPENOKINGAICOMPONENT_H_