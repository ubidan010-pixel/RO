
#include "precompiled_engine.h"

#ifndef _ITF_PICKABLE_H_
#include "engine/picking/Pickable.h"
#endif // _ITF_PICKABLE_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif // _ITF_SCENE_H_

#ifndef ITF_FINAL
    #ifndef _ITF_CONFIG_H_
        #include "core/Config.h"
    #endif // _ITF_CONFIG_H_
#endif // ITF_FINAL

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif // _ITF_CAMERA_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#ifndef SERIALIZEROBJECTPARSER_H
#include "core/serializer/ZSerializerObjectParser.h"
#endif // SERIALIZEROBJECTPARSER_H

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

//VERY UGLY ;but no choice because someone put some edition thingie in the pickable....
#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_DEPTH_RANGE_H_
#include "engine/scene/DepthRange.h"
#endif //_ITF_DEPTH_RANGE_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_
#ifndef _ITF_PLUGINGATEWAY_H_
#include "PluginGateway/PluginGateway.h"
#endif //_ITF_PLUGINGATEWAY_H_

#ifndef _ITF_OBJECTGROUP_H_
#include "engine/scene/ObjectGroup.h"
#endif //_ITF_OBJECTGROUP_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#include <float.h>

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Pickable)

    BEGIN_SERIALIZATION_ABSTRACT_CHILD(Pickable)

    SERIALIZE_FUNCTION(preChangeProperties,ESerialize_PropertyEdit);

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("POS2D",m_initialPos);
        SERIALIZE_MEMBER("RELATIVEZ",m_initialZ);
        SERIALIZE_MEMBER("ANGLE",m_initialRot);
        SERIALIZE_MEMBER("SCALE",m_initialScale);
        SERIALIZE_MEMBER("USERFRIENDLY",m_userFriendly);
        SERIALIZE_MEMBER("OFFSCREEN_UPDATE_OVERRIDE",m_updateTypeFromTemplateOverridden);

        SERIALIZE_ENUM_BEGIN("UPDATE_TYPE",m_updateType);
            SERIALIZE_ENUM_VAR(UpdateType_OnlyIfVisible);
            SERIALIZE_ENUM_VAR(UpdateType_OffscreenAllowed);
        SERIALIZE_ENUM_END();
    END_CONDITION_BLOCK()

    BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
        SERIALIZE_OBJECT("AABB_RELATIVE", m_relativeAABB);
    END_CONDITION_BLOCK()
    
    BEGIN_CONDITION_BLOCK(ESerialize_Editor)
        SERIALIZE_MEMBER("CURPOS", m_pos);
        SERIALIZE_MEMBER("CURANGLE", m_angle);
        SERIALIZE_MEMBER("CURSCALE", m_scale);
    END_CONDITION_BLOCK()

    BEGIN_CONDITION_BLOCK(ESerializeGroup_Checkpoint)
        SERIALIZE_MEMBER("ISALLOWEDFORCELL",m_isAllowedForCell);
        SERIALIZE_MEMBER("isEnabled",m_isEnabled);
    END_CONDITION_BLOCK()

    SERIALIZE_FUNCTION(updateAbsoluteAABBFromRelative, ESerialize_Data_Load); // Compute abs aabb from relative after serialization load
    SERIALIZE_FUNCTION(postChangeProperties,ESerialize_PropertyEdit_Load);

    END_SERIALIZATION()



    ///////////////////////////////////////////////////////////////////////////////////////////
    SafeArray<PickablePropertyEditState> Pickable::m_PropertyEditStates;
    SafeArray<ObjectRef> Pickable::m_modifiedList;
        
    Pickable::Pickable()
    : m_angle(0.0f)
    , m_lastActiveFrame(U32_INVALID)
    , m_bRequestedLoading(bfalse)
    , m_isSelected(bfalse)
    , m_isAllowedForCell(btrue)
    , m_initialPos(Vec2d::Zero)
    , m_pos(Vec3d::Zero)
    , m_initialZ(0.f)
    , m_initialRot(0.0f)
    , m_initialScale(Vec2d::One)
    , m_scale(Vec2d::One)
    , m_lastVisibleFrame(0)
    , m_isAlwaysVisible(bfalse)
    , m_isEnabled(btrue)
    , m_activeInSceneWhen2D(btrue)
    , m_active(bfalse)
    , m_physicalReady(btrue)
    , m_deletionRequested(bfalse)
    , m_isSerializable(btrue)
    , m_modified(bfalse)
    , m_bHasDataError(bfalse)                                       
    , m_objectGroupIndex(ObjectGroup::GroupIndex_Default)
    , m_taggedForDeletion(bfalse)
    , m_dontUnloadResourceAtFrame(0xffffffff)
    , m_updateTypeFromTemplateOverridden(bfalse)
    , m_updateType(UpdateType_OnlyIfVisible)
    , m_isComputingNextAABB(bfalse)
    , m_relativeAABB(Vec2d(0,0))
    , m_nextRelativeAABB(Vec2d(0,0))
#ifdef  ITF_SUPPORT_EDITOR
    , m_pickingInfo2D(NULL)
    , m_ObjectFamily(ObjectFamily_LevelDesign)
    , m_isHiddenForEditor(bfalse)
#endif //ITF_SUPPORT_EDITOR
    , m_updateTypeSetByPropagation(0)
    {
        m_lastCellEnableData.m_frame = 0xffffffff;
        resetCellUpdateInfo();
        resetCurrentInfos();
    }

    Pickable::~Pickable()
    {
#ifdef CHECK_WORLDMANAGER_LIST
        if (WORLD_MANAGER)
            WORLD_MANAGER->checkPickableCanBeDeleted(this,0);
#endif //CHECK_WORLDMANAGER_LIST
        ITF_ASSERT_CRASH(!m_sceneRef.isValid(), "can't cow-boy delete an object owned by a scene");
    }


    Vec3d Pickable::getLocalPos() const
    {
        Vec3d pos = getPos();

        if ( Scene* scene = getScene() )
        {
            if ( SubSceneActor* subActor = scene->getSubSceneActorHolder() )
            {
                pos -= subActor->getPos();
                Vec3d_Rotate(&pos, -subActor->getAngle());
                if(subActor->isFlipped())
                    pos.m_x = -pos.m_x;
                pos.m_x /= subActor->getScale().m_x;
                pos.m_y /= subActor->getScale().m_y;
            }
        }

        return pos;
    }

    void Pickable::setTaggedForDeletion()
    {
        m_taggedForDeletion = btrue;
        disable();
    }


    void Pickable::setLocalPos( Vec3d _pos )
    {
        if ( Scene* scene = getScene() )
        {
            if ( SubSceneActor* subActor = scene->getSubSceneActorHolder() )
            {
                _pos.m_x *= subActor->getScale().m_x;
                _pos.m_y *= subActor->getScale().m_y;
                if(subActor->isFlipped())
                    _pos.m_x = -_pos.m_x;
                Vec3d_Rotate(&_pos, subActor->getAngle());
                _pos += subActor->getPos();
            }
        }

        setPos(_pos);
    }

    void Pickable::setLocal2DPos( Vec2d _pos )
    {
        if ( Scene* scene = getScene() )
        {
            if ( SubSceneActor* subActor = scene->getSubSceneActorHolder() )
            {
                _pos = _pos.Rotate(subActor->getAngle());
                _pos.m_x *= subActor->getScale().m_x;
                _pos.m_y *= subActor->getScale().m_y;
                if(subActor->isFlipped())
                    _pos.m_x = -_pos.m_x;
                _pos += subActor->get2DPos();
            }
        }

        set2DPos(_pos);
    }

    f32 Pickable::getLocalAngle() const
    {
        f32 angle = m_angle;

        if ( Scene* scene = getScene() )
        {
            if ( SubSceneActor* subActor = scene->getSubSceneActorHolder() )
            {
                angle -= subActor->getAngle();
            }
        }

        return angle;
    }

    void Pickable::setLocalAngle( f32 _angle )
    {
        if ( Scene* scene = getScene() )
        {
            if ( SubSceneActor* subActor = scene->getSubSceneActorHolder() )
            {
                _angle += subActor->getAngle();
            }
        }

        setAngle(_angle);
    }


    void Pickable::setWorldInitialPos( const Vec3d& _pos )
    {
        Scene* myScene = getScene();
        Vec2d myPos = _pos.truncateTo2D();
        f32 myZ = _pos.m_z;

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                Vec3d subScenePos = subActor->getWorldInitialPos();
                f32 subSceneRot = subActor->getWorldInitialRot();

                myPos = (myPos - subScenePos.truncateTo2D()).Rotate(-subSceneRot);

                if(subActor->isFlipped())
                    myPos.m_x = -myPos.m_x;

                myPos.m_x /= subActor->getScale().m_x;
                myPos.m_y /= subActor->getScale().m_y;

                myZ -= subScenePos.m_z;
            }
        }

        setLocalInitialPos(myPos.to3d(myZ));
    }

    void Pickable::setLocalInitialPos( const Vec3d& _pos )
    {
        m_initialPos = _pos.truncateTo2D();
        m_initialZ = _pos.m_z;
    }

    Vec3d Pickable::getWorldInitialPos() const
    {
        Scene* myScene = getScene();
        Vec2d myPos = getLocalInitialPos().truncateTo2D();
        f32 myZ = getLocalInitialZ();

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                Vec3d subScenePos = subActor->getWorldInitialPos();
                f32 subSceneRot = subActor->getWorldInitialRot();

                if(subActor->isFlipped())
                    myPos.m_x = -myPos.m_x;

                myPos.m_x *= subActor->getScale().m_x;
                myPos.m_y *= subActor->getScale().m_y;

                myPos = myPos.Rotate(subSceneRot) + subScenePos.truncateTo2D();
                myZ += subScenePos.m_z;
            }
        }

        return myPos.to3d(myZ);
    }

    void Pickable::setWorldInitialZ( f32 _z )
    {
        Scene* myScene = getScene();
        f32 myZ = _z;

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                myZ -= subActor->getWorldInitialZ();
            }
        }

        setLocalInitialZ(myZ);
    }

    f32 Pickable::getWorldInitialZ() const
    {
        Scene* myScene = getScene();
        f32 myZ = getLocalInitialZ();

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                myZ += subActor->getWorldInitialZ();
            }
        }

        return myZ;
    }

    void Pickable::setWorldInitialRot( f32 _rot )
    {
        Scene* myScene = getScene();
        f32 myRot = _rot;

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                if(subActor->isFlipped())
                    myRot = -myRot;

                myRot -= subActor->getWorldInitialRot();
            }
        }

        setLocalInitialRot(myRot);
    }

    void Pickable::setLocalInitialRot( f32 _rot )
    {
        m_initialRot = _rot;
    }

    f32 Pickable::getWorldInitialRot() const
    {
        Scene* myScene = getScene();
        f32 myRot = getLocalInitialRot();

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                if(subActor->isFlipped())
                    myRot = -myRot;

                myRot += subActor->getWorldInitialRot();
            }
        }

        return myRot;
    }

    void Pickable::setWorldInitialScale( const Vec2d& _scale )
    {
        Scene* myScene = getScene();
        Vec2d myScale = _scale;

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                myScale /= subActor->getWorldInitialScale();
            }
        }

        setLocalInitialScale(myScale);
    }

    void Pickable::setLocalInitialScale( const Vec2d& _scale )
    {
        m_initialScale = _scale;
    }

    Vec2d Pickable::getWorldInitialScale() const
    {
        Scene* myScene = getScene();
        Vec2d myScale = getLocalInitialScale();

        if ( myScene )
        {
            SubSceneActor* subActor = myScene->getSubSceneActorHolder();

            if ( subActor )
            {
                myScale *= subActor->getWorldInitialScale();
            }
        }

        return myScale;
    }
        
    void Pickable::computePlacementMatrix      (   Matrix44 &_result, bbool _ignoreScale   ) const
    {
        ITF_ASSERT(_ignoreScale);
        M44_setMatrixRotationZ(&_result, getAngle());
        //We do not support scaling yet. This is because of norm issues in polylines
        const Vec3d &pos = getPos();
        _result.setLineAt(3, pos.m_x, pos.m_y, pos.m_z, 1.f);
    }

    void Pickable::offsetPosition(const Vec3d& _delta, bbool _applyToInitialValue)
    {
        if(_applyToInitialValue)
        {
            setWorldInitialPos(getWorldInitialPos() + _delta);
        }
        else
        {
            setPos(getPos() + _delta);
        }
    }
    
    void Pickable::rotate(f32 _deltaAngle, bbool _applyToInitialValue, const Vec3d* _pOrigin/* = NULL*/)
    {
        // If rotating around position "_pOrigin"
        if(_pOrigin)
        {
            Vec3d pos3D;
            if(_applyToInitialValue)
                pos3D = getWorldInitialPos();
            else
                pos3D = getPos();

            Vec3d axeToRotate = pos3D - *_pOrigin;

            Matrix44 m;
            M44_setMatrixRotationZ(&m, _deltaAngle);
            
            Vec3d newAxe;
            M44_matrixVec3dTransform(&newAxe, &m, &axeToRotate);

            const Vec3d newPos = *_pOrigin + newAxe;
            if(_applyToInitialValue)
                setWorldInitialPos(newPos);
            else
                setPos(newPos);
            
        }

        if(_applyToInitialValue)
            setWorldInitialRot(getWorldInitialRot() + _deltaAngle);
        else
            setAngle(getAngle() + _deltaAngle);
    }

    void Pickable::scale( const Vec2d& _deltaScale, bbool _applyToInitialValue)
    {
        Vec2d worldScale = getScale() + _deltaScale;

        if(_applyToInitialValue)
        {
            // Respect the forced scale
            setWorldInitialScale(worldScale);
            setScale(getWorldInitialScale());
        }
        else
        {
            setScale(worldScale);
        }
    }

    void Pickable::initializePosAngleScaleFromInitialValues()
    {
        m_pos = getWorldInitialPos();
        m_angle = getWorldInitialRot();
        m_scale = getWorldInitialScale();
    }

    bbool  Pickable::onLoadedFromScene(HotReloadType _hotReload)
    {
        onLoaded(_hotReload);
        return bfalse;
    }

    void Pickable::onLoaded(HotReloadType /*_hotReload*/)
    {
        ITF_ASSERT_CRASH(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "This function must be called from main thread");

#ifdef ASSERT_ENABLED
        World* pWorld = getWorld();
        ITF_ASSERT_CRASH(pWorld, "At this point the pickable must have a scene that belongs to a world, maybe Word::addScene(getScene()) is missing before calling Scene::onSceneLoaded()");
#endif // ASSERT_ENABLED

        initializePosAngleScaleFromInitialValues();

        getCellEnableData().m_updateType = getUpdateType();
    }

    void Pickable::setScale( const Vec2d& _scale )
    {
        if ( m_scale != _scale )
        {
            setModified();
            m_scale = _scale;

            EventScaleChanged scaleChanged;
            onEvent(&scaleChanged);
        }
    }

    void Pickable::setSelected (   PickingInfo2D* _info2d) 
    {
#ifdef  ITF_SUPPORT_EDITOR
        m_pickingInfo2D = _info2d; 
#endif //ITF_SUPPORT_EDITOR
        if (_info2d) 
            m_isSelected = btrue;
        else
            m_isSelected = bfalse; 
    }
    
    void Pickable::hotReloadFromMem( ArchiveMemory* _archive )
    {
        const bbool resloaded = isRequestedLoading();
        const bbool wasActive = isActive();
        const bbool wasActiveInScene = isEnabled();
        const bbool wasAllowForCell = isAllowedForCell();

        resetDataError();

        if ( resloaded )
            unloadResources();
        
        if ( wasActiveInScene )
            onBecomeInactive();

        // Save the instance data
        _archive->rewindForReading();

        CSerializerObjectBinary serializeBin(_archive);

        Serialize(&serializeBin,ESerialize_Data_Load);

        finalizeReload(resloaded,wasActive,wasActiveInScene,wasAllowForCell);
    }

    void Pickable::finalizeReload( bbool _wasResLoaded, bbool _wasActive, bbool _wasActiveInScene, bbool _wasAllowForCell )
    {
        RESOURCE_MANAGER->flushPendingOps();

        onLoaded();

        if ( _wasResLoaded )
        {
            loadResources();
        }

        RESOURCE_MANAGER->flushPendingOps();

        updatePhysicalReady();

        if ( _wasActiveInScene )
        {
            enable();
        }
        else
        {
            disable();
        }

        setAllowedCell(_wasAllowForCell);

        if ( _wasActive )
        {
            onBecomeActive();
        }
    }


    void Pickable::unregisterFromWorldCells()
    {
        if (isCellObjectType())
        {
            World * pWorld = getWorld();
            if (pWorld)
                pWorld->registerOrRemoveObjectInCells(this, World::RemoveFromCells);
        }
    }

    void Pickable::registerInWorldCells()
    {
        World * pWorld = getWorld();
        if (pWorld && !pWorld->getIgnoreCells())
            pWorld->registerOrRemoveObjectInCells(this, World::AddInCells);
    }

    void  Pickable::setActiveForFrame(u32 _frame)
    {
        m_dontUnloadResourceAtFrame = _frame;

#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if (getUserFriendly().equals(TRACK_PICKABLE_FRIENDLY_NAME, btrue))
        {
            LOG("[Track pickable activation] Frame: %u  Pickable::setActiveForFrame "
                "(hasDataError: %s) "
                "(isPhysicalReady: %s) "
                "(isEnabled: %s) "
                "(getLastActiveFrame: %d) ",
                CURRENTFRAME,
                hasDataError() ? "true" : "false",
                isPhysicalReady() ? "true" : "false",
                isEnabled() ? "true" : "false",
                getLastActiveFrame()
                );
        }
#endif
        
        if (!hasDataError()) 
        {
            if (!isPhysicalReady())
            {
                updatePhysicalReady();
                if (!isPhysicalReady())
                {
#ifndef ITF_FINAL //Debug to catch poping
                    if (CONFIG->m_debugPopup)
                        GFX_ADAPTER->drawDBGAABB(getAABB(), 1.0f, 0.0f, 0.0f, 0.0f, 10.0f, getDepth());
#endif //ITF_FINAL

                    if (getObjectType() == BaseObject::eFrise)
		                loadResources();
                    return;
                }
            }
        }

        if (isEnabled() || hasDataError())
        {
            if (_frame != getLastActiveFrame())
            {
                setLastActiveFrame(_frame);
                Scene* pScene = getScene();
                if (pScene)
                    pScene->addActiveObject(getRef());
                if (!isActive())
                    onBecomeActive();
            }
    #ifdef _DEBUG
            else
            {
                ITF_ASSERT_MSG(getScene()->getActiveObjects().hasObject(getRef()), "The pickable is set active multiple time for this frame, but it could not be found in the current active list...");
            }
    #endif //_DEBUG
        }
    }

    i32 Pickable::getPropertyEditStateIndex(const ObjectRef _ref)
    {
        for (u32 i = 0; i < m_PropertyEditStates.size(); i++)
        {
            if (_ref == m_PropertyEditStates[i].m_object)
                return i;
        }
        return -1;
    }

    void Pickable::preChangeProperties()
    {
        PickablePropertyEditState* propertyEditState  = NULL;
        i32 propertyEditStateIndex = getPropertyEditStateIndex(getRef());
        if ( propertyEditStateIndex < 0)
            propertyEditState = &m_PropertyEditStates.incrSize();
        else
            propertyEditState = &m_PropertyEditStates[propertyEditStateIndex];

        propertyEditState->m_object = getRef();
        propertyEditState->m_previousInitPos = getLocalInitialPos();
        propertyEditState->m_previousPos = getPos();
        propertyEditState->m_previousInitAngle = getLocalInitialRot();
        propertyEditState->m_previousAngle = getAngle();
        propertyEditState->m_previousInitialScale = getLocalInitialScale();
        propertyEditState->m_previousScale = getScale();
    }

    void Pickable::postChangeProperties()
    {
        i32 propertyEditStateIndex = getPropertyEditStateIndex(getRef());
        ITF_ASSERT(propertyEditStateIndex >= 0);
        PickablePropertyEditState& propertyEditState  = m_PropertyEditStates[propertyEditStateIndex];

        // clamp RelativeZ
        if(m_initialZ > 5000)
        {
            m_initialZ = 5000;
        }
        else if(m_initialZ < -5000)
        {
            m_initialZ = -5000;
        }

        if((propertyEditState.m_previousInitPos - getLocalInitialPos()).sqrnorm() > MTH_EPSILON)
        {
            setPos(getWorldInitialPos());
        }
        
        if((propertyEditState.m_previousInitialScale - getLocalInitialScale()).sqrnorm() > MTH_EPSILON)
        {
            setScale(getLocalInitialScale());
        }

        if(f32_Abs(propertyEditState.m_previousInitAngle - getLocalInitialRot()) > MTH_EPSILON)
        {
            const f32 newAngle = getLocalInitialRot();

            // apply delta rotation
            rotate(newAngle - propertyEditState.m_previousInitAngle, btrue, NULL);

            // overwrite
            setAngle(newAngle);
            setLocalInitialRot(newAngle);
        }

        getCellEnableData().m_updateType = getUpdateType();

        m_PropertyEditStates.eraseNoOrder(propertyEditStateIndex);
    }

    bbool Pickable::isOnScreen()
    {
        //return CAMERA->isRectVisible(m_AABB, m_pos.m_z);
        return getLastVisibleFrame() == (CURRENTFRAME - 1);
    }

    Scene* Pickable::getScene() const
    {
        Scene* pScene = static_cast<Scene*>(m_sceneRef.getObject());        
        return pScene;
    }

    World* Pickable::getWorld() const 
    {
        const Scene* pScene = getScene();
        return pScene ? pScene->getWorld() : NULL;
    }


    bbool Pickable::updateCellsNeeded(const AABB& _aabb, u32 _layerIndex )
    {
        const float tolerance = 0.01f;

        if (!m_lastCellUpdateData.m_aabb.isValid())
            return btrue;

        if (!_aabb.isValid())
            return btrue;

        if ((m_lastCellUpdateData.m_aabb.getMin() - _aabb.getMin()).sqrnorm() > tolerance)
            return btrue;

        if ((m_lastCellUpdateData.m_aabb.getMax() - _aabb.getMax()).sqrnorm() > tolerance)
            return btrue;

        if ( m_lastCellUpdateData.m_layerIndex != _layerIndex )
            return btrue;

        return bfalse;
    }

    void Pickable::resetCurrentInfos() 
    {
        setPos(getWorldInitialPos());
        setAngle(getWorldInitialRot());
        setScale(getWorldInitialScale());
        resetCellUpdateInfo();
    } 


    void Pickable::resetDataError()  
    {
        m_bHasDataError = bfalse; 
    #ifdef ITF_SUPPORT_EDITOR
        m_dataErrorLabel.clear();
    #endif //ITF_SUPPORT_EDITOR
    }

    void Pickable::updateCellUpdateData(const AABB& _aabb, u8 _layerIndex)
    {
        m_lastCellUpdateData.m_aabb = _aabb;
        m_lastCellUpdateData.m_layerIndex = _layerIndex;
    }


    void Pickable::resetCellUpdateInfo()
    {
        // create invalid aabb
        m_lastCellUpdateData.m_aabb.invalidate();
        m_lastCellUpdateData.m_layerIndex = 0xff;
    }

    void Pickable::setScene( Scene* _scene )
    {
        m_sceneRef = _scene ? _scene->getRef() : ITF_INVALID_OBJREF;

        if(_scene)
        {
            World* pWorld = getWorld();
            if(pWorld)
                setAllowedCell(!pWorld->getIgnoreCells());
        }

        resetCellUpdateInfo();
    }


    bbool Pickable::isVisible(Camera* _camera) const 
    {
        return (isAlwaysVisible() || _camera->isRectVisible(getAABB(), getDepth()));
    }


    void Pickable::onAddedToScene( Scene* _pScene )
    {
        ITF_ASSERT_CRASH(!getSceneRef().isValid() , "can't register an object that belongs to another scene");
        setScene(_pScene);
    }

    void Pickable::onRemovedFromScene( Scene* _pScene ,bbool _forDeletion )
    {
        unregisterFromWorldCells(); // avoid dead objects or recycled objrefs in cells

        ITF_ASSERT_CRASH( _pScene->hasStaticSerializationFlag( Scene::flag_dependency ) || getScene() == _pScene, "Can't remove a Pickable from a scene he doesn't belong to");
        setScene(NULL);
    }

    void Pickable::enable()
    {
		if ( !m_isEnabled && isAllowedForCell() )
		{
			registerInWorldCells();
		}	
        m_isEnabled = btrue;
        
#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if(strstr(getUserFriendly().cStr(), TRACK_PICKABLE_FRIENDLY_NAME))
        {
            LOG("[Track pickable activation] Frame: %u enable()", CURRENTFRAME);
        }
#endif // TRACK_PICKABLE_FRIENDLY_NAME
    }

    void Pickable::disable()
    {
        m_isEnabled = bfalse;
        
#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if(strstr(getUserFriendly().cStr(), TRACK_PICKABLE_FRIENDLY_NAME))
        {
            LOG("[Track pickable activation] Frame: %u disable()", CURRENTFRAME);
        }
#endif // TRACK_PICKABLE_FRIENDLY_NAME
    }

    void Pickable::setAllowedCell(bbool _isAllowedForCell)
    {
        m_isAllowedForCell = _isAllowedForCell;
    }

    void Pickable::loadResources()
    {
        m_dontUnloadResourceAtFrame = CURRENTFRAME;
        if (isRequestedLoading())
            return;

#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if(strstr(getUserFriendly().cStr(), TRACK_PICKABLE_FRIENDLY_NAME))
        {
            LOG("[Track pickable activation] Frame: %u loadResources()", CURRENTFRAME);
        }
#endif // TRACK_PICKABLE_FRIENDLY_NAME

        m_bRequestedLoading = btrue;
        ResourceGroup* grp = getResourceGroup_Slow();
        if ( grp )
            RESOURCE_MANAGER->addUserToResourcePhysicalData(grp);
    }

    void Pickable::unloadResources()
    {
        if (!isRequestedLoading())
            return;

#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if (getUserFriendly().equals(TRACK_PICKABLE_FRIENDLY_NAME, btrue))
        {
            LOG("[Track pickable activation] Frame: %u unloadResources()", CURRENTFRAME);
        }
#endif // TRACK_PICKABLE_FRIENDLY_NAME

        m_bRequestedLoading = bfalse;
        m_physicalReady     = bfalse;

        ResourceGroup* grp = getResourceGroup_Slow();
        if ( grp )
            RESOURCE_MANAGER->removeUserToResourcePhysicalData(grp);
    } 

    void Pickable::generateNewUserFriendly(const String8& _prefix /*= ""*/)
    {
        static i32 iStaticCount = 0;

        String8 prefix = _prefix.getLen() > 0 ? _prefix : getUserFriendly();

        int iIdx = 0;
        char* result = prefix.strstr("@", bfalse, &iIdx);
        if(result)
            prefix.truncate(iIdx);
#ifndef ITF_CTR  //CASA::TOCHECK
        String8 friendly;
        friendly.setTextFormat("%s@%u%i", prefix.cStr(), u32(time(NULL)), ++iStaticCount);

        setUserFriendly(friendly);
#endif
    }

    void Pickable::setModified()
    {
		if (m_modified || !isAllowedForCell() || !getWorld() || !getWorld()->isActive())
            return;

		ITF_ASSERT(isCellObjectType());
        ITF_ASSERT_CRASH(Synchronize::getCurrentThreadId() == ThreadSettings::m_settings[eThreadId_mainThread].m_threadID, "This function must be called from main thread -> static list update");
        m_modified = btrue;
        m_modifiedList.push_back(getRef());
    }

    void Pickable::registerAllModifiedInWorldCells()
    {
        u32 count = m_modifiedList.size();
        for (u32 i=0; i<count; i++)
        {
            Pickable * pick = (Pickable *)GETOBJECT(m_modifiedList[i]);
            if (pick)
            {
                pick->registerInWorldCells();
                pick->m_modified = bfalse;
            }
        }
        m_modifiedList.clear();
    }

    void Pickable::setDataError(const String& _str)
    {
        m_bHasDataError = btrue; 

#ifdef  ITF_SUPPORT_EDITOR
        m_dataErrorLabel = _str;
#endif //ITF_SUPPORT_EDITOR

#ifdef ITF_SUPPORT_PLUGIN
        if (PLUGINGATEWAY && SYSTEM_ADAPTER->IsSilent())
            PLUGINGATEWAY->OnDataError(_str,this);
#endif // ITF_CONSOLE_FINAL
    }

    f32 Pickable::getReferenceDepth() const
    {
        if(Scene* pScene = getScene())
            return pScene->getReferenceDepth();

        return 0.0f;
    }

    void Pickable::onBecomeActive()
    {
        ITF_ASSERT_MSG(!m_active, "Should not be called if already active");        
        m_active = btrue;

#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if(strstr(getUserFriendly().cStr(), TRACK_PICKABLE_FRIENDLY_NAME))
        {
            LOG("[Track pickable activation] Frame: %u onBecomeActive()", CURRENTFRAME);
        }
#endif // TRACK_PICKABLE_FRIENDLY_NAME
    }

    void Pickable::onBecomeInactive()
    {
        ITF_ASSERT_MSG(m_active, "Should not be called if not active"); 
        m_active = bfalse;

#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if(strstr(getUserFriendly().cStr(), TRACK_PICKABLE_FRIENDLY_NAME))
        {
            LOG("[Track pickable activation] Frame: %u onBecomeInactive()", CURRENTFRAME);
        }
#endif // TRACK_PICKABLE_FRIENDLY_NAME
    }

    u32 Pickable::addObjectInGroupForUpdate(ObjectGroup* _listToFill, bbool _fromParent)
    {
        u32 myGroup = getObjectGroupIndex();
        ITF_ASSERT_MSG(isActive(), "IMPORTANT!! Do you really want to update an inactive object ?");
        ITF_ASSERT_MSG(isPhysicalReady() || hasDataError(), "IMPORTANT!! This may crash in next call of this pickable update() if unloaded resources are used");
        _listToFill[myGroup].addElement(this);

#ifdef TRACK_PICKABLE_FRIENDLY_NAME
        if(strstr(getUserFriendly().cStr(), TRACK_PICKABLE_FRIENDLY_NAME))
        {
            LOG("[Track pickable activation] Frame: %u addObjectInGroupForUpdate() from parent:%u", CURRENTFRAME, (u32)_fromParent);
        }
#endif // TRACK_PICKABLE_FRIENDLY_NAME

        if(_fromParent && getLastActiveFrame() < CURRENTFRAME)
        {
            if(!isRequestedLoading())
                loadResources();

            updatePhysicalReady();
            if(isPhysicalReady())
            {
                if (!isActive())
                    onBecomeActive();
                
                setLastActiveFrame(CURRENTFRAME);
                getScene()->addActiveObject(getRef());
            }
        }
        return myGroup;
    }
    
    void Pickable::addObjectInGroupForDraw()
    {
        WORLD_MANAGER->addObjectInGroupForDraw(this, getObjectGroupIndex());
    }

    ResourceGroup*  Pickable::getResourceGroup_Slow()
    {
        switch(getObjectType())
        {
            case BaseObject::eActor:
                return ((Actor*)this)->getResourceGroup();
            case BaseObject::eFrise:
                return ((Frise*)this)->getResourceGroup();
            default:
                return NULL;
        }
    }

    void Pickable::setUpdateType( UpdateType _t, bbool _propagate /*= bfalse*/ )
    {
        if(_propagate || !m_updateTypeSetByPropagation)
        {
            m_updateType = _t;
            m_lastCellEnableData.m_updateType = _t;

            m_updateTypeSetByPropagation = _propagate;
        }
    }

} //namespace ITF
