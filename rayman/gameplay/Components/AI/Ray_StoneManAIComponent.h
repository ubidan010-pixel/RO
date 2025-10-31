#ifndef _ITF_RAY_STONEMANAICOMPONENT_H_
#define _ITF_RAY_STONEMANAICOMPONENT_H_

#ifndef _ITF_RAY_AIGROUNDENEMYCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_GroundEnemyAIComponent.h"
#endif //_ITF_RAY_AIGROUNDENEMYCOMPONENT_H_

namespace ITF
{
    //forward declarations
    class Ray_StoneManAIComponent_Template;

    //-------------------------------------------------------
    //  Manage stone man behaviors
    //-------------------------------------------------------
    class Ray_StoneManAIComponent : public Ray_GroundEnemyAIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StoneManAIComponent, Ray_GroundEnemyAIComponent,3804759324)
        DECLARE_VALIDATE_COMPONENT()
        DECLARE_SERIALIZE();

    public:

        Ray_StoneManAIComponent();
        virtual ~Ray_StoneManAIComponent();

        virtual void onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void onCheckpointLoaded();
        virtual void onBehaviorFinished();
        virtual void Update(f32 _dt);
        virtual void onEvent(Event* _event);

        ITF_INLINE void setBoulderPresence(const bbool _presence);
        ITF_INLINE bbool getBoulderPresence() const;

    protected:

        virtual void startReceiveHit(HitStim* _hit);
        virtual void onFinishedReceiveHit();
        virtual void startCloseRangeAttack(const bbool _flip);
        virtual bbool checkHealthDeath();

        //manage boulder list, remove invalid refs
        virtual void updateBoulderList();
        //inform boulders that stoneman is dying
        void sendSympathyToBoulders();

    private:
        ITF_INLINE const Ray_StoneManAIComponent_Template * getTemplate() const;

        void onFinishedReceiveHitWithBoulder();


        //Data
    private:
        Ray_AIReceiveHitBehavior * m_receiveHitWithBoulderBehavior;
        AIBehavior * m_closeRangeAttackWithBoulderBehavior;
        AIBehavior * m_closeRangeBackAttackBehavior;
        AIBehavior * m_closeRangeBackAttackWithBoulderBehavior;
        struct BoulderRef
        {
            BoulderRef()
                : m_ref(ActorRef::InvalidRef)
                , m_active(bfalse)
            {
            }

            BoulderRef(const ActorRef & _ref)
            : m_ref(_ref)
            , m_active(bfalse)
            {
            }

            ITF_INLINE bool operator == (const ActorRef & _actorRef)
            {
                return m_ref == _actorRef;
            }
            ITF_INLINE bool operator == (const BoulderRef & _other)
            {
                return m_ref == _other.m_ref;
            }

            ActorRef m_ref;
            bbool m_active;
        };
        SafeArray<BoulderRef> m_boulderList;
        bbool m_boulderPresence;
    };


    
    //-------------------------------------------------------
    //  Template
    //-------------------------------------------------------
    class Ray_StoneManAIComponent_Template : public Ray_GroundEnemyAIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_StoneManAIComponent_Template, Ray_GroundEnemyAIComponent_Template,3113601795);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_StoneManAIComponent);

    public:

        Ray_StoneManAIComponent_Template();
        ~Ray_StoneManAIComponent_Template();

        ITF_INLINE const Ray_AIReceiveHitBehavior_Template * getReceiveHitWithBoulderBehavior() const;
        ITF_INLINE const TemplateAIBehavior * getCloseRangeAttackWithBoulderBehavior() const;
        ITF_INLINE const TemplateAIBehavior * getCloseRangeBackAttackBehavior() const;
        ITF_INLINE const TemplateAIBehavior * getCloseRangeBackAttackWithBoulderBehavior() const;

        //Data
    private:
        Ray_AIReceiveHitBehavior_Template * m_receiveHitWithBoulderBehavior;
        TemplateAIBehavior * m_closeRangeAttackWithBoulderBehavior;
        TemplateAIBehavior * m_closeRangeBackAttackBehavior;
        TemplateAIBehavior * m_closeRangeBackAttackWithBoulderBehavior;
    };



    //-------------------------------------------------------
    //  Ray_StoneManAIComponent inlined functions definition
    //-------------------------------------------------------

    ITF_INLINE const Ray_StoneManAIComponent_Template * Ray_StoneManAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_StoneManAIComponent_Template *>(m_template);
    }

    ITF_INLINE void Ray_StoneManAIComponent::setBoulderPresence(const bbool _presence)
    {
        m_boulderPresence = _presence;
    }

    ITF_INLINE bbool Ray_StoneManAIComponent::getBoulderPresence() const
    {
        return m_boulderPresence;
    }



    //-------------------------------------------------------
    // Ray_StoneManAIComponent_Template inlined functions definition
    //-------------------------------------------------------

    ITF_INLINE const Ray_AIReceiveHitBehavior_Template * Ray_StoneManAIComponent_Template::getReceiveHitWithBoulderBehavior() const
    {
        return m_receiveHitWithBoulderBehavior;
    }

    ITF_INLINE const TemplateAIBehavior * Ray_StoneManAIComponent_Template::getCloseRangeAttackWithBoulderBehavior() const
    {
        return m_closeRangeAttackWithBoulderBehavior;
    }

    ITF_INLINE const TemplateAIBehavior * Ray_StoneManAIComponent_Template::getCloseRangeBackAttackBehavior() const
    { 
        return m_closeRangeBackAttackBehavior; 
    }

    ITF_INLINE const TemplateAIBehavior * Ray_StoneManAIComponent_Template::getCloseRangeBackAttackWithBoulderBehavior() const
    {
        return m_closeRangeBackAttackWithBoulderBehavior;
    }

}

#endif //_ITF_RAY_STONEMANAICOMPONENT_H_
