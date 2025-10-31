#ifndef _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
#define _ITF_RAY_AIRECEIVEHITBEHAVIOR_H_

#ifndef _ITF_AIRECEIVEHITBEHAVIOR_H_
#include "gameplay/AI/Behaviors/AIReceiveHitBehavior.h"
#endif //_ITF_AIRECEIVEHITBEHAVIOR_H_

namespace ITF
{
    class AIReceiveHitAction;
    class PhantomComponent;
    class Ray_AvoidanceComponent;
    class PolylineComponent;

    class Ray_AIReceiveHitBehavior : public AIReceiveHitBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIReceiveHitBehavior, AIReceiveHitBehavior,2277900683);
        DECLARE_VALIDATE_BEHAVIOR()

    public:

        Ray_AIReceiveHitBehavior();
        virtual ~Ray_AIReceiveHitBehavior();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void                onActivate();
        virtual void                onDeactivate();
        virtual void                onActionFinished();
        virtual void                updateAnimInputs();
        virtual void                onEvent(Event * _event);
        virtual void                update( f32 _dt );
#ifdef ITF_SUPPORT_EDITOR
        virtual void                drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

        virtual void                setData( HitStim* _hit );
        virtual bbool               canReceiveHit(HitStim * _hit);
        f32                         getHurtTimer() const { return m_hurtTimer; }
		void						updateRehit();
        PhysShape *                 getDominoHitShape() const;

    private:

        ITF_INLINE const class Ray_AIReceiveHitBehavior_Template * getTemplate() const;

		void						processHitSuccessful( EventHitSuccessful* _onHitSuccessful );

        PhantomComponent*           m_phantomComponent;
        Ray_AvoidanceComponent*     m_avoidanceComponent;
        StickToPolylinePhysComponent* m_physComponent;
        PolylineComponent*          m_polylineComponent;

        bbool                       m_isInAir;

        struct ReceiveHitData
        {
            ReceiveHitData() : m_action(NULL)
            {
            }
            ~ReceiveHitData();

            AIReceiveHitAction*             m_action;
        };

        ITF_VECTOR<ReceiveHitData>          m_receiveHits;

		//domino hits
		PhysShape *							m_dominoHitShape;
		// from setData
		bbool								m_reHit;
        u32                                 m_consecutiveHitCount;
        f32                                 m_hurtTimer;
        bbool                               m_justEntered;
        u32                                 m_lastHitFrame;

        u32                                 m_identifier;
        AIReceiveHitAction *                m_ceilingAction;
        AIReceiveHitAction *                m_wallAction;
    };

    class Ray_AIReceiveHitBehavior_Template : public AIReceiveHitBehavior_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIReceiveHitBehavior_Template,AIReceiveHitBehavior_Template,2856657059);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIReceiveHitBehavior);

    public:

        struct ReceiveHitData
        {
            DECLARE_SERIALIZE()

            ReceiveHitData()
                : m_level(U32_INVALID)
                , m_useIfAlive(btrue)
                , m_useIfDead(btrue)
                , m_useIfInAir(btrue)
                , m_useIfOnGround(btrue)
                , m_action(NULL)
                , m_isInterruptible(btrue)
            {
            }
            ~ReceiveHitData();

            SafeArray<u32>                      m_types;        // empty = any type
            u32                                 m_level;        // default (invalid) = any level
            bbool                               m_useIfAlive;
            bbool                               m_useIfDead;
            bbool                               m_useIfInAir;
            bbool                               m_useIfOnGround;
            bbool                               m_isInterruptible;
            class AIReceiveHitAction_Template*  m_action;
        };

        Ray_AIReceiveHitBehavior_Template();
        ~Ray_AIReceiveHitBehavior_Template();

        ITF_INLINE const ITF_VECTOR<ReceiveHitData>&       getReceiveHits() const { return m_receiveHits; }
        ITF_INLINE bbool                                   getCanReceiveRehits() const { return m_canReceiveRehits; }
        ITF_INLINE f32                                     getHurtDuration() const { return m_hurtDuration; }
        ITF_INLINE u32                                     getMaxNumberOfSuccessiveHits() const { return m_maxNumberOfSuccessiveHits; }
		ITF_INLINE f32                                     getDominoHitSpeedMultiplier() const { return m_dominoHitSpeedMultiplier; }
		ITF_INLINE bbool                                   getCanRehit() const { return m_canRehit; }
        ITF_INLINE const class PhysShape*                  getDominoHitShape() const { return m_dominoHitShape; }
        ITF_INLINE u32                                     getMinHitLevelForDomino() const {return m_minHitLevelForDomino;}
        ITF_INLINE bbool                                   getDisableStickOnWallsOnHit() const { return m_disableStickOnWallsOnHit; }
        ITF_INLINE bbool                                   getCanBubblize() const { return m_canBubblize; }
        ITF_INLINE bbool                                   getReceiveDamageFromDomino() const { return m_receiveDamageFromDomino; }
        ITF_INLINE const class AIReceiveHitAction_Template* getCeilingAction() const { return m_ceilingAction; }
        ITF_INLINE const class AIReceiveHitAction_Template* getWallAction() const { return m_wallAction; }
        ITF_INLINE const StringID &                         getDeathMarkerName() const { return m_deathMarkerName; }

    private:

        ITF_VECTOR<ReceiveHitData>              m_receiveHits;
        bbool                                   m_canReceiveRehits;
        bbool                                   m_canRehit;
        f32                                     m_hurtDuration;
        u32                                     m_maxNumberOfSuccessiveHits;
		class PhysShape*                        m_dominoHitShape;     
		f32                                     m_dominoHitSpeedMultiplier;
        u32                                     m_minHitLevelForDomino;
        bbool                                   m_disableStickOnWallsOnHit;
        bbool                                   m_canBubblize;
        bbool                                   m_receiveDamageFromDomino;
        class AIReceiveHitAction_Template*      m_ceilingAction;
        class AIReceiveHitAction_Template*      m_wallAction;
        StringID                                m_deathMarkerName;
    };

    ITF_INLINE const class Ray_AIReceiveHitBehavior_Template * Ray_AIReceiveHitBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIReceiveHitBehavior_Template *>(m_template);
    }
}

#endif //_ITF_RAY_AIRECEIVEHITBEHAVIOR_H_
