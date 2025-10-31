#ifndef _ITF_RAY_GAMEOPTIONPERSISTENCE_H_
#define _ITF_RAY_GAMEOPTIONPERSISTENCE_H_

#ifndef _ITF_RAY_GAMEOPTIONMANAGER_H_
#include "Ray_GameOptionManager.h"
#endif //_ITF_RAY_GAMEOPTIONMANAGER_H_
namespace ITF
{
class Ray_GameOptionPersistence
{
public:
    enum State
    {
        State_Idle,
        State_Enumerating,
        State_Saving,
        State_Loading,
        ENUM_FORCE_SIZE_32(0)
    };

    enum Result
    {
        Result_None,
        Result_SaveSuccess,
        Result_SaveFailed,
        Result_LoadSuccess,
        Result_LoadFailed,
        Result_LoadNotFound,
        ENUM_FORCE_SIZE_32(1)
    };

    typedef void (*CompletionCallback_t)(Result _result);
    Ray_GameOptionPersistence(Ray_GameOptionManager* optionManager);
    ~Ray_GameOptionPersistence();
    void init();
    bbool startSaveOptions(u32 playerIndex, CompletionCallback_t callback = NULL);
    bbool startLoadOptions(u32 playerIndex, CompletionCallback_t callback = NULL);
    void update();
    bbool isBusy() const { return m_state != State_Idle; }
    State getState() const { return m_state; }
    Result getLastResult() const { return m_lastResult; }
    
    // Platform support check
    static bbool isPlatformSupported();
    
    const String8& getSaveBaseName() const { return m_saveBaseName; }
    void setSaveBaseName(const String8& baseName) { m_saveBaseName = baseName; }
    u32 getSaveSlotIndex() const { return m_saveSlotIndex; }
    void setSaveSlotIndex(u32 slotIndex) { m_saveSlotIndex = slotIndex; }

private:
    void setState(State newState);
    void setResult(Result result);
    void fireCallback(Result result);

    Ray_GameOptionManager* m_optionManager;
    State m_state;
    Result m_lastResult;
    u32 m_currentPlayerIndex;
    CompletionCallback_t m_currentCallback;
    String8 m_saveBaseName;
    u32 m_saveSlotIndex;
};

} // namespace ITF

#endif //_ITF_RAY_GAMEOPTIONPERSISTENCE_H_
