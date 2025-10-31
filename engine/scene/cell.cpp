#include "precompiled_engine.h"

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CELL_H_
#include "engine/scene/cell.h"
#endif //_ITF_CELL_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDserver.h"
#endif //_ITF_ID_SERVER_H_


#ifdef ITF_SUPPORT_EDITOR
    #ifndef _ITF_EDITOR_H_
    #include "editor/editor.h"
    #endif //_ITF_EDITOR_H_
#endif //ITF_SUPPORT_EDITOR

#ifndef _ITF_REPORTISSUEUTILS_H_
#include "engine/debug/reportIssueUtils.h"
#endif //_ITF_REPORTISSUEUTILS_H_


#ifndef _ITF_STATSMANAGER_H_
#include "engine/stats/statsManager.h"
#endif //_ITF_STATSMANAGER_H_

#ifndef _CELLLAYERSSETTINGS_H_
#include "engine/scene/CellsLayersSettings.h"
#endif // _CELLLAYERSSETTINGS_H_

#ifndef _ITF_CONFIG_H_
#include "core/Config.h"
#endif //_ITF_CONFIG_H_

namespace ITF
{
    Cell*   Cell::m_beingUpdatedCell = NULL;
    SafeArray<BaseObject*>   Cell::m_solvedPointers;

#define CELL_EPSILON 0.1f

    void Cell::init(const Vec2d& _pos, const Vec2d& _size,const i16 _indexX,const i16 _indexY, World* _world)                
    {
        m_bEnable = 0;
        m_world = _world;
        m_aabb.setMin(_pos);
        m_aabb.setMax(_pos+_size);
        m_FrameEnable = U32_INVALID;
        m_loadResourcesMask = _indexX & 7;
        m_ignoreLoadResourceMask = 0;
    };

    Cell::~Cell()
    {
    }
    
    void    Cell::enable(u32 _currentFrame)
    {
        m_beingUpdatedCell = this;
        m_solvedPointers.clear();

        if (m_FrameEnable == _currentFrame) //protect against multiple activation...
        {
            m_beingUpdatedCell = NULL;
            return;
        }
        STATS_CELLENABLEINC
        m_FrameEnable  = _currentFrame;

        AABB aabb;

        Camera* cam = CAMERA;

        const f32 camMinX = cam->m_minAABBAtAZZero.m_x;
        const f32 camMinY = cam->m_maxAABBAtAZZero.m_y;
        const f32 camMaxX = cam->m_maxAABBAtAZZero.m_x;
        const f32 camMaxY = cam->m_minAABBAtAZZero.m_y;

        const f32 deltaX = cam->getDeltaX();
        const f32 deltaY = cam->getDeltaY();

        const f32 minX = getAABB().getMin().m_x - CELL_EPSILON;
        const f32 minY = getAABB().getMin().m_y - CELL_EPSILON;
        const f32 maxX = getAABB().getMax().m_x + CELL_EPSILON;
        const f32 maxY = getAABB().getMax().m_y + CELL_EPSILON;

#if defined ITF_SUPPORT_EDITOR
        const bbool allowClipping = !CAMERA->isBiased();
#endif // ITF_SUPPORT_EDITOR
      
        ID_SERVER->getObjectListV2(m_vPickable, m_solvedPointers);
        for (u32 index = 0; index < m_solvedPointers.size(); index++)
        {
            ITF_ASSERT_MSG(m_vPickable.size() == m_solvedPointers.size(), "very bad assert");

            Pickable::CellEnableData*   data = NULL;
#ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            Pickable*  pObj = static_cast<Pickable*> (GETOBJECT(m_vPickable[index].m_obj));
#else
            Pickable*  pObj = (Pickable*)m_solvedPointers[index];
#endif

#ifdef TRACK_PICKABLE_FRIENDLY_NAME
            if (pObj->getUserFriendly().equals(TRACK_PICKABLE_FRIENDLY_NAME, btrue))
            {
                LOG("[Track pickable activation] Frame: %u cell::enable() with update type", CURRENTFRAME);
            }
#endif // TRACK_PICKABLE_FRIENDLY_NAME

            if (pObj)
            {
                ITF_ASSERT(pObj->isAllowedForCell());
                data = &pObj->getCellEnableData();
                if (!data->m_isStatic)
                {
                    if (_currentFrame == data->m_frame)
                        aabb = data->m_aabb;
                    else
                    {
                        aabb = pObj->getAABB();
                        aabb.projectTo2D(deltaX,deltaY,pObj->getDepth());
                        if (pObj->isLastCellUpdateDataValid())
                        aabb.grow(pObj->getLastCellUpdateData().m_aabb);
                        //aabb = pObj->getLastCellUpdateData();
                        data->m_frame = _currentFrame;
                        data->m_aabb = aabb;
                    }
                }
                else
                    aabb = data->m_aabb;

#if defined ITF_SUPPORT_EDITOR
                if(allowClipping)
#endif // ITF_SUPPORT_EDITOR
                {
                    if (Pickable::UpdateType_OnlyIfVisible == data->m_updateType)
                    {
                        if(
                        camMinX > (aabb.getMax().m_x) ||  // optimized version of aabb::checkOverlap feturing pre-backed epsilon
                        camMinY > (aabb.getMax().m_y) ||
                        camMaxX < (aabb.getMin().m_x) ||
                        camMaxY < (aabb.getMin().m_y))
                        {
                            if (!data->m_mustBeInitializedWithCell || pObj->isActive())
                            {
                                continue;
                            }
                        }
                    }
                }
            }
            else
            {
                m_vPickable.eraseNoOrder(index);
                m_solvedPointers.eraseNoOrder(index);
                index--;
                continue;
            }

            if ( (!data->m_isStatic) && (
                minX > (aabb.getMax().m_x) ||  // optimized version of aabb::checkOverlap feturing pre-backed epsilon
                minY > (aabb.getMax().m_y) ||
                maxX < (aabb.getMin().m_x) ||
                maxY < (aabb.getMin().m_y)) )
            {
                //if object doesnt exist anymore, or should not be in cell, remove it from cell
                // this test is not 100% secure because pObj can be not NULL but correspond to a recycled ObjectRef, that's why World::registerOrRemoveObjectInCells starts by unregistering the object from previous cells
                m_vPickable.eraseNoOrder(index);
                m_solvedPointers.eraseNoOrder(index);
                index--;
            }
            else
            {
#ifdef ITF_SUPPORT_EDITOR
                // Allows the editor to display picking shapes for objects that are not active
                if (EDITOR->authorizeDeactivatedObjects() && (!pObj->isEnabled() || pObj->hasDataError()))
                    EDITOR->addExtraSelectableObject(pObj->getRef());
#endif // ITF_SUPPORT_EDITOR

                pObj->setActiveForFrame(_currentFrame);
                
                ITF_ASSERT_MSG(!pObj->isActive() || (pObj->isPhysicalReady() || pObj->hasDataError()), "Starting to update an object that is not ready");
            }
        }

        m_bEnable = 1;

        m_beingUpdatedCell = NULL;
    }

    bbool Cell::removeObject(ObjectRef _ObjRef)
    {
#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if (Pickable* pickable = _ObjRef.getObject()->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
        {
            if (pickable->getUserFriendly().equals(TRACK_PICKABLE_FRIENDLY_NAME, btrue))
            {
                LOG("[Track pickable activation] Frame: %u Cell::removeObject", CURRENTFRAME);
            }
        }
#endif

        STATS_REMOVEDFROMCELLINC
        u32 size = m_vPickable.size();
        for (u32 index = 0; index < size; index++)
        {
            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            if (_ObjRef == m_vPickable[index].m_obj)
            #else
            if (_ObjRef == m_vPickable[index])
            #endif
            {
                m_vPickable.eraseNoOrder(index);
                if (this == m_beingUpdatedCell)
                {
                    ITF_ASSERT_MSG(m_solvedPointers[index] == _ObjRef.getObject(), "very bad assert");
                    m_solvedPointers.eraseNoOrder(index);
                    ITF_ASSERT_MSG(m_vPickable.size() == m_solvedPointers.size(), "very bad assert");
                }
                return btrue;
            }
        }
        return bfalse;
    }

    bbool Cell::hasObject(ObjectRef _ObjRef) const
    {
        i32 index = m_vPickable.find(_ObjRef);
        if (index >=0)
            return btrue;
        return bfalse;
    }

    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
        void    Cell::getObjects(SafeArray<CellObjData>& _list, u32 _flags) const
    #else
        void    Cell::getObjects(ObjectRefList& _list) const
    #endif
    {
        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            u32 size = m_vPickable.size();
            bbool isfirstLine = _flags & cellObjectFlag_FirstLine;
            bbool isfirstColumn = _flags & cellObjectFlag_FirstColumn;
            if (isfirstLine || isfirstColumn)
            {
                _list = m_vPickable;
                return;
            }

            for (u32 i = 0; i < size; i++)
            {
                if (m_vPickable[i].m_flags == Cell::cellObjectFlag_Corner)
                    _list.push_back(m_vPickable[i]);
            }
        #else // ENABLE_CELL_FLAGS_FAST_DISCARD
            _list = m_vPickable;
        #endif // ENABLE_CELL_FLAGS_FAST_DISCARD   
    }

    #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
        void    Cell::getObjects(ObjectRefList& _list, u32 _flags) const
        {
            u32 size = m_vPickable.size();
            bbool isfirstLine = _flags & cellObjectFlag_FirstLine;
            bbool isfirstColumn = _flags & cellObjectFlag_FirstColumn;
            if (isfirstLine || isfirstColumn)
            {
                for (u32 i=0; i<size; i++)
                    _list.push_back(m_vPickable[i].m_obj);
                return;
            }

            for (u32 i = 0; i < size; i++)
            {
                if (m_vPickable[i].m_flags == Cell::cellObjectFlag_Corner)
                    _list.push_back(m_vPickable[i].m_obj);
            }
        }
    #endif

    bbool Cell::checkObjectBelongsToCell (Pickable* _pObj) const
    {
        AABB aabb;
        const f32 deltaX = CAMERA->getDeltaX();
        const f32 deltaY = CAMERA->getDeltaY();

        f32 minX = getAABB().getMin().m_x - CELL_EPSILON;
        f32 minY = getAABB().getMin().m_y - CELL_EPSILON;
        f32 maxX = getAABB().getMax().m_x + CELL_EPSILON;
        f32 maxY = getAABB().getMax().m_y + CELL_EPSILON;

        aabb = _pObj->getAABB();
        aabb.projectTo2D(deltaX,deltaY, _pObj->getDepth());
        if (_pObj->isLastCellUpdateDataValid())
        aabb.grow(_pObj->getLastCellUpdateData().m_aabb);

        if ( minX > (aabb.getMax().m_x) || 
             minY > (aabb.getMax().m_y) ||
             maxX < (aabb.getMin().m_x) ||
             maxY < (aabb.getMin().m_y) )
            {
                return bfalse;
            }
        return btrue;
    }

    bbool Cell::checkForPhysicalReady()
    { 
        #ifdef USE_LOAD_RESOURCE_MAX_RANGE
            Vec2d camPos(CAMERA->getX(), CAMERA->getY());
        #endif //USE_LOAD_RESOURCE_MAX_RANGE

        for (i32 index = 0;index<(i32)m_vPickable.size();++index)   // keep index as i32 because is can de decreased within the loop, also don't precache .size() for the same reason
        {
            #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
                Pickable*  pObj =static_cast<Pickable*> (GETOBJECT(m_vPickable[index].m_obj));
            #else
                Pickable*  pObj =static_cast<Pickable*> (GETOBJECT(m_vPickable[index]));
            #endif
            ITF_ASSERT(pObj);
            if (pObj &&  !pObj->isPhysicalReady())
            {
                #ifdef USE_LOAD_RESOURCE_MAX_RANGE
                    if (!pObj->isLastCellUpdateDataValid())
                        continue;
                    {
                        f32 sqCamDist = (f32)CellsLayersSettings::getLoadResourceMaxRange(pObj->getLastCellUpdateData().m_layerIndex);
                        if (sqCamDist > MTH_EPSILON)
                        {
                            AABB aabb = pObj->getLastCellUpdateData().m_aabb;
                            sqCamDist *= sqCamDist;
                            if ((camPos-aabb.getCenter()).sqrnorm() > sqCamDist)
                                continue;
                        }
                    }
                #endif //USE_LOAD_RESOURCE_MAX_RANGE

				if (!pObj->updatePhysicalReady())
                {
                    #if defined(ITF_WINDOWS) && !defined(ITF_FINAL) && defined(ASSERT_ENABLED) && defined(DEVELOPER_FRED)
                        String8 msg("Cell is waiting for not loaded object:\n");
                        msg += pObj->getUserFriendly();
                        ITF_ASSERT_MSG(pObj->isRequestedLoading(), msg.cStr());
                    #endif
                    if (!pObj->isRequestedLoading())    // Happens sometimes (enables great optimizations in World.cpp)
                    {
                        if (!checkObjectBelongsToCell(pObj))
                        {
                            m_vPickable.eraseNoOrder(index);
                            if (this == m_beingUpdatedCell)
                            {
                                ITF_ASSERT_MSG(m_solvedPointers[index] == pObj, "very basd assert");
                                m_solvedPointers.eraseNoOrder(index);
                                ITF_ASSERT_MSG(m_vPickable.size() == m_solvedPointers.size(), "very bad assert");
                            }
                            index--;
                            #if defined(ITF_WINDOWS) && !defined(ITF_FINAL) && defined(ASSERT_ENABLED) && defined(DEVELOPER_FRED)
                                String8 msg("object belongs to wrong cell:\n");
                                msg += pObj->getUserFriendly();
                                ITF_ASSERT_MSG(bfalse, msg.cStr());
                            #endif
                            continue;
                        }
                        pObj->loadResources();
                    }
                    #ifndef ITF_FINAL
                        if (reportIssueUtils::investigateMapLoadingStuck(pObj))
                            continue;
                    #endif
					return bfalse;
                }
            }
        }
        return btrue;
    }

    void    Cell::disable()
    {
        m_bEnable = 0;
    }

#ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
    void Cell::addObject(ObjectRef _ObjRef, u32 _iLayer, u32 _flags, bbool _checkDoublons)
#else
    void Cell::addObject(ObjectRef _ObjRef, u32 _iLayer, bbool _checkDoublons)
#endif
    {
#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if (Pickable* pickable = _ObjRef.getObject()->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
        {
            if (pickable->getUserFriendly().equals(TRACK_PICKABLE_FRIENDLY_NAME, btrue))
            {
                LOG("[Track pickable activation] Frame: %u cell::addObject()", CURRENTFRAME);
            }
        }
#endif

        #ifdef ENABLE_CELL_FLAGS_FAST_DISCARD
            u32 size = m_vPickable.size();
            for (u32 index = 0; index < size; index++)
            {
                if (_ObjRef == m_vPickable[index].m_obj)
                {
    			    m_vPickable[index].m_flags = _flags;
                    return;
                }
            }
            CellObjData data;
            data.m_obj = _ObjRef;
            data.m_flags = _flags;
            m_vPickable.push_back(data);
            m_world->cellHasObjects(this, _iLayer);
        #else // ENABLE_CELL_FLAGS_FAST_DISCARD
            i32 index = -1;
            if (_checkDoublons)
                index = m_vPickable.findWithPrefetch(_ObjRef);

            if (index == -1 )
            {
                ITF_ASSERT_MSG(_checkDoublons || m_vPickable.findWithPrefetch(_ObjRef) == -1, "Major issue: Doublons in cell");
                m_vPickable.push_back(_ObjRef);
                if (this == m_beingUpdatedCell)
                {
                    m_solvedPointers.push_back(_ObjRef.getObject());
                    ITF_ASSERT_MSG(m_vPickable.size() == m_solvedPointers.size(), "very bad assert");
                }

                m_world->cellHasObjects(this, _iLayer);
            }
        #endif // ENABLE_CELL_FLAGS_FAST_DISCARD

#ifdef ASSERT_ENABLED
        if (Pickable* pickable = _ObjRef.getObject()->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)))
        {
            ITF_ASSERT(pickable->isAllowedForCell());
        }
#endif // ASSERT_ENABLED



    }





} //namespace ITF
