#ifndef _ITF_RAY_CHEATMANAGER_H_
#define _ITF_RAY_CHEATMANAGER_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_RAY_POWERUPMANAGER_H_
#include "rayman/gameplay/Ray_PowerUpManager.h"
#endif //_ITF_RAY_POWERUPMANAGER_H_

namespace ITF
{
    #define RAY_CHEATMANAGER static_cast<Ray_CheatManager*>(CHEATMANAGER)

    class Ray_CheatManager : public CheatManager
    {
    public:

        Ray_CheatManager();
        virtual ~Ray_CheatManager();

        virtual void        init( const class CommandArgs& _args );
        virtual void        update(f32 _dt);

        void                forceAllPlayersTogether( bbool _val );
        bbool               getIsForceAllPlayersTogether() const { return m_allPlayersTogether; }

        void                setCheatAllCostumesUnlocked( bbool _val ) { m_unlockAllCostumes = _val; }
        bbool               getIsCheatAllCostumesUnlocked() const { return m_unlockAllCostumes; }

        void                setCheatTeethSequence( bbool _val ) { m_teethSequence = _val; }
        bbool               getIsCheatTeethSequence() const { return m_teethSequence; }

        void                setCheatTimeAttack( bbool _val ) { m_timeAttackEnabled = _val; }
        bbool               getIsCheatTimeAttack() const { return m_timeAttackEnabled; }

        bbool               getAllWorldMap() const { return m_allWorldMap; }
        void                setAllWorldMap(bbool _val) { m_allWorldMap = _val; }

        bbool               getAllSkullTeeth() const { return m_allSkullTeeth; }
        void                setAllSkullTeeth(bbool _val) { m_allSkullTeeth = _val; }

        //CASA::VITA::TARIK::FRESCO GAMEPLAY
        void                setActivateDebugFresco(bbool _val) { m_activateDebugFresco = _val; }
        bbool               getActivateDebugFresco() const { return m_activateDebugFresco; }

        virtual void        onReceive( u32 deviceID /* player */, f32 axis, const StringID& action ) ;
        u32                 activityCheatIndex = 0;
        ITF_VECTOR<StringID> activity;

    private:
        virtual void        teleportToCheckpoint(GameManager::teleportTarget _target);

        bbool               m_allPlayersTogether;
        bbool               m_unlockAllCostumes;
        bbool               m_teethSequence;
        bbool               m_timeAttackEnabled;
        bbool               m_allWorldMap;
        bbool               m_allSkullTeeth;

        bbool               m_activateDebugFresco;//CASA::VITA::TARIK::FRESCO GAMEPLAY
    };
}

#endif // _ITF_RAY_CHEATMANAGER_H_

