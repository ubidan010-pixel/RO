#include "precompiled_RewardAdapter_PS5.h"

#include <libsysmodule.h>

#include "adapters/AccountAdapter_PS5/AccountAdapter_PS5.h"
#include "np_universal_data_system_codegen.h"
#include "rayman/gameplay/Ray_GameManager.h"

#ifndef _ITF_REWARDADAPTER_PS5_H__
#include "adapters/RewardAdapter_PS5/RewardAdapter_PS5.h"
#endif	//_ITF_REWARDADAPTER_PS5_H__

#ifndef _ITF_NETWORKSERVICES_PS5_H_
// #include "engine/networkservices/PS3/NetworkServices_PS5.h"
#endif	//_ITF_NETWORKSERVICES_PS5_H_

#ifndef _ITF_TASKMANAGER__H_
#include "engine/taskmanager/TaskManager.h"
#endif	//_ITF_TASKMANAGER__H_

#ifndef _ITF_TRCADAPTER_PS5_H_
#include "adapters/TRCAdapter_PS5/TRCAdapter_PS5.h"
#endif	//_ITF_TRCADAPTER_PS5_H_

#ifndef _ITF_SYSTEMADAPTER_PS5_H_
#include "adapters/SystemAdapter_PS5/SystemAdapter_PS5.h"
#endif	//_ITF_SYSTEMADAPTER_PS5_H_

#ifndef _ITF_ERRORHANDLER_H_
#include "core/error/ErrorHandler.h"
#endif	//_ITF_ERRORHANDLER_H_

namespace ITF
{
#ifndef ITF_FINAL
    // #define REWARD_PS5_READONLY
#endif

    RewardAdapter_PS5::RewardAdapter_PS5()
        : m_taskManager(NULL),
          m_currentState(STATE_NOT_INITIALIZED),
          m_nextState(STATE_NOT_INITIALIZED),
          m_contextTrophy(SCE_NP_TROPHY2_INVALID_CONTEXT),
          m_contextUniversalDataSystem(SCE_NP_UNIVERSAL_DATA_SYSTEM_INVALID_CONTEXT)
    {
        ITF_VERIFY_SCE_CALL(sceSysmoduleLoadModule(SCE_SYSMODULE_NP_TROPHY2));
        ITF_VERIFY_SCE_CALL(sceSysmoduleLoadModule(SCE_SYSMODULE_NP_UNIVERSAL_DATA_SYSTEM));
        memset(m_trophy2DataArray, 0, sizeof(m_trophy2DataArray));
        addAcitiviy(s_JU_A, String("Jibberish_Jungle"));
        addAcitiviy(s_JU_B, String("Ticklish_Temple"));
        addAcitiviy(s_MU_A, String("Desert_Dijiridoos"));
        addAcitiviy(s_MU_B, String("Grumbling_Grottos"));
        addAcitiviy(s_FO_A, String("Gourmand_Land"));
        addAcitiviy(s_FO_B, String("Lucious_Lakes"));
        addAcitiviy(s_OC_A, String("Sea_Serendipity"));
        addAcitiviy(s_OC_B, String("Angsty_Abyss"));
        addAcitiviy(s_MO_A, String("Mystical_Pique"));
        addAcitiviy(s_MO_B, String("Moody_Clouds"));
    }
    void RewardAdapter_PS5::addAcitiviy(StringID _id,String _name) {
        STD_PAIR< StringID, String> pair;
        pair.first = _id;
        pair.second = _name;
        m_activitiesMapping.insert(pair);
    }
    void RewardAdapter_PS5::init_rewardList()
    {
        addReward(s_REWARD_RAY_BETILLA_BACK, 1);
        addReward(s_REWARD_RAY_BEAUTIFUL_BEATS, 2);
        addReward(s_REWARD_RAY_FOODIE_FAIRY, 3);
        addReward(s_REWARD_RAY_MERMAID, 4);
        addReward(s_REWARD_RAY_NYMPHS_ROCK, 5);
        addReward(s_REWARD_RAY_BIG_MAMA, 6);
        addReward(s_REWARD_RAY_SCOUT, 7);
        addReward(s_REWARD_RAY_Explorer, 8);
        addReward(s_REWARD_RAY_DR_LIVIDSTONE, 9);
        addReward(s_REWARD_RAY_SPEEDY, 10);
        addReward(s_REWARD_RAY_TURBO, 11);
        addReward(s_REWARD_RAY_NITRO, 12);
        addReward(s_REWARD_RAY_ELEC_FRIEND, 13);
        addReward(s_REWARD_RAY_ELEC_HERO, 14);
        addReward(s_REWARD_RAY_ELEC_LEGEND, 15);
        addReward(s_REWARD_RAY_MILK_MOUTH, 16);
        addReward(s_REWARD_RAY_FULL_MOUTH, 17);
        addReward(s_REWARD_RAY_THE_JAW, 18);
        addReward(s_REWARD_RAY_POP_POP_BOOM, 19);
        addReward(s_REWARD_RAY_BUBBLIZER, 20);
        addReward(s_REWARD_RAY_IAM_BACK, 21);
        addReward(s_REWARD_RAY_PAINLESS, 22);
        addReward(s_REWARD_RAY_VACUUM_KNACK, 23);
        addReward(s_REWARD_RAY_NOPANIC, 24);
        addReward(s_REWARD_RAY_FINISH_WITH_COSTUME, 25);
        addReward(s_REWARD_RAY_HOVER_HAPPY, 26);
        addReward(s_REWARD_RAY_CRUSHER, 27);
        // addReward(s_REWARD_RAY_PEACEFUL_DREAMS, 28); --> removed
        addReward(s_REWARD_RAY_CRUSH_COMBO, 28);
        addReward(s_REWARD_RAY_SPRINTER, 29);
        addReward(s_REWARD_RAY_FISHER_KING, 30);
        addReward(s_REWARD_RAY_KUNGFUCOMBO, 31);
        addReward(s_REWARD_RAY_HYPERSPEED, 32);
        addReward(s_REWARD_RAY_BOING, 33);
        addReward(s_REWARD_RAY_BLUE_BARON, 34);
        addReward(s_REWARD_RAY_SURVIVEDPIRANHAPOND, 35);
        addReward(s_REWARD_RAY_BACK_AT_YOU, 36);
    }

