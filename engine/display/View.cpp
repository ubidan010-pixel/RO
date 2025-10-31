#include "precompiled_engine.h"

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

namespace ITF
{


SafeArray<View*>    View::m_views;
View*               View::m_mainView = NULL;

void    View::createCamera()
{
    deleteCamera();
    m_camera = newAlloc(mId_Singleton, Camera);
    m_type = viewType_Camera;
}

void    View::deleteCamera()
{
    if (m_camera)
    {
        delete m_camera;
        m_camera = NULL;
    }
}

void View::setTargetAABB( const AABB& _aabb )
{
    m_aabbAtZero = _aabb;
    m_lastScreenProj = _aabb; // to have a default value before it gets treated by the world
    m_type = viewType_AABB;
}

void    View::setMainView()
{
    m_mainView = this;
    setActive(btrue);
}


bbool    View::isMainView()
{
    return (this == m_mainView);
}


void    View::deleteView(View* _view)
{
    delete _view;
    i32 index = m_views.find(_view);
    ITF_ASSERT(index >= 0);
    if (index < 0)
        return;
    m_views.eraseNoOrder(index);
    if (_view == m_mainView)
    {
        if (m_views.size())
            m_mainView = m_views[0];
        else
            m_mainView = NULL;
    }
}

void View::setActive(bbool _active)
{
    m_isActive = _active;
}


View*    View::createView(const char* _name, bbool _canBeMainView, bbool _prefetch)
{
    View* ret = newAlloc(mId_System, View());
    ret->m_name = _name;
    ret->m_prefetchDataOnly = _prefetch;
    m_views.push_back(ret);
    if (_canBeMainView && NULL == m_mainView)
        m_mainView = ret;
    return ret;
}

void    View::deleteAll()
{
    for(;;)
    {
        if (0 == m_views.size())
            break;
        deleteView(m_views[0]);
    }
}


View::View()
{
    m_camera = NULL;
    m_or = ITF_INVALID_OBJREF;
    m_isActive = bfalse;
    m_type = viewType_Count;
    m_lockAABB = bfalse;
}

View::~View()
{
    ITF_ASSERT(-1 != m_views.find(this));
    deleteCamera();
}


void View::lockAll()
{
    for (u32 i = 0; i < m_views.size(); i++)
    {
        m_views[i]->lockAABB(btrue);
    }
}

void View::unLockAll()
{
    for (u32 i = 0; i < m_views.size(); i++)
    {
        m_views[i]->lockAABB(bfalse);
    }
}

void View::switchLockAll()
{
    for (u32 i = 0; i < m_views.size(); i++)
    {
        m_views[i]->lockAABB(!m_views[i]->isLocked());
    }
}

} // namespace ITF