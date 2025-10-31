#ifndef _ITF_RAY_AILIGHTNINGACTION_H_
#define _ITF_RAY_AILIGHTNINGACTION_H_

#ifndef _ITF_AIACTION_H_
#include "gameplay/ai/Actions/AIAction.h"
#endif //_ITF_AIACTION_H_

namespace ITF
{
    class Ray_AILightningAction : public AIAction
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILightningAction, AIAction,1689104191);

    public:

        Ray_AILightningAction();
        virtual ~Ray_AILightningAction();

		virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void            onActivate();
		virtual void            onDeactivate();
		
        virtual void            update( f32 _dt );
        virtual void            updateAnimInputs();
		virtual void            onEvent( Event * _event );

	protected:

        void                    updatePos();

        ActorRef                m_bindedActor;
        ActorRef                m_lightning;
        u32                     m_startBoneIdx;
        u32                     m_destBoneIdx;

        class TimedSpawnerAIComponent * m_timedSpawnerAIComponent;

    private:

        ITF_INLINE const class Ray_AILightningAction_Template* getTemplate() const;
    };

    class Ray_AILightningAction_Template : public AIAction_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AILightningAction_Template,AIAction_Template,302679119);
        DECLARE_SERIALIZE();
        DECLARE_AIACTION_TEMPLATE(Ray_AILightningAction);

    public:

        Ray_AILightningAction_Template();
        ~Ray_AILightningAction_Template() {}

		ITF_INLINE const StringID &	 getStartBoneName()	const	{ return m_startBone; }
        ITF_INLINE const StringID &	 getDestBoneName()	const	{ return m_destBoneName; }
        ITF_INLINE const f32 &	     getDefaultSize()	const	{ return m_defaultSize; }
        ITF_INLINE const Angle &	 getAngleOffset()	const	{ return m_angleOffset; }
        ITF_INLINE u32 getHitType() const { return m_hitType; }
        ITF_INLINE u32 getHitLevel() const { return m_hitLevel; }
        ITF_INLINE ITF::bbool getUseStartStim() const { return m_useStartStim; }
        ITF_INLINE ITF::bbool getUseDestStim() const { return m_useDestStim; }
        ITF_INLINE f32 getStartStimRadius() const { return m_startStimRadius; }
        ITF_INLINE f32 getDestStimRadius() const { return m_destStimRadius; }
        ITF_INLINE const Vec2d & getStartStimOffset() const { return m_startStimOffset; }
        ITF_INLINE const Vec2d & getDestStimOffset() const { return m_destStimOffset; }
	private :

        StringID    m_startBone;
        StringID    m_destBoneName;
        f32         m_defaultSize;
        Angle       m_angleOffset;
        u32         m_hitType;
        u32         m_hitLevel;
        bbool       m_useStartStim;
        bbool       m_useDestStim;
        f32         m_startStimRadius;
        f32         m_destStimRadius;
        Vec2d       m_startStimOffset;
        Vec2d       m_destStimOffset;
    };

    ITF_INLINE const class Ray_AILightningAction_Template* Ray_AILightningAction::getTemplate() const { return static_cast<const Ray_AILightningAction_Template*>(m_template); }
}

#endif //_ITF_RAY_AILIGHTNINGACTION_H_