    void RewardAdapter_PS5::update()
    {
        RewardAdapter::update();
        if (m_taskManager)
        {
            m_taskManager->update();
            stateUpdate();
        }
    }

    void RewardAdapter_PS5::stateUpdate()
    {
        if (m_currentState != m_nextState)
        {
            m_currentState = m_nextState;
            switch (m_currentState)
            {
            case STATE_INITIALIZE:
                {
                    InitiateTrophySystem();
                    break;
                }
            case STATE_REGISTER_TROPHY:
                    registerContexAndGetUnlockTrophies();
                break;
            case STATE_REGISTER_UDS:
                {
                    registerUDSContext();
                    break;
                }
            case STATE_READY:
                break;
            default:
                break;
            }
        }
    }

    void RewardAdapter_PS5::InitiateTrophySystem()
    {
        registerUnlockCalback();
        initializeNpUniversalDataSystemLib();
        createContexts();
        switchState(STATE_REGISTER_TROPHY);
    }

    bbool RewardAdapter_PS5::unlock(RewardID _id)
    {
        if (m_currentState == STATE_READY)
        {
            SceNpTrophy2Id trophyId = getRewardPlateform(_id);
            if (isLocked(_id))
            {
                if (trophyId != U32_INVALID)
                {
                    // @TODO need to rework after alpha when all trophies are set up.
                    unlockTrophy(trophyId - 1);
                    return true;
                }
            }
            else
            {
                LOG("#Trophy id %d was unloked before", trophyId);
            }
        }
        return false;
    }

    bbool RewardAdapter_PS5::isLocked(RewardID _id) const
    {
        if (m_currentState == STATE_READY)
        {
            SceNpTrophy2Id trophyId = getRewardPlateform(_id);
            if (trophyId != U32_INVALID)
            {
                return !(m_trophy2DataArray[trophyId].unlocked);
            }
        }
        return false;
    }

    void RewardAdapter_PS5::init(bbool _enable)
    {
        if (_enable)
        {
            if (!m_taskManager)
                m_taskManager = newAlloc(mId_System, TaskManager());
            initForBoot();
        }
        RewardAdapter::init(_enable);
    }

    void RewardAdapter_PS5::term()
    {
        RewardAdapter::term();
        if (m_taskManager)
        {
            SF_DEL(m_taskManager);
            m_taskManager = NULL;
        }
        if (m_contextTrophy != SCE_NP_TROPHY2_INVALID_CONTEXT)
        {
            sceNpTrophy2DestroyContext(m_contextTrophy);
        }
        if (m_contextUniversalDataSystem != SCE_NP_UNIVERSAL_DATA_SYSTEM_ERROR_INVALID_CONTEXT)
        {
            sceNpUniversalDataSystemDestroyContext(m_contextTrophy);
        }
        sceNpTrophy2UnregisterUnlockCallback();
        sceNpUniversalDataSystemTerminate();
    }

