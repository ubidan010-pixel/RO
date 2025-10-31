#include "precompiled_engine.h"

#ifndef ITF_FINAL

#ifndef _ITF_TESTMANAGER_H_
#include "engine/test/testmanager.h"
#endif //_ITF_TESTMANAGER_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/Actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_SYSTEMADAPTER_
#include "core/AdaptersInterfaces/SystemAdapter.h"
#endif //_ITF_SYSTEMADAPTER_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_CHECKPOINTCOMPONENT_H_
#include "gameplay/components/misc/CheckpointComponent.h"
#endif //_ITF_CHECKPOINTCOMPONENT_H_


namespace ITF
{
   
TestManager::TestManager():m_isRunning(bfalse),m_pStateTest(NULL)
{   
    
    
}

TestManager::~TestManager()
{
    SF_DEL(m_pStateTest);
}

////-------------------------------------------------------------------------------------------------------

void TestManager::start(const String& _szName)
{
    static String TELEPORT = "teleport";
    static String CAMERA_RUN = "camera_run";
        
    if (_szName  == TELEPORT)
    {
        m_isRunning     = btrue;
        m_pStateTest    = new TeleportStateTest();
    }
    else if (_szName  == CAMERA_RUN)
    {
        m_isRunning     = btrue;
        m_pStateTest    = new CameraRunStateTest();
    }
}


////-------------------------------------------------------------------------------------------------------
//Teleport;
void TeleportStateTest::onEnter()
{

}

void TeleportStateTest::onExit()
{

}

void TeleportStateTest::update(f32 _dt)
{
    if (onENTER == m_State)
    {
        for (u32 i = 0; i < GAMEMANAGER->getMaxPlayerCount(); i++)
        {
            Actor* actor = GAMEMANAGER->getActivePlayer(i);
            if (!actor)
            {
                continue;
            }
            else
            {
                //take the first player available
                m_ActorID = actor->getRef();
                actor->pauseComponent(PhysComponent::GetClassCRCStatic());
                break;
            }
        }
        
        GAMEMANAGER->teleportToFirstCheckpoint(bfalse);

        const ObjectRef& currentWorld = GAMEMANAGER->getCurrentWorld();

        //Next Checkpoint
        Actor *  pNextCheckpoint = GAMEMANAGER->getFirstCheckpoint(currentWorld);

        if ( pNextCheckpoint )
        {
            CheckpointComponent* chk = pNextCheckpoint->GetComponent<CheckpointComponent>();

            if ( chk )
            {
                pNextCheckpoint = GAMEMANAGER->getCheckpointFromIndex(currentWorld, chk->getIndex()+1,btrue);

                if (pNextCheckpoint)
                {
                    m_EndingPos = pNextCheckpoint->getPos();
                    m_Direction = m_EndingPos-m_StartingPos;
                    m_fTimeToReach = 5.0f;
                    f32 cst  =  LOGICDT;
                    f32 frameReach = m_fTimeToReach/cst;
                    m_DirFrame = m_Direction/frameReach;
                    m_State = onUPDATE;
                }
            }
        }
    }
    else
    if (onUPDATE == m_State)
    {
        {
            Actor* actor = static_cast<Actor*>(GETOBJECT(m_ActorID));

            if (actor)
            {
                Vec3d pos = actor->getPos();

                f32 radius = m_DirFrame.norm()*10.0f;//appro
               
                if (pos.IsEqual(m_EndingPos,radius))
                {
                     m_State = onENTER; //start again
                }
                else
                {
                    pos += m_DirFrame;

                    actor->setPos(pos);
                }
            }
        }
    }

}


void TestManager::update(f32 _elapsed)
{
    if (!m_isRunning)
        return;

    if (m_pStateTest)
        m_pStateTest->update(_elapsed);
}

}

#endif // ITF_FINAL
