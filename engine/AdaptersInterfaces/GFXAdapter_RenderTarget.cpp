#include "precompiled_engine.h"

#ifndef _ITF_SINGLETONS_H_
#include "engine/singleton/Singletons.h"
#endif //_ITF_SINGLETONS_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_GFXADAPTER_RENDERTARGET_H_
#include "engine/AdaptersInterfaces/GFXAdapter_RenderTarget.h"
#endif //_ITF_GFXADAPTER_RENDERTARGET_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifdef ITF_IPAD
#include <algorithm>
#endif

namespace ITF
{
//----------------------------------------------------------------------------//
// Access functions from GFX_ADAPTER to manage render target.
//----------------------------------------------------------------------------//

renderTarget* GFXAdapter::getNewRenderTarget()
{
    if (!m_RenderTargetManager)
        m_RenderTargetManager = newAlloc(mId_GfxAdapter, RenderTargetManager());

    renderTarget* target = newAlloc(mId_GfxAdapter, renderTarget());
    target->init();

    getTargetManager()->addRenderTarget(target);
    return target;
}

void GFXAdapter::deleteRenderTarget(renderTarget* _target)
{
   if (!_target) return;

    _target->destroy();
    if(getTargetManager()) 
        getTargetManager()->removeRenderTarget(_target);
}

//----------------------------------------------------------------------------//
// Render Target.
//----------------------------------------------------------------------------//

renderTarget::renderTarget()
{
    m_surface = 0;
    m_prevColourSurface = 0;
    m_savedSurfaceDesc = 0; 
    m_savedSurfaceDescValid = 0;
    m_BackGroundColor = 0x0;
    m_area[0] = 0;
    m_area[1] = 0;
    m_area[2] = 0;
    m_area[3] = 0;
    m_Texture = NULL;
}

renderTarget::~renderTarget()
{
}

void renderTarget::init()
{
    m_Texture = (Texture*)RESOURCE_MANAGER->newEmptyResource(Resource::ResourceType_Texture);
    RESOURCE_MANAGER->addUserToResourcePhysicalData(m_Texture);
}

void renderTarget::set()
{
    GFX_ADAPTER->enableRenderTarget(this);
}

void renderTarget::clear(u32 _color)
{
    Color c(_color);
    GFX_ADAPTER->clear(GFX_CLEAR_COLOR, c.getRed(), c.getGreen(), c.getBlue(), c.getAlpha()); 
}

void renderTarget::unSet()
{
    GFX_ADAPTER->disableRenderTarget(this);
}

void renderTarget::clean()
{
    GFX_ADAPTER->cleanupRenderTarget(this);
}

void renderTarget::create(f32 _w, f32 _h)
{
    m_area[0] = 0;
    m_area[1] = 0;
    m_area[2] = (int)_w;
    m_area[3] = (int)_h;
    GFX_ADAPTER->initialiseRenderTarget(this);
}

void renderTarget::recreate()
{
    GFX_ADAPTER->initialiseRenderTarget(this);
}

void renderTarget::destroy()
{
    clean();

    if(m_Texture)
    {
        RESOURCE_MANAGER->removeUserToResourceData(m_Texture);
        m_Texture = NULL;
    }    
}

//----------------------------------------------------------------------------//
// Render Target Manager.
//----------------------------------------------------------------------------//

RenderTargetManager::RenderTargetManager()
{
    m_numberTarget = 0;
}

RenderTargetManager::~RenderTargetManager()
{
    destroyAllRenderTarget();
}

void RenderTargetManager::destroyAllRenderTarget()
{
    for (u32 i = 0; i < m_targetList.size(); i++)
    {
        m_targetList[i]->destroy();
        delete m_targetList[i];
    }
}

void RenderTargetManager::cleanAllRenderTarget()
{
    if (!m_numberTarget) return;

    ITF_VECTOR<renderTarget*>::iterator i = m_targetList.begin();
    for (; i != m_targetList.end(); ++i)
        (*i)->clean();
}

void RenderTargetManager::recreateAllRenderTarget()
{
    if (!m_numberTarget) return;

    ITF_VECTOR<renderTarget*>::iterator i = m_targetList.begin();
    for (; i != m_targetList.end(); ++i)
        (*i)->recreate();
}

void RenderTargetManager::addRenderTarget(renderTarget* _target)
{
    m_targetList.push_back(_target);
    m_numberTarget++;
}

void RenderTargetManager::removeRenderTarget(renderTarget* _target)
{
    ITF_VECTOR<renderTarget*>::iterator i= std::find(m_targetList.begin(),
        m_targetList.end(),
        _target);  

    if (m_targetList.end() != i)
    {
        m_targetList.erase(i);
        m_numberTarget--;
    }

    SF_DEL(_target);
}

} //namespace ITF