#ifndef _ITF_RAY_AIRELICBEHAVIOR_H_
#define _ITF_RAY_AIRELICBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_


namespace ITF
{
    class TriggerComponent;

    class Ray_AIRelicBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRelicBehavior, AIBehavior,573866982);

    public:
        DECLARE_VALIDATE_BEHAVIOR()

		enum State_t {State_NA,State_Preappears,State_Stands, State_Appears, State_BeingTaken, State_Taken,
			ENUM_FORCE_SIZE_32(0)
		};

        Ray_AIRelicBehavior();
        ~Ray_AIRelicBehavior();

		virtual void        update( f32 _deltaTime );

		virtual void		onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        onEvent( Event* _event );
        virtual void        onActivate() {}

        void                resetState();      //reset state from current leveldata

        i32                 getRelicIndex() const { return m_relicIndex; }
        void                setRelicIndex(i32 _relicIndex);
        void                setIsSpawnedByChest(bbool _isFromChest) {m_isFromChest = _isFromChest;}

    private:
        ITF_INLINE const class Ray_AIRelicBehavior_Template * getTemplate() const;

		void setState(State_t _state);

		State_t				m_state;
		f32					m_timeInCurrentState;
        bbool               m_isFromChest;
        i32                 m_relicIndex;

        TriggerComponent*   m_triggerComponent;
    };

    class Ray_AIRelicBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIRelicBehavior_Template,TemplateAIBehavior,2405251001);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIRelicBehavior);

    public:

        Ray_AIRelicBehavior_Template();
        ~Ray_AIRelicBehavior_Template(){}

        i32                 getRelicIndex() const {return m_relicIndex;}

    private:

        i32                 m_relicIndex;
    };

    ITF_INLINE const class Ray_AIRelicBehavior_Template * Ray_AIRelicBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIRelicBehavior_Template *>(m_template);
    }
}

#endif // _ITF_Ray_AIrelicBehavior_H_

