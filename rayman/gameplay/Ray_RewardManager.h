#ifndef _ITF_RAY_REWARDMANAGER_H_
#define _ITF_RAY_REWARDMANAGER_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef _ITF_REWARDADAPTER_H_
#include "engine/AdaptersInterfaces/RewardAdapter.h"
#endif  //_ITF_REWARDADAPTER_H_


namespace ITF
{

    //_____________________________________________________________________    
    //      Trigger 
    // Triggers are used in order to unlock a reward
    // A reward can have several triggers (using "AND" operator to unlock the reward)
    //_____________________________________________________________________    
    class Ray_RewardTrigger_Base: public IRTTIObject
    {    
        DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_RewardTrigger_Base,IRTTIObject,3170418666);
        DECLARE_SERIALIZE_INTERFACE();
    public:
        
        Ray_RewardTrigger_Base() {}
        virtual ~Ray_RewardTrigger_Base(){}
        virtual bbool   check_reward()=0;

    protected:

        f32 GetTotalAmountOfVarType(StringID var, u32 player);
        f32 GetTotalAmountOfVarTypeForCurrentSession(StringID var, u32 player);
    };

    // Basic trigger, just make the sum of a type of data
    // and valid if the amount of data is reached
    class Ray_RewardTrigger_Sum : public Ray_RewardTrigger_Base
    {   
        DECLARE_OBJECT_CHILD_RTTI(Ray_RewardTrigger_Sum, Ray_RewardTrigger_Base,1124065120);
        DECLARE_SERIALIZE_VIRTUAL();
    public:      

        Ray_RewardTrigger_Sum (): m_varTypeToSum(), m_amountToGet(0), m_useCurrentOnly(bfalse), m_strictlyEqual(bfalse) {}
        virtual ~Ray_RewardTrigger_Sum(){}

        virtual bbool   check_reward();

        StringID            m_varTypeToSum;
        u32                 m_amountToGet;
        bbool               m_useCurrentOnly;
        bbool               m_strictlyEqual; 
    };

    // Same as the Sum trigger, but with a limit of time
    class Ray_RewardTrigger_SumWithTimer : public Ray_RewardTrigger_Sum
    {   
        DECLARE_OBJECT_CHILD_RTTI(Ray_RewardTrigger_SumWithTimer,Ray_RewardTrigger_Sum,2171631799);
        DECLARE_SERIALIZE();
    public:      
        Ray_RewardTrigger_SumWithTimer (): m_time(0.0f), m_validTime(NULL), m_lastAmount(0),
            Ray_RewardTrigger_Sum(){}
        virtual ~Ray_RewardTrigger_SumWithTimer(){
            if(m_validTime) Memory::free(m_validTime);
        }

        virtual bbool   check_reward();

        f32         m_time;
        f32*        m_validTime;
        u32         m_lastAmount;
    };

    // Triggered when the end of a level is reached
    class Ray_RewardTrigger_FinishLevel: public Ray_RewardTrigger_Base
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_RewardTrigger_FinishLevel,Ray_RewardTrigger_Base,2436977909);
        DECLARE_SERIALIZE_VIRTUAL();
    public:
        Ray_RewardTrigger_FinishLevel() : Ray_RewardTrigger_Base() {}
        virtual ~Ray_RewardTrigger_FinishLevel(){}

        virtual bbool   check_reward();

        StringID    m_WorldTag;
    };

    // Trigger event
    class Ray_RewardTrigger_Event: public Ray_RewardTrigger_Base
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_RewardTrigger_Event,Ray_RewardTrigger_Base,3694137488);
        DECLARE_SERIALIZE_VIRTUAL();
    public:
        Ray_RewardTrigger_Event() : Ray_RewardTrigger_Base() {}
        virtual ~Ray_RewardTrigger_Event(){}

        virtual bbool   check_reward();

        StringID    m_Event;
    };


    // Triggered when a specific time is elapsed
    class Ray_RewardTrigger_Timer: public Ray_RewardTrigger_Base
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_RewardTrigger_Timer,Ray_RewardTrigger_Base,1659492541);
        DECLARE_SERIALIZE_VIRTUAL();
    public:
        Ray_RewardTrigger_Timer() : m_timer(StringID::Invalid), m_timeToGet(0.0f), m_useCurrentOnly(bfalse), m_reachTimeToGet( btrue ), Ray_RewardTrigger_Base() {}
        virtual ~Ray_RewardTrigger_Timer(){}

        virtual bbool   check_reward();

        StringID        m_timer;
        f32             m_timeToGet;
        bbool           m_useCurrentOnly;
        bbool           m_reachTimeToGet;           // flag used check if the timer has to reached the 'timed to get' or must be less
    };

    //_____________________________________________________________________
    //      Details
    // (Name is only used for PC)
    // Contains a list of triggers and the ID, common data between all platforms
    //_____________________________________________________________________
    class Ray_RewardDetail
    {
        DECLARE_SERIALIZE()
        virtual ~Ray_RewardDetail();

        RewardID ID;
        StringID Name;
        ITF_VECTOR<Ray_RewardTrigger_Base*>  Triggers;
    };

    //_____________________________________________________________________
    //      Ray_RewardContainer_Template
    // Contains the list of the rewards described in script
    //_____________________________________________________________________    
    class Ray_RewardContainer_Template : public TemplateObj
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_RewardContainer_Template, TemplateObj,3983479571);
        DECLARE_SERIALIZE()
        Ray_RewardContainer_Template()
            : m_isSilent(bfalse), Super()
        {}
        Ray_RewardContainer_Template(const Path& _path)
            : m_isSilent(bfalse), Super(_path)
        {}

        typedef ITF_VECTOR<Ray_RewardDetail> RewardDetailList;
        ITF_INLINE const RewardDetailList & getRewards()const { return m_rewards; }

        bbool   m_isSilent;     // only available on WIN32, will not show the notification if false
        friend class Ray_RewardManager;
    private:
        RewardDetailList    m_rewards;
    };

    //_____________________________________________________________________
    class Ray_PersistentGameData_Universe;
    class Ray_RewardManager 
    {
    public:
        Ray_RewardManager() : m_templateIsLoaded(bfalse), m_isInit(bfalse), m_container(NULL)            
#ifdef ITF_WINDOWS
            , m_displayNotification(bfalse), m_displayRewardsMenu(bfalse)
#endif //ITF_WINDOWS
            {}
        virtual ~Ray_RewardManager();

        void init();
        void update();
        void fillRewardManagerFromLoadGame(Ray_PersistentGameData_Universe* gameData);
        void hideRewards();
        void displayRewards();
        bbool isRewardsListDisplayed() const;

        enum Tags
        {
            Action_Unknown = 0,
            Action_Jump,
            Action_GiveHit,
            Action_ReceiveHit,
            Action_GetSkull,
            Action_GetSkullTooth,
            Action_GetHiddenCage,
            Action_KillEnnemy,
            Action_KilledByEnnemy,
            Action_KilledAlone, // Maybe change this tag?
            Action_GetCrushedEnemy,                     // The player killed an enemy with a crush attack
            Action_GetCrushedEnemySameAttack,           // The player killed an enemy within the current crush attack
            Action_GetSprintDistance,                   // The distance the player has sprinted
            Action_GetSwimDistance,                     // The distance the player has swam
            Action_FinishLevelNoHit,                    // Finished the level without being touch
            Action_FinishLevelSprintNotReleased,        // Finished the level without releasing the sprint
            Action_SurvivedPiranhaPond,
            Action_GetBouncedEnemiesSameJump,           // Number of bounced enemies within the same jump
            Action_GetRescuedRedWizardsOceanA1,         // Number of red wizards rescued on Ocean A1
            Action_GetKungFuCombo,                      // Kung-Fu combos performed (crouch kick + kick in the air)
            Action_ReplayingAMap,                       // Replayed any completed map.
            Action_PoppedEnemyBubble,                   // Popped a bubbleized Enemy
            Action_PoppedEnemyBubbleByTapping,          // Popped a bubbleized Enemy by tapping it
            Action_PoppedItemBubbleByTapping,           // Popped a bubbleized Item by tapping it
            Action_PickedRelic,                         // Picked a relic
            Action_GoThroughCageMapDoor,                // Popped a bubbleized Enemy
            Action_KillHunterWithOwnBullet,             // Kill a Hunter with his own seeking projectile.
            Action_VacuumKnack,                         // Vacuum a min number of element
            Action_ShooterKillBoss,                     // kill a boss (works with bleu baron timer)
            Action_KillBigMama,                         // kill the very last boss (BigMama,death world)
            Action_FinishWorld,
            Action_ChainBubble,                         // Chain-Bubblized 4 Enemies.
            Action_FinishedLevelWithCostume,            // If we finished a level with a custom costume

            Action_TimeAttackWon,                       // Get a cup in time attack mode
            
            // powers up
            Action_PowerUp_Fight,
            Action_PowerUp_Helicopter,
            Action_PowerUp_Reduction,
            Action_Powerup_Dive,
            Action_PowerUp_WalkOnWallsGlobal,

            Action_MedalCompleted,                      // Finish a medal
            Action_RewardCompleted,                     // Finish a reward; Sweet Dreams counts these to N-1
                        
            Event_StartNewLevel,
            Event_EndOfLevel,
            Event_CompleteWorld,

            // IMPORTANT  ! Must be the last ENUM ENTRY
            Tags_Count
        };

        enum Timers
        {
            Timer_Unknown=0,
            Timer_IsAlive,
            Timer_IsGhost,

            Timer_DoingHelico,
            Timer_BlueBaron,                           // Shoot the Giant Eel within N Seconds

            // IMPORTANT  ! Must be the last ENUM ENTRY
            Timers_Count
        };

        
        ObjectFactory *     getTriggerFactory() { return &m_triggersFact;}
        const StringID &    getTriggerFromTag(Tags& _tag);
        Tags                getTagFromTrigger(const StringID & _trigger);
        Timers              getTimerFromTrigger(const StringID & _trigger);

    private:
        const Ray_RewardContainer_Template*     m_container;
        ObjectFactory       m_triggersFact;
        bbool               m_templateIsLoaded;
        bbool               m_isInit;

        void initTags();
        bbool loadLUAFile();

        typedef ITF_MAP<Tags, StringID> TagsTriggersList;
        TagsTriggersList    m_tagsTriggersList;

        typedef ITF_MAP<Timers, StringID> TimersTriggersList;
        TimersTriggersList  m_timersTriggersList;


        void addTriggerForTags(Tags _tag, const StringID& _id);
        void addTriggerForTimers(Timers _timer, const StringID& _id);

#ifdef ITF_WINDOWS
        bbool   m_displayNotification;
        bbool   m_displayRewardsMenu;
        struct RewardToDisplay
        {
            RewardID ID;
            f64 StartTime;
        };
        SafeArray<RewardToDisplay>  m_toDisplay;
#endif //ITF_WINDOWS
    };

};

#endif //_ITF_RAY_POWERUPNAMES_H_

