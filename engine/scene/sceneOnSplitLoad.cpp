#include "precompiled_engine.h"

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

namespace ITF
{
//--------------------------------------------------------------------------------------------------------------------------------------------------------

void SceneSplitOnLoad::onFinish()
{
    for (ITF_VECTOR<Pickable*>::iterator iter = m_vSubScene.begin();iter!=m_vSubScene.end();iter++)
    {
        SubSceneActor* pSubSceneActor = static_cast<SubSceneActor*>(*iter);
        pSubSceneActor->onFinishLoadedFromScene();
    }

    m_templateClientHandler.freeUsedTemplates();
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------
#define PROCESS_ACTOR_MAX_TIME (10./1000.)
#define PROCESS_FRIEZE_BYFRAME 100

void SceneSplitOnLoad::process()
{
   // f32 timeStart =(f32) SYSTEM_ADAPTER->getTime();
    bbool processDone = bfalse;
    if (m_isProcessingActor)
    {
        f64 timeStart = SYSTEM_ADAPTER->getTime();
        for (;;)
        {
            Actor* pActor = static_cast<Actor*> (m_vActor[m_currentActorIndex]);
            if (pActor->onLoadedFromScene())
            {
                m_vSubScene.push_back(pActor);
            }

            //LOG(">>%s",pActor->getLua().getString8().cStr());
            m_currentActorIndex++;

            if (m_vActor.size() == m_currentActorIndex)
            {
                m_isProcessingActor  = bfalse;
                m_isProcessingFrieze = (m_vFrieze.size()>0);
                processDone = m_vFrieze.size() ? bfalse : btrue;
                break;
            }

            f64 timeToProcess = SYSTEM_ADAPTER->getTime()-timeStart;
            if(timeToProcess > PROCESS_ACTOR_MAX_TIME)
                break;
        }
    }
    else if (m_isProcessingFrieze)
    {
        u32 count  = 0;
        for (;;)
        {
            Frise* pFrise = static_cast<Frise*> (m_vFrieze[m_currentFriezeIndex]);
            pFrise->onLoaded();
            pFrise->postCheckPoint();

            m_currentFriezeIndex++;

            if (m_vFrieze.size() == m_currentFriezeIndex)
            {
               processDone = btrue;
               break;
            }

            count++;

            if (count == PROCESS_FRIEZE_BYFRAME)
            {
                break;
            }
        }
    }
    
    if (processDone)
    {
        m_isProcessingActor  = bfalse;
        m_isProcessingFrieze = bfalse;

        m_ProcessStatus   = ProcessStatusFinish;
        LOG("OnSceneCollect process done");
    }

    //f32 timeToProcess = (f32)(SYSTEM_ADAPTER->getTime()-timeStart);
    //LOG("time to process:%d %0.02f ms",count, timeToProcess*1000.0f);

}
//--------------------------------------------------------------------------------------------------------------------------------------------------------

void SceneSplitOnLoad::clear()
{
    m_ProcessStatus  = ProcessStatusOff;
    m_isProcessingActor  = bfalse;
    m_isProcessingFrieze = bfalse;
    m_currentActorIndex  = 0;
    m_currentFriezeIndex = 0;
    m_vActor.clear();
    m_vFrieze.clear();
    m_vSubScene.clear();
    m_processedScenes.clear();
}
//--------------------------------------------------------------------------------------------------------------------------------------------------------


void SceneSplitOnLoad::startProcessing()
{
    if (m_vFrieze.size() == 0 && m_vActor.size()==0)
        return;

    m_ProcessStatus = ProcessStatusProcessing;

    //manage on by frame to 
    m_currentActorIndex     = 0;
    m_currentFriezeIndex    = 0;

    m_isProcessingActor = (m_vActor.size()>0);
    m_isProcessingFrieze = !m_isProcessingActor && (m_vFrieze.size() > 0);

}

//--------------------------------------------------------------------------------------------------------------------------------------------------------

void SceneSplitOnLoad::onCollect(ITF_VECTOR<Scene*> &_asyncLoadedSceneDone)
{
    m_ProcessStatus = ProcessStatusCollecting;

    ITF_ASSERT(m_processedScenes.size() == 0);
    m_processedScenes = _asyncLoadedSceneDone;

    for (ITF_VECTOR<Scene*> ::iterator iterScene = m_processedScenes.begin() ;iterScene!=m_processedScenes.end();++iterScene)
    {
        Scene* pScene = *iterScene;

        World* pWorld = pScene->getWorld();
        pWorld->addScene(pScene);

        pScene->onSceneCollect(m_vActor,m_vFrieze);
    }

    m_ProcessStatus = ProcessStatusOff;


    //test
    /*
    for (ITF_VECTOR<Pickable*>::iterator iter = m_vActor.begin();iter!=m_vActor.end();iter++)
    {
        Actor* pActor = static_cast<Actor*> (*iter);
        const Path& path = pActor->getLua();
        TEMPLATEDATABASE->getTemplate<Actor_Template>(&m_templateClientHandler,path,btrue);
    }*/

    startProcessing();
    LOG("OnSceneCollect : Actor:%d Frieze:%d",m_vActor.size(),m_vFrieze.size());
}

//--------------------------------------------------------------------------------------------------------------------------------------------------------

}