    int RewardAdapter_PS5::initForBoot()
    {
        init_rewardList();
        switchState(STATE_INITIALIZE);
        return true;
    }

    int RewardAdapter_PS5::registerUnlockCalback()
    {
        int ret = sceNpTrophy2RegisterUnlockCallback(unlockCallback, this);

        if (ret < 0)
        {
            ITF_ERROR("Cant registerUnlockCalback %d", ret);
        }
        return SCE_OK;
    }

    int RewardAdapter_PS5::createContexts()
    {
        SceUserServiceUserId initialUserId = SCE_USER_SERVICE_USER_ID_INVALID;
        int32_t ret = sceUserServiceGetInitialUser(&initialUserId);
        ret = sceNpTrophy2CreateContext(&m_contextTrophy, initialUserId, 0, 0);
        if (ret < 0)
        {
            ITF_ERROR("#Trophy Cant createTrophyContext %d", ret);
        }
        ret = sceNpUniversalDataSystemCreateContext(&m_contextUniversalDataSystem, initialUserId, 0, 0);
        if (ret < 0)
        {
            LOG("#Trophy sceNpUniversalDataSystemCreateContext failed %d", ret);
            return ret;
        }
        //   switchState(STATE_REGISTERING);
        return SCE_OK;
    }

    void RewardAdapter_PS5::unlockCallback(SceNpTrophy2Context _context, SceNpTrophy2Id _trophyId, void* _userdata)
    {
        LOG("#Trophy Unlocked Trophy %d", _trophyId);
    }

    SceNpTrophy2Handle RewardAdapter_PS5::createTrophyHandler()
    {
        SceNpTrophy2Handle m_handler = SCE_NP_TROPHY2_INVALID_HANDLE;
        int ret = sceNpTrophy2CreateHandle(&m_handler);
        if (ret < 0)
        {
            ITF_ERROR("#Trophy Cant create Handler %d", ret);
        }
        return m_handler;
    }

    SceNpUniversalDataSystemHandle RewardAdapter_PS5::createUniversalDataSystemHandler()
    {
        SceNpUniversalDataSystemHandle handler = SCE_NP_UNIVERSAL_DATA_SYSTEM_INVALID_HANDLE;
        int ret = sceNpUniversalDataSystemCreateHandle(&handler);
        if (ret < 0)
        {
            LOG("#Trophy sceNpUniversalDataSystemCreateHandle failed %d", ret);
        }
        return handler;
    }

    void RewardAdapter_PS5::registerContexAndGetUnlockTrophies()
    {
        RegisterTrophyData* data = (RegisterTrophyData*)Memory::malloc(sizeof(RegisterTrophyData));
        data->context = m_contextTrophy;
        data->handler = createTrophyHandler();
        data->adapter = this;
        m_taskManager->pushTask(task_registerAndUnlockedTrophy, task_registerAndUnlockedTrophyCallback, (void*)data);
    }

    i32 RewardAdapter_PS5::task_registerAndUnlockedTrophy(void* _param)
    {
        RegisterTrophyData* pDatas = (RegisterTrophyData*)_param;
        int ret = sceNpTrophy2RegisterContext(pDatas->context, pDatas->handler, 0);
        if (ret < 0)
        {
            ITF_ERROR("#Trophy Can't Register Context %d", ret);
            return ret;
        }
        ret = sceNpTrophy2GetTrophyInfoArray(pDatas->context, pDatas->handler, 0, SCE_NP_TROPHY2_NUM_MAX, nullptr, pDatas->adapter->m_trophy2DataArray,
                                             &pDatas->adapter->m_trophyCount);
        if (ret < 0)
        {
            ITF_ERROR("#Trophy Can't get trophies status %d", ret);
            return ret;
        }
        return SCE_OK;
    };

    void RewardAdapter_PS5::task_registerAndUnlockedTrophyCallback(void* _param, i32 res)
    {
        RegisterTrophyData* pDatas = (RegisterTrophyData*)_param;
        if (res == SCE_OK)
        {
            LOG("#Trophy Register Context + Get TrophyStatus Success fully");
            pDatas->adapter->switchState(STATE_REGISTER_UDS);
        }
        else
        {
            ITF_ERROR("#Trophy Register Context Fail %d", res);
        }
        sceNpTrophy2DestroyHandle(pDatas->handler);
        Memory::free(pDatas);
    };


    void RewardAdapter_PS5::registerUDSContext()
    {
        RegisterUDSData* data = (RegisterUDSData*)Memory::malloc(sizeof(RegisterUDSData));
        data->context = m_contextUniversalDataSystem;
        data->handler = createUniversalDataSystemHandler();
        data->adapter = this;
        m_taskManager->pushTask(task_registerUDSContext, task_registerUDSContextCallback, (void*)data);
    }

    i32 RewardAdapter_PS5::task_registerUDSContext(void* _param)
    {
        RegisterUDSData* pDatas = (RegisterUDSData*)_param;
        int ret = sceNpUniversalDataSystemRegisterContext(pDatas->context, pDatas->handler, 0);
        if (ret < 0)
        {
            return ret;
        }
        return SCE_OK;
    };

    void RewardAdapter_PS5::task_registerUDSContextCallback(void* _param, i32 res)
    {
        RegisterUDSData* pDatas = (RegisterUDSData*)_param;
        if (res == SCE_OK)
        {
            LOG("#Trophy Register UDS Context Success fully");
            pDatas->adapter->switchState(STATE_READY);
        }
        else
        {
            ITF_ERROR("#Trophy Register UDS Context Fail %d", res);
        }
        sceNpUniversalDataSystemDestroyHandle(pDatas->handler);
        Memory::free(pDatas);
    };

    void RewardAdapter_PS5::unlockTrophy(SceNpTrophy2Id _id)
    {
        UnlockTrophyData* data = (UnlockTrophyData*)Memory::malloc(sizeof(UnlockTrophyData));
        data->context = m_contextUniversalDataSystem;
        data->handler = createUniversalDataSystemHandler();
        data->adapter = this;
        data->trophyId = _id;
        m_taskManager->pushTask(task_unlockTrophyTask, task_unlockTrophyTaskCallback, (void*)data);
    }

    i32 RewardAdapter_PS5::task_unlockTrophyTask(void* _param)
    {
        UnlockTrophyData* pDatas = (UnlockTrophyData*)_param;
        int ret = sceNpUniversalDataSystemCodegenPost_UnlockTrophy(pDatas->context, pDatas->handler, pDatas->trophyId);
        if (ret == SCE_OK)
        {
            LOG("#Trophy Unlocked Trophy Success On Server %d", pDatas->trophyId);
            return SCE_OK;
        }
        else
        {
            ITF_ERROR("#Trophy Unlocked Trophy Fail %d", ret);
            return ret;
        }
    };

    void RewardAdapter_PS5::task_unlockTrophyTaskCallback(void* _param, i32 _res)
    {
        UnlockTrophyData* pDatas = (UnlockTrophyData*)_param;
        if (_res == SCE_OK)
        {
            pDatas->adapter->m_trophy2DataArray[pDatas->trophyId].unlocked = true;
            LOG("#Trophy Unlocked CALLBACK Trophy Success fully %d", pDatas->trophyId);
        }
        sceNpUniversalDataSystemDestroyHandle(pDatas->handler);
        Memory::free(pDatas);
    };

    void RewardAdapter_PS5::activityAction(const String& _activityId, ActivityAction _action)
    {
        LOG("#Acitivty %S", _activityId.cStr());
        ActivityData* data = new ActivityData();
        data->context = m_contextUniversalDataSystem;
        data->handler = createUniversalDataSystemHandler();
        data->adapter = this;
        data->activity = _activityId;
        data->action = _action;
        m_taskManager->pushTask(task_activityAction, task_activityActionCallback, (void*)data);
    }

    const String& RewardAdapter_PS5::getWorldByStringID(const StringID& _stringID)
    {
        ITF_MAP<StringID, String>::const_iterator it = m_activitiesMapping.find(_stringID);

        if (it != m_activitiesMapping.end())
        {
            return it->second;
        }
        return String::emptyString;
    }

    i32 RewardAdapter_PS5::task_activityAction(void* _param)
    {
        const ActivityData* pDatas = (ActivityData*)_param;
        int ret = SCE_OK;
        ActivityAction action = pDatas->action;
        switch (action)
        {
        case Start:
            ret = sceNpUniversalDataSystemCodegenPostactivityStart(pDatas->context, pDatas->handler, StringConverter(pDatas->activity).getChar());
            break;
        case Resume:
            {
                SceNpUniversalDataSystemCodegenactivityResumeOption option;
                sceNpUniversalDataSystemCodegenactivityResumeOptionInit(&option);
                const char** unlockedWorld;
                const char** currentWorld;
                u32 unlockedCount = 0;
                u32 currentCount = 0;
                pDatas->adapter->getWorldsByState(unlockedWorld,unlockedCount,SPOT_STATE_COMPLETED);
                pDatas->adapter->getWorldsByState(currentWorld,currentCount,SPOT_STATE_OPEN);
                sceNpUniversalDataSystemCodegenactivityResumeOptionSetcompletedActivities(&option, unlockedWorld, unlockedCount);
                sceNpUniversalDataSystemCodegenactivityResumeOptionSetinProgressActivities(&option, currentWorld, currentCount);
                ret = sceNpUniversalDataSystemCodegenPostactivityResumeWithOption(pDatas->context, pDatas->handler, StringConverter(pDatas->activity).getChar(), &option);
                SF_DEL_ARRAY(unlockedWorld);
                SF_DEL_ARRAY(currentWorld);
                break;
            }

        case End:
            SceNpUniversalDataSystemCodegenactivityEndOption option;
            sceNpUniversalDataSystemCodegenactivityEndOptionInit(&option);
            sceNpUniversalDataSystemCodegenactivityEndOptionSetoutcome(&option, "completed");
            ret =  sceNpUniversalDataSystemCodegenPostactivityEndWithOption(pDatas->context, pDatas->handler, StringConverter(pDatas->activity).getChar(),&option);
            break;
        case Terminate:
            sceNpUniversalDataSystemCodegenPostactivityTerminate(pDatas->context,pDatas->handler);
            break;
        default: break;
        }
        if (ret == SCE_OK)
        {
            LOG("#Activity action Success %d with id %S", pDatas->action, pDatas->activity.cStr());
            return SCE_OK;
        }
        else
        {
            ITF_ERROR("#Activity FAILED action %d with id %s %d", pDatas->action, pDatas->activity.cStr(), ret);
            return ret;
        }
    }
    void  RewardAdapter_PS5::getWorldsByState(const char**& _outArray,u32& _outCount,u32 _state)  {
        ITF_VECTOR<const char*> unlockedWorld;

        for (const auto& k : m_activitiesMapping)
        {
            StringID mapID = k.first;
            if (RAY_GAMEMANAGER->getWMSpotState(mapID) == static_cast<ESpot_State>(_state)) {
                String worldName = getWorldByStringID(mapID);
                if (!worldName.isEmpty()) {
                    unlockedWorld.push_back(worldName.getCharCopy());
                }
            }
        }
        _outCount = unlockedWorld.size();
        _outArray = new const char* [_outCount];

        for (u32 i = 0; i < _outCount; i++)
        {
            _outArray[i] = unlockedWorld[i];  // copy pointer
        }
    }
    void RewardAdapter_PS5::task_activityActionCallback(void* _param, i32 res)
    {
        ActivityData* pDatas = (ActivityData*)_param;
        sceNpUniversalDataSystemDestroyHandle(pDatas->handler);
        Memory::free(pDatas);
    };

    void RewardAdapter_PS5::startActivity(const StringID& _stage)
    {
        activityAction( getWorldByStringID(_stage), Start);
    }

    void RewardAdapter_PS5::stopActivity(const StringID& _stage)
    {
        activityAction(getWorldByStringID(_stage), End);
    }

    void RewardAdapter_PS5::resumeActivity(const StringID& _stage)
    {
        activityAction(MAIN_ACTIVIYTY, Resume);
    }

    void RewardAdapter_PS5::terminateActivity()
    {
        activityAction(MAIN_ACTIVIYTY, Terminate);
    }

    void RewardAdapter_PS5::switchState(State _newState)
    {
        if (m_currentState != _newState)
        {
            m_nextState = _newState;
        }
    }

    u32 RewardAdapter_PS5::getRewardsCount() const
    {
        return m_trophyCount;
    }

    i32 RewardAdapter_PS5::initializeNpUniversalDataSystemLib()
    {
        int ret;
        SceNpUniversalDataSystemInitParam param;
        memset(&param, 0, sizeof(param));
        param.size = sizeof(param);
        param.poolSize = kUdsMemSize;
        ret = sceNpUniversalDataSystemInitialize(&param);
        if (ret < 0)
        {
            ITF_ERROR("#Trophy initializeNpUniversalDataSystemLibl %d", ret);
            return ret;
        }
        return SCE_OK;
    }
} // namespace ITF
