#ifndef _ITF_RAY_AISCORELUMSBEHAVIOR_H_
#define _ITF_RAY_AISCORELUMSBEHAVIOR_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_


namespace ITF
{
    class TriggerComponent;

    class Ray_AIScoreLumBehavior : public AIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIScoreLumBehavior, AIBehavior,2156335494);

    public:
        
        DECLARE_VALIDATE_BEHAVIOR()

		enum ComboState_t {ComboState_None, ComboState_Combo,	
			ENUM_FORCE_SIZE_32(0)
		};
		enum State_t {State_NA,State_Hidden,State_Appears, State_Stands, State_Disappears,	
		ENUM_FORCE_SIZE_32(1)
		};
		enum Color_t {Color_Yellow, Color_Red, 
		ENUM_FORCE_SIZE_32(2)
		};

        Ray_AIScoreLumBehavior();
        ~Ray_AIScoreLumBehavior();

        virtual void        update( f32 _deltaTime );

        virtual void		onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        onEvent( Event* _event );
        virtual void        onActivate() {}

        static ObjectRef    getInstance() {return m_thisObjectRef;}

        Color_t             getGlobalLumColor() const {return (m_comboState==ComboState_Combo)?Color_Red:Color_Yellow;} //Color for all lums
        void                getGlobalColorWeights(f32 &_yellowW, f32 &_orangeW, f32 &_redW) const;

        void                clearRedMode();

        ITF_INLINE const class Ray_AIScoreLumBehavior_Template* getTemplate() const;

    private:

        void setComboState(ComboState_t _state);
        void clear();
        void resetState();
        void processAddLum(Ray_EventAddLum *_event);
        void processLumReachesScore(Ray_EventLumReachesScore *_event);
        u32 getLumValue(Ray_EventAddLum *_event);
        void flushScorePerPlayer();
        void flushScoreForGivenPlayer(u32 _playerIndex);
        static void getColorWeightsFromColor(Color_t _color, f32 &_redW, f32 &_orangeW, f32 &_yellowW);

        u32                 m_acquiredLumsForSession;
        f32                 m_timeInCurrentComboState; //reset in every setComboState, even if doesnt change
        f32                 m_timeSinceLumReachedScore;
        u32                 m_lumScoreForSession;
        u32                 m_visualLumScoreForSize;
        ComboState_t        m_comboState;
        u32                 m_lumScoreForSessionPerPlayer[s_maxPlayers];


        static ObjectRef    m_thisObjectRef;
    };

    class Ray_AIScoreLumBehavior_Template : public TemplateAIBehavior
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_AIScoreLumBehavior_Template,TemplateAIBehavior,2806870689);
        DECLARE_SERIALIZE();
        DECLARE_AIBEHAVIOR_TEMPLATE(Ray_AIScoreLumBehavior);

    public:

        Ray_AIScoreLumBehavior_Template();
        ~Ray_AIScoreLumBehavior_Template();

        f32                 getMaxTimeBeforeExplosion_RedMode() const { return m_maxTimeBeforeExplosion_RedMode; }
        u32                 getYellowLumValue() const { return m_yellowLumValue; }
        u32                 getRedLumValue() const { return m_redLumValue; }
        u32                 getLumKingValue() const {return m_lumKingValue;}
        Event               *getStartKingMusicEvent() const { return m_startKingMusicEvent;}
        Event               *getStopKingMusicEvent() const { return m_stopKingMusicEvent;}

    private:

        f32                 m_maxTimeBeforeExplosion_RedMode;
        u32                 m_yellowLumValue;
        u32                 m_redLumValue;
        u32                 m_lumKingValue;
        Event               *m_startKingMusicEvent, *m_stopKingMusicEvent;
    };

    ITF_INLINE const class Ray_AIScoreLumBehavior_Template * Ray_AIScoreLumBehavior::getTemplate() const
    {
        return static_cast<const class Ray_AIScoreLumBehavior_Template *>(m_template);
    }
}

#endif // _ITF_Ray_AIScoreLumBehavior_H_

