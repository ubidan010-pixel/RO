#ifndef _ITF_RAY_BOSSBUBOAICOMPONENT_H_
#define _ITF_RAY_BOSSBUBOAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{
    class ShapeDetectorComponent;

    class Ray_BossBuboAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossBuboAIComponent, Ray_AIComponent,1566657424)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BossBuboAIComponent();
        virtual ~Ray_BossBuboAIComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        onEvent(Event* _event);

    private:

        void                hitBy(ActorRef senderRef);

        ITF_INLINE const class Ray_BossBuboAIComponent_Template* getTemplate() const;

        void                triggerMegaBubo(bbool _activate);
        void                triggerBubo(bbool _activate);
        void                onGameplayEvent(AnimGameplayEvent* animEvent);
        void                processInteractionQuery(EventInteractionQuery* _evt);
        void                die();
        void                died();
        void                triggerChildren();
        void                triggerActivator();
        bbool               triggerRewards();
        void                spawnReward(u32 _index, Actor* _sender);

        LinkComponent*          m_linkComponent;
        ShapeDetectorComponent* m_shapeDetector;

        bbool                   m_isDisplay;
        bbool                   m_isHitable;
        bbool                   m_isDying;
        ActorRef                m_activator;
        u32                     m_megaBuboHitPoints;
    };

    //-------------------------------------------------------------------------------------

    class Ray_BossBuboAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossBuboAIComponent_Template, Ray_AIComponent_Template,805555326);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BossBuboAIComponent);

    public:

        Ray_BossBuboAIComponent_Template();
        ~Ray_BossBuboAIComponent_Template() {}
        ITF_INLINE const StringID &  getInvisibleAnim() const   { return m_invisibleAnim;   } 
        ITF_INLINE const StringID &  getAppearAnim() const      { return m_appearAnim;      }
        ITF_INLINE const StringID &  getDisappearAnim() const   { return m_disappearAnim;   }
        ITF_INLINE const StringID &  getHitAnim() const         { return m_hitAnim;         }
        ITF_INLINE const StringID &  getDeathAnim() const       { return m_deathAnim;       }
        ITF_INLINE Faction           getAllowedFaction() const  { return m_allowedFaction;  }
        ITF_INLINE u32               getNbRewards() const       { return m_nbRewards;       }
        ITF_INLINE bbool             getTriggerActivator() const { return m_triggerActivator; }
        ITF_INLINE bbool             getDelayTrigger() const    { return m_delayTrigger;    }
        ITF_INLINE bbool             getIsCrushable() const     { return m_isCrushable;     }
        ITF_INLINE bbool             getIsMebaBubo() const      { return m_isMegaBubo;      }
        ITF_INLINE u32               getHitPoints() const       { return m_megaBuboHitPoints;}

    private:

        StringID        m_invisibleAnim;
        StringID        m_appearAnim;
        StringID        m_disappearAnim;
        StringID        m_hitAnim;
        StringID        m_deathAnim;
        Faction         m_allowedFaction;
        u32             m_nbRewards;
        bbool           m_triggerActivator;
        bbool           m_delayTrigger;
        bbool           m_isCrushable;
        bbool           m_isMegaBubo;
        u32             m_megaBuboHitPoints;
    };


    //-------------------------------------------------------------------------------------

    ITF_INLINE const Ray_BossBuboAIComponent_Template* Ray_BossBuboAIComponent::getTemplate() const
    {
        return static_cast<const Ray_BossBuboAIComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_BOSSBUBOAICOMPONENT_H_