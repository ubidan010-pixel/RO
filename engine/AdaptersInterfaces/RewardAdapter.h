#ifndef _ITF_REWARDADAPTER_H_
#define _ITF_REWARDADAPTER_H_

#ifndef _ITF_TEMPLATESINGLETON_H_
#include "core/templateSingleton.h"
#endif //_ITF_TEMPLATESINGLETON_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef SERIALIZEROBJECT_H
#include "core/serializer/ZSerializerObject.h"
#endif // SERIALIZEROBJECT_H

#ifndef ITF_TEMPLATEDATABASE_H
#include "engine/TemplateManager/TemplateDatabase.h"
#endif // ITF_TEMPLATEDATABASE_H

//_____________________________________________________________________
//      Reward Adapter  (supported by WIN32 / PS3 / X360)
//_____________________________________________________________________
// -> RewardManager = class used to count/time things
// -> RewardTrigger = class used to unlock a reward
// -> RewardDetail  = class used to describe a reward (list of triggers + ID + name for WIN32)
// -> RewardAdapter = Interface for all the plateform to manage unlocking of all rewards
//_____________________________________________________________________

namespace ITF
{
    typedef StringID	RewardID;

    // List of rewards (used for consoles)
    #define s_REWARD_RAY_GETLUMS_1000 ITF_GET_STRINGID_CRC(REWARD_RAY_GETLUMS_1000,3034406398)
    #define s_REWARD_RAY_GETLUMS_100 ITF_GET_STRINGID_CRC(REWARD_RAY_GETLUMS_100,2817827059)
    #define s_REWARD_RAY_GETLUMS_10 ITF_GET_STRINGID_CRC(REWARD_RAY_GETLUMS_10,2293043137)
    #define s_REWARD_RAY_GETLUMS_WITHSPEED_10 ITF_GET_STRINGID_CRC(REWARD_RAY_GETLUMS_WITHSPEED_10,3435864543)
    #define s_REWARD_RAY_CRUSH_COMBO ITF_GET_STRINGID_CRC(REWARD_RAY_CRUSH_COMBO,1777562132)
    #define s_REWARD_RAY_CRUSHER ITF_GET_STRINGID_CRC(REWARD_RAY_CRUSHER,1525047725)
    #define s_REWARD_RAY_SPRINTER ITF_GET_STRINGID_CRC(REWARD_RAY_SPRINTER,627309668)
    #define s_REWARD_RAY_FISHER_KING ITF_GET_STRINGID_CRC(REWARD_RAY_FISHER_KING,2430448838)
    #define s_REWARD_RAY_PAINLESS ITF_GET_STRINGID_CRC(REWARD_RAY_PAINLESS,647916566)
    #define s_REWARD_RAY_HYPERSPEED ITF_GET_STRINGID_CRC(REWARD_RAY_HYPERSPEED,2079958745)
    #define s_REWARD_RAY_SURVIVEDPIRANHAPOND ITF_GET_STRINGID_CRC(REWARD_RAY_SURVIVEDPIRANHAPOND,1668806073)
    #define s_REWARD_RAY_BOING ITF_GET_STRINGID_CRC(REWARD_RAY_BOING,1748529094)
    #define s_REWARD_RAY_POP_POP_BOOM ITF_GET_STRINGID_CRC(REWARD_RAY_POP_POP_BOOM,3389574431)
    #define s_REWARD_RAY_NOPANIC ITF_GET_STRINGID_CRC(REWARD_RAY_NOPANIC,3715794749)
    #define s_REWARD_RAY_KUNGFUCOMBO ITF_GET_STRINGID_CRC(REWARD_RAY_KUNGFUCOMBO,3521565428)
    #define s_REWARD_RAY_PEACEFUL_DREAMS ITF_GET_STRINGID_CRC(REWARD_RAY_PEACEFUL_DREAMS,4154081194)
    #define s_REWARD_RAY_HOVER_HAPPY ITF_GET_STRINGID_CRC(REWARD_RAY_HOVER_HAPPY,2745191180)
    #define s_REWARD_RAY_IAM_BACK ITF_GET_STRINGID_CRC(REWARD_RAY_IAM_BACK,3364256339)
    #define s_REWARD_RAY_SCOUT ITF_GET_STRINGID_CRC(REWARD_RAY_SCOUT,4015206807)
    #define s_REWARD_RAY_Explorer ITF_GET_STRINGID_CRC(REWARD_RAY_Explorer,4064804322)
    #define s_REWARD_RAY_DR_LIVIDSTONE ITF_GET_STRINGID_CRC(REWARD_RAY_DR_LIVIDSTONE,1087948402)
    #define s_REWARD_RAY_BACK_AT_YOU ITF_GET_STRINGID_CRC(REWARD_RAY_BACK_AT_YOU,3772836673)
    #define s_REWARD_RAY_VACUUM_KNACK ITF_GET_STRINGID_CRC(REWARD_RAY_VACUUM_KNACK,1821940084)
    #define s_REWARD_RAY_BLUE_BARON ITF_GET_STRINGID_CRC(REWARD_RAY_BLUE_BARON,160786933)
    #define s_REWARD_RAY_BIG_MAMA ITF_GET_STRINGID_CRC(REWARD_RAY_BIG_MAMA,3039754095)
    #define s_REWARD_RAY_MILK_MOUTH ITF_GET_STRINGID_CRC(REWARD_RAY_MILK_MOUTH,1996477754)
    #define s_REWARD_RAY_FULL_MOUTH ITF_GET_STRINGID_CRC(REWARD_RAY_FULL_MOUTH,3342677759)
    #define s_REWARD_RAY_THE_JAW ITF_GET_STRINGID_CRC(REWARD_RAY_THE_JAW,1619709071)
    #define s_REWARD_RAY_BETILLA_BACK ITF_GET_STRINGID_CRC(REWARD_RAY_BETILLA_BACK,2505787543)
    #define s_REWARD_RAY_BEAUTIFUL_BEATS ITF_GET_STRINGID_CRC(REWARD_RAY_BEAUTIFUL_BEATS,976218004)
    #define s_REWARD_RAY_FOODIE_FAIRY ITF_GET_STRINGID_CRC(REWARD_RAY_FOODIE_FAIRY,2273787826)
    #define s_REWARD_RAY_MERMAID ITF_GET_STRINGID_CRC(REWARD_RAY_MERMAID,1484153912)
    #define s_REWARD_RAY_NYMPHS_ROCK ITF_GET_STRINGID_CRC(REWARD_RAY_NYMPHS_ROCK,2912888128)
    #define s_REWARD_RAY_BUBBLIZER ITF_GET_STRINGID_CRC(REWARD_RAY_BUBBLIZER,2278706554)
    #define s_REWARD_RAY_SPEEDY  ITF_GET_STRINGID_CRC(REWARD_RAY_SPEEDY,2192179138)
    #define s_REWARD_RAY_TURBO ITF_GET_STRINGID_CRC(REWARD_RAY_TURBO,2357523315)
    #define s_REWARD_RAY_NITRO ITF_GET_STRINGID_CRC(REWARD_RAY_NITRO,4183817537)
    #define s_REWARD_RAY_ELEC_FRIEND ITF_GET_STRINGID_CRC(REWARD_RAY_ELEC_FRIEND,2008558216)
    #define s_REWARD_RAY_ELEC_HERO ITF_GET_STRINGID_CRC(REWARD_RAY_ELEC_HERO,3521403621)
    #define s_REWARD_RAY_ELEC_LEGEND ITF_GET_STRINGID_CRC(REWARD_RAY_ELEC_LEGEND,14947537)
    #define s_REWARD_RAY_FINISH_WITH_COSTUME ITF_GET_STRINGID_CRC(REWARD_RAY_FINISH_WITH_COSTUME,3974782933)


    //_____________________________________________________________________
    //      RewardManager
    // Used to store all datas necessary to unlock all rewards
    // Can count a lot a values, use timer, game session ... and all players
    //_____________________________________________________________________
    class RewardManager : public TemplateSingleton<RewardManager>
    {
    public:
        RewardManager();
        virtual ~RewardManager();

        enum Players { p1=0,p2,p3,p4};
        static const u32 PlayersMax = 4;

        // All tags are here, even the timers (can be usefull)
        typedef u32 Tag;
        typedef u32 Timer;

        // Stats ....
        struct sTimer
        {
            sTimer() : m_start(0.0f), m_total(0.0f), m_running(bfalse), m_actif(bfalse),m_updated(bfalse) {}
            f32 m_start, m_total;
            bbool m_running, m_actif;
            bbool m_updated;
        };
        struct sTag
        {
            sTag(): m_value(0.0f), m_updated(0) {}
            f32   m_value;
            bbool m_updated;
        };
        struct sAction
        {
            sAction() : m_actif(bfalse), m_enterSession(0.0f), m_leaveSession(0.0f) {}
            SafeArray <sTag>    m_tags;
            SafeArray <sTimer>  m_timers;

            bbool   m_actif;
            f32     m_enterSession ,m_leaveSession;
        };
        struct sEvent
        {
            Tag m_tag;
            StringID m_param;
        };

        struct GameSession
        {
            GameSession() : m_Start(0.0), m_Total(0.0), m_LengthInSec(0), m_isRunning(bfalse), m_isPaused(bfalse) {}

            f64     m_Start;
            f64     m_Total;
            u32     m_LengthInSec;
            bbool   m_isRunning, m_isPaused;
            sAction m_actions[RewardManager::PlayersMax];
            SafeArray<sEvent>   m_events;
        };

        class SaveSession
        {
            DECLARE_SERIALIZE();

            SafeArray<f32>  m_tags;
            SafeArray<f32>  m_timersTotal;
        };

        // ----------------------------------------------------------------------
        // Reset all the stats (called after the compute)
        void    Reset();

        // ----------------------------------------------------------------------
        // Actions recording -----------------
        void    Action_TimerStart(Timer _timer, u32 _player);
        void    Action_TimerStop(Timer _timer, u32 _player);
        void    Action_TimerPause(Timer _timer, u32 _player, bbool _forceUpdateToTotalSession = bfalse);
        void    Action_TimerResume(Timer _timer, u32 _player, bbool _forceStartIfNeeded = bfalse);
        void    Action_TimerReset(Timer _timer, u32 _player);
        void    Action_New(Tag _tag, u32 _player, f32 _count = 1.f );
        void    Action_Set(Tag _tag, u32 _player, f32 _value);
        void    Action_AddPlayer(u32 _player);
        void    Action_RemovePlayer(u32 _player);
        void    Action_GlobalEvent(Tag _tag, StringID _id = StringID::Invalid);

        // ----------------------------------------------------------------------
        // Sessions recording -----------------
        void    Session_New();          // Start a new session and reset all stats
        void    Session_Pause();        // Pause all timers
        void    Session_Resume();       // Resume all timers
        void    Session_Stop();         // Stop current session, and backup it in the previousSession
        void    Session_Reset();        // Reset all stats of current session, and keep the players actives states
        GameSession*    Session_GetCurrent()    { return &m_currentSession; }
        GameSession*    Session_GetPrevious()   { return &m_previousSession; }
        GameSession*    Session_GetTotal()      { return &m_totalSessions; }

