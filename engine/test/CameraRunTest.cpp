#include "precompiled_engine.h"

#ifndef _ITF_TESTMANAGER_H_
#include "engine/test/testmanager.h"
#endif //_ITF_TESTMANAGER_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_


namespace ITF
{

extern u32 resourcePhysicalLifeTime;

bbool CameraRunStateTest::m_isRunning = bfalse;
bbool CameraRunStateTest::m_loop;
bbool CameraRunStateTest::m_isAborted;
float CameraRunStateTest::m_camSpeedPercent = 0.5f;

CameraRunStateTest::CameraRunStateTest()
{   
    m_isRunning = bfalse;
    m_bActive = bfalse;
    m_Timer = 30.f; // 30 seconds 
    m_innerTimer = 0.f;
    m_startZ = m_curFrameX = m_curFrameY = m_curFrameZ = m_frameIncrX = m_frameIncrY = 0.f;
}

void CameraRunStateTest::onEnter()
{

}

void CameraRunStateTest::onExit()
{

}

void CameraRunStateTest::stopTest()
{
#ifndef ITF_FINAL
    m_isRunning = bfalse;
    TESTMANAGER->setTest(NULL);
#endif // ITF_FINAL
}

void CameraRunStateTest::startNewTest( bbool loop /* = true */, bbool waitAtEndOfWorld /* = true */ )
{
#ifndef ITF_FINAL
    CameraRunStateTest* test = new CameraRunStateTest();
    test->build();
    TESTMANAGER->setTest(test);
    m_isRunning = btrue;
    m_loop = loop;
    m_isAborted = bfalse;
#endif // ITF_FINAL
}

void CameraRunStateTest::build()
{
#ifndef ITF_FINAL
    CAMERA->setMode(Camera::Camera_Editor);
    m_aabb.setMin(CURRENTWORLD->getminCellPosEverRegistered(0));
    m_aabb.setMax(CURRENTWORLD->getmaxCellPosEverRegistered(0));
    m_curFrameX = m_aabb.getMin().m_x;
    m_curFrameY = m_aabb.getMin().m_y;
    m_x = m_aabb.getMin().m_x;
    m_y = m_aabb.getMin().m_y;
    m_curFrameZ = 0.f;
    m_frameIncrX = 1.f / m_aabb.getWidth();
    m_frameIncrY = 1.f / m_aabb.getHeight();
    m_bActive = btrue;
#endif // ITF_FINAL
}



void CameraRunStateTest::update(f32 _dt)
{
#ifndef ITF_FINAL
    if (!m_bActive)
    {
        m_Timer -= LOGICDT;
        if (m_Timer < 0.f)
        {
            World* pWorld = CURRENTWORLD;
            if(!pWorld)
                return;

            m_startZ = CAMERA->getZ();
            m_bActive = btrue;

            Scene* pScene = pWorld->getRootScene();
            bbool done = bfalse;
            if (pScene)
            {
                const PickableList& frises = pScene->getFrises();
                if (frises.size() > 10.f)
                {
                    CAMERA->setMode(Camera::Camera_Editor);
                    m_aabb.setMin(CAMERA->getPos().truncateTo2D());
                    m_aabb.setMax(CAMERA->getPos().truncateTo2D());
                    for (u32 i = 0; i < frises.size(); i++)
                    {
                        const Frise* pFrise = static_cast<Frise*>(frises[i]);
                        for (u32 i = 0; i<pFrise->getPosCount(); i++)
                        {
                            m_aabb.grow(pFrise->getPosAt(i));
                        }                        
                    }
                    m_curFrameX = 0.f;
                    m_curFrameY = 0.f;
                    m_curFrameZ = 0.f;
                    m_frameIncrX = 1.f / m_aabb.getWidth();
                    m_frameIncrY = 1.f / m_aabb.getHeight();
                    done = btrue;
                }
            }
            if (!done)
            {
                m_bActive = bfalse;
                m_Timer = 10.f; // retry in 10 seconds
            }
        }
    }
    if (m_bActive)
    {/*
        f32 cameraX = m_aabb.getCenter().m_x + sin(m_curFrameX * 0.25f) * m_aabb.getWidth() * 0.5f;
        f32 cameraY = m_aabb.getCenter().m_y + sin(m_curFrameY * 0.25f) * m_aabb.getHeight() * 0.5f;
        f32 cameraZ = (m_startZ + (1.f+cos(m_curFrameZ)) * m_startZ);
        CAMERA->forcePosition(Vec3d(cameraX, cameraY, cameraZ));

        m_curFrameX += m_frameIncrX * ((float)rand() / (float)RAND_MAX * 15.f);
        m_curFrameY += m_frameIncrY * ((float)rand() / (float)RAND_MAX * 15.f);
        m_curFrameZ += m_frameIncrX * ((float)rand() / (float)RAND_MAX * 15.f);*/

        if ( CAMERA->getMode() != Camera::Camera_Editor )
        {
            stopTest();
            m_isAborted = btrue;
            return;
        }

        if (m_innerTimer > 0.f)
        {
            String label;
            label.setTextFormat("Waiting: %f...", m_innerTimer);
            u32 uSeparatorColor     = ITFCOLOR_TO_U32(150, 50, 50, 50);
            GFX_ADAPTER->draw2dBox(Vec2d(GFX_ADAPTER->getScreenWidth()*0.5f-10.f, GFX_ADAPTER->getScreenHeight()*0.5f-10.f), 200.f, 40.f, uSeparatorColor, uSeparatorColor, uSeparatorColor);
            GFX_ADAPTER->drawDBGText(label, GFX_ADAPTER->getScreenWidth()*0.5f, GFX_ADAPTER->getScreenHeight()*0.5f, 1.f, 1.f, 1.f, btrue);
            m_innerTimer -= LOGICDT;
        }
        else
        {
            m_x += m_camSpeedPercent *(f32)LAYER_0_CELL_SIZE * 0.5f;//0.1f;
            if (m_x >= m_aabb.getMax().m_x)
            {
                m_x = m_aabb.getMin().m_x;
                m_y += (f32)LAYER_0_CELL_SIZE * 0.5f;
                m_innerTimer = resourcePhysicalLifeTime * LOGICDT;
                if (m_y >= m_aabb.getMax().m_y)
                {
                    if ( !m_loop )
                    {
                        stopTest();
                        return;
                    }
                    m_y = m_aabb.getMin().m_y;
                }
            }
        }
        CAMERA->forcePosition(Vec3d(m_x, m_y, CAMERA->getZ()));
    }
#endif // ITF_FINAL
}

}
