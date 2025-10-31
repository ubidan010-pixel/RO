#ifndef _ITF_RAY_AIBONUSSKULLBEHAVIOR_H_
#define _ITF_RAY_AIBONUSSKULLBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif


namespace ITF
{
    class TriggerComponent;
    class Ray_AIScoreLumBehavior;
    class Trail3DComponent;

    class Ray_AIBonusSkullBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBonusSkullBehavior, AIBehavior,3245584007)
        DECLARE_SERIALIZE();

    public:
        DECLARE_VALIDATE_BEHAVIOR()

		enum State_t {State_NA,
            State_Hidden,
            State_Appears,
            State_Stands, 
            State_Disappears,
			ENUM_FORCE_SIZE_32(0)
		};

        Ray_AIBonusSkullBehavior();
        ~Ray_AIBonusSkullBehavior();

        virtual void        update( f32 _deltaTime );

        virtual void		onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );
        virtual void        onActivate();

    private:
        void setState(State_t _state);
        void resetState();
        void updateScaleAndPosition();
        bbool updatePulse(f32 _t);
        void setToothPresence(u32 _room, bbool _isPresent);

        const class Ray_AIBonusSkullBehavior_Template * getTemplate() const;

        State_t				m_state;
        f32					m_timeInCurrentState;
        f32                 m_skullScale;
        u32                 m_takenTeethCount;
    };

    class Ray_AIBonusSkullBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIBonusSkullBehavior_Template,TemplateAIBehavior,3820934562);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIBonusSkullBehavior);

    public:

        Ray_AIBonusSkullBehavior_Template();
        ~Ray_AIBonusSkullBehavior_Template();

        f32 getDefaultSizePercent() const {return m_defaultSizePercent;}
        f32 getMaxSizePercent() const {return m_maxSizePercent;}
        f32 getPulseDuration() const {return m_pulseDuration;}
        f32 getMaxSizeDuration() const {return m_maxSizeDuration;}
        f32 getStandDuration() const {return m_standDuration;}

    private:
        f32         m_defaultSizePercent;
        f32         m_maxSizePercent;
        f32         m_pulseDuration;
        f32         m_maxSizeDuration;
        f32         m_standDuration;
    };

    ITF_INLINE const class Ray_AIBonusSkullBehavior_Template * Ray_AIBonusSkullBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIBonusSkullBehavior_Template *>(m_template);
    }
}

#endif // _ITF_Ray_AIBonusSkullBehavior_H_