        // ----------------------------------------------------------------------
        // Getter
        f32     GetTagValue(GameSession* _gameSession, Tag _tag, u32 _player);
        bbool   IsTagValueUpdated(GameSession* _gameSession, Tag _tag, u32 _player);
        bbool   GetIsSessionRunning(GameSession* _gameSession);
        u32     GetSessionLengthInSec(GameSession* _gameSession);
        bbool   GetIsPlayerActif(GameSession* _gameSession, u32 _player);
        bbool   IsTimerUpdated(GameSession* _gameSession, Timer _timer, u32 _player);
        f64     GetTimeElapsedTimer(GameSession* _gameSession, Timer _timer, u32 _player);
        bbool   IsGlobalEventTriggered(Tag _tag);
        StringID GetGlobalEventID(Tag _tag);
        // ----------------------------------------------------------------------
        f32     GetTotalOf (Tag _tag, u32 _player, bbool _fromAllSessions = btrue) const
        {
            ITF_ASSERT(_player<PlayersMax);
            if(_player<PlayersMax)
                return _fromAllSessions ? m_totalSessions.m_actions[_player].m_tags[_tag].m_value :
                m_currentSession.m_actions[_player].m_tags[_tag].m_value;
            else
                return -1.f;
        }

        void    resetTotalOf( Tag _tag, u32 _player )
        {
            ITF_ASSERT(_player<PlayersMax);
            if(_player<PlayersMax)
            {
                m_currentSession.m_actions[_player].m_tags[_tag].m_value = 0.0f;
                m_currentSession.m_actions[_player].m_tags[_tag].m_updated = btrue;
            }
        }

        friend class RewardAdapter;
        bbool   m_updated;
        virtual void    update();
        void    postUpdate();

        void    setTagsCount(u32 tagsCount) { m_tagsCount = tagsCount; }
        void    setTimersCount(u32 timersCount) { m_timersCount = timersCount; }
        void    computeRewardManager();

        // Save
        bbool   backupSaveSession (SaveSession* _pSaveSession, u32 _playerToBackup);
        bbool   setSaveSessionFromSave (SaveSession* _pSaveSession, u32 _playerToSet);

        void    disable() { m_enable = bfalse; }
        void    enable() { m_enable = btrue; }
        bbool   isEnable() const { return m_enable; }

    private:
        void    IamUpdated() { m_updated = btrue; }

        void    clearEvents();

        GameSession  m_currentSession, m_previousSession, m_totalSessions;

        sTimer*  getTimer(Timer _timer, u32 _player);
        sTimer*  getTimerFromTotalSession(Timer _timer, u32 _player);
        void    stopAllTimers();
        void    pauseAllTimers();
        void    resumeAllTimers();

        u32     getSafePlayerIndex(u32 _player);

        u32     m_tagsCount;
        u32     m_timersCount;
        bbool   m_enable;
        bbool   m_computed;
    };


    class RewardAdapter : public TemplateSingleton<RewardAdapter>
    {
    public:
        RewardAdapter();
        virtual ~RewardAdapter(){}

        virtual bbool       unlock (RewardID _id)=0;
        virtual bbool       isLocked (RewardID _id) const =0;

        virtual void        init(bbool enable);
        virtual void        term(){}
        virtual void        loadManager(){}

        virtual u32         getRewardsCount() const = 0;
        virtual void        update();

        bbool               isEnable() const { return m_enable; }
        void                setEnable(bbool enable) { m_enable = enable; }
        virtual void        addReward(const RewardID& _id, u32 _plateformID);

        virtual void        displayRewards() {}

        virtual bbool       needsSaveSystemToWork() const { return btrue; }

        void                setMainPlayerIndex(u32 dwIndex) { m_mainPlayerIndex = dwIndex; }
        void                setMainPlayerIndexIsInvalid() { m_mainPlayerIndex = U32_INVALID; }

        // To handle all platform
        typedef ITF_MAP<RewardID, u32> RewardsMap;
        RewardsMap			m_rewardMap;
        virtual void    startActivity(const StringID& _activity) {};
        virtual void    stopActivity(const StringID& _activity){};
        virtual void    resumeActivity(const StringID& _activity){};
        virtual void    terminateActivity(){};
    protected:
        bbool               m_enable;
        u32                 m_mainPlayerIndex;


        u32             getRewardPlateform(const RewardID& _id)const;
    };

#define REWARD_ADAPTER          RewardAdapter::getptr()
#define REWARD_MANAGER          RewardManager::getptr()

} // namespace ITF

#endif //_ITF_REWARDADAPTER_H_
