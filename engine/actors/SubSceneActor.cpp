#include "precompiled_engine.h"

#ifndef _ITF_SUBSCENEACTOR_H_
#include "engine/actors/SubSceneActor.h"
#endif //_ITF_SUBSCENEACTOR_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_SUBSCENECOMPONENT_H_
#include "engine/actors/components/SubSceneComponent.h"
#endif //_ITF_SUBSCENECOMPONENT_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_OBJECTID_SERVER_H_
#include "core/ObjectIDserver.h"
#endif //_ITF_OBJECTID_SERVER_H_

#ifndef _ITF_BASEOBJECTFACTORY_H_
#include "engine/factory/baseObjectFactory.h"
#endif //_ITF_BASEOBJECTFACTORY_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(SubSceneActor)

    BEGIN_SERIALIZATION_CHILD(SubSceneActor)

    BEGIN_CONDITION_BLOCK(ESerialize_Editor)
        SERIALIZE_MEMBER("RELATIVEPATH", m_SceneRelativePath);
        SERIALIZE_MEMBER("EMBED_SCENE", m_embedScene);
        SERIALIZE_MEMBER("IS_SINGLE_PIECE", m_isSinglePiece);
        SERIALIZE_MEMBER("ZFORCED", m_zLock);
        SERIALIZE_MEMBER("xFLIPPED", m_startFlipped);
        SERIALIZE_OBJECT("parentBind", m_pParentBind);
    END_CONDITION_BLOCK()

    BEGIN_CONDITION_BLOCK(ESerializeGroup_Data)
        SERIALIZE_MEMBER("RELATIVEPATH", m_SceneRelativePath);
        SERIALIZE_MEMBER("EMBED_SCENE", m_embedScene);
        SERIALIZE_MEMBER("IS_SINGLE_PIECE", m_isSinglePiece);
        SERIALIZE_MEMBER("ZFORCED", m_zLock);

        if(m_embedScene)
        {
            Scene* pScene = static_cast<Scene*>(m_sceneRef.getObject());

            SERIALIZE_OBJECT_WITH_FACTORY("SCENE", pScene, &BASEOBJECT_FACTORY->m_objectFactory);

            if(pScene)
                m_sceneRef = pScene->getRef();
        }
    END_CONDITION_BLOCK()

    
    SERIALIZE_FUNCTION(postSerializeLoad, ESerialize_Data_Load);
    END_SERIALIZATION()

    SubSceneActor::SubSceneActor() 
        : m_embedScene(bfalse)
        , m_sceneRef(ITF_INVALID_OBJREF)
        , m_subsceneComponent(NULL)
        , m_alpha(1.0f)
        , m_alphaSrc(1.0f)
        , m_alphaDst(1.0f)
        , m_faidingTime(1.0f)
        , m_faidingValue(0.0f)
        , m_isSinglePiece(bfalse)
        , m_repositionedOnLoad(1)
        , m_isSinglePieceFromHierarchy(0)
        , m_zLock(bfalse)
    {
        m_staticFriezesAABB = Vec2d::Zero;
    }

    SubSceneActor::~SubSceneActor()
    {
        //subscenecomponent is deleted by actor
    }

    void SubSceneActor::pauseUpdate()
    {
        Actor::pauseUpdate();

        Scene* pScene = static_cast<Scene*>(GETOBJECT(m_sceneRef));
        if(pScene)
        {
            const PickableList& actlst = pScene->getActors();
            for (u32 i=0; i<actlst.size(); i++)
            {
                Actor* pAct = (Actor*)actlst[i];
                if (pAct)
                {
                    pAct->pauseUpdate();
                }
            }
        }
    }

    
    void SubSceneActor::unpauseUpdate()
    {
        Actor::unpauseUpdate();

        Scene* pScene = getSubScene();
        if(pScene)
        {
            const PickableList& actlst = pScene->getActors();
            for (u32 i=0; i<actlst.size(); i++)
            {
                Actor* pAct = (Actor*)actlst[i];
                if (pAct)
                {
                    pAct->unpauseUpdate();
                }
            }
        }
    }


    void SubSceneActor::update( f32 _deltaTime )
    {
        Actor::update(_deltaTime);
        if (isUpdatePaused())
        {
            m_subsceneComponent->Update(_deltaTime);
        }

        processAlpha(_deltaTime);

        m_isSinglePieceFromHierarchy = 0;
    }

    void SubSceneActor::postSerializeLoad()
    {
        m_subsceneComponent = GetComponent<SubSceneComponent>();

        if ( m_subsceneComponent )
        {
            //m_subsceneComponent->SetActor(this);

            if(m_embedScene)
            {
                if(Scene* pScene = static_cast<Scene*>(m_sceneRef.getObject()))
                {
                    SCENE_MANAGER->registerExistingScene(pScene);

                    pScene->setSubSceneActorHolder(this);
                    pScene->setPath(m_SceneRelativePath);
                    m_SceneRelativePath = pScene->getPath();

                    m_subsceneComponent->setLoadedSubScene(pScene);

                    if(const Scene* pParentScene = getScene())
                    {
                        pScene->setActive(pParentScene->isActive());
                        pScene->setIsAlwaysActive(pParentScene->getIsAlwaysActive());
                    }
                }
            }
        }
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void SubSceneActor::onSceneCollect(ITF_VECTOR<Pickable*> &_vActor,ITF_VECTOR<Pickable*> &_vFrieze)
    {
        Scene* pSubScene = getSubScene();
        pSubScene->onSceneCollect(_vActor,_vFrieze);
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void SubSceneActor::onFinishLoadedFromScene(HotReloadType _hotReload) // from actor/pickable
    {
        
        m_subsceneComponent = GetComponent<SubSceneComponent>();
        ITF_ASSERT_CRASH(m_subsceneComponent, "Mandatory component missing!");

        setAlwaysVisible(btrue);

        Scene* pSubScene = getSubScene();
        if(m_embedScene)
        {
            if ( !m_repositionedOnLoad )
            {
                onLoadReposition();
            }

         //   pSubScene->onSceneLoaded();
            pSubScene->replaceReinitDataByCurrent();

            Scene* pParentScene = getScene();
            pSubScene->setActive(pParentScene->isActive());
            pSubScene->setIsAlwaysActive(pParentScene->getIsAlwaysActive());
        }
        else
        {
            ITF_ASSERT_CRASH(this->hasDataError() || (pSubScene || m_SceneRelativePath.isEmpty()), "The external subscene must be loaded, loadExternalSubscene() was called?");

            if(pSubScene) // NULL when doing editor manipulation like "group"
            {
                if ( !m_repositionedOnLoad )
                {
                    onLoadReposition();
                }

                //pSubScene->onSceneLoaded();
                pSubScene->setActive(getScene()->isActive());
                pSubScene->replaceReinitDataByCurrent();
            }
        }
        m_subsceneComponent->rebuildAABB();

        getCellEnableData().m_isSubSceneActor = 1;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    
    bbool SubSceneActor::onLoadedFromScene(HotReloadType _hotReload)
    {
        Super::onLoaded(_hotReload);

       
        return btrue;
    }

    //------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void SubSceneActor::onLoaded(HotReloadType _hotReload)
    {
        Super::onLoaded(_hotReload);

        m_subsceneComponent = GetComponent<SubSceneComponent>();
        ITF_ASSERT_CRASH(m_subsceneComponent, "Mandatory component missing!");

        setAlwaysVisible(btrue);

        Scene* pSubScene = getSubScene();
        if(m_embedScene)
        {
            if ( !m_repositionedOnLoad )
            {
                onLoadReposition();
            }

            pSubScene->onSceneLoaded();
            pSubScene->replaceReinitDataByCurrent();

            Scene* pParentScene = getScene();
            pSubScene->setActive(pParentScene->isActive());
            pSubScene->setIsAlwaysActive(pParentScene->getIsAlwaysActive());
        }
        else
        {
            ITF_ASSERT_CRASH(this->hasDataError() || (pSubScene || m_SceneRelativePath.isEmpty()), "The external subscene must be loaded, loadExternalSubscene() was called?");

            if(pSubScene) // NULL when doing editor manipulation like "group"
            {
                if ( !m_repositionedOnLoad )
                {
                    onLoadReposition();
                }

                pSubScene->onSceneLoaded();
                pSubScene->setActive(getScene()->isActive());
                pSubScene->replaceReinitDataByCurrent();
            }
        }
        m_subsceneComponent->rebuildAABB();

        getCellEnableData().m_isSubSceneActor = 1;
    }

    void SubSceneActor::onLoadReposition()
    {
        ITF_ASSERT(!m_repositionedOnLoad);

        Scene* pSubScene = getSubScene();

        if ( pSubScene ) 
        {
            pSubScene->onLoadReposition();

            const PickableList &pickables = pSubScene->getPickableList();

            for(u32 i = 0; i < pickables.size(); ++i)
            {
                Pickable* pPickable = pickables[i];
                
                if ( pPickable->DynamicCast<SubSceneActor>(ITF_GET_STRINGID_CRC(SubSceneActor,1336151817)) )
                {
                    continue;
                }

                pPickable->setPos(pPickable->getLocalInitialPos());
                pPickable->setAngle(pPickable->getLocalInitialRot());
            }

            Vec2d pos;
            f32 angle;
            f32 z;

            pos = getLocalInitialPos().truncateTo2D();
            angle = getLocalInitialRot();
            z = getLocalInitialZ();

            m_pos = Vec3d::Zero;
            m_angle = 0.f;
            setPos(pos.to3d(z));
            setAngle(angle);
        }

        m_repositionedOnLoad = 1;
    }

    Actor* SubSceneActor::clone( Scene* _pDstScene, bbool _callOnLoaded ) const
    {
        ITF_ASSERT_CRASH(_pDstScene, "cloning actor inside an empty scene");

        SubSceneActor* newSSActor = newAlloc(mId_Actor, SubSceneActor());
        BaseObjectClone(const_cast<SubSceneActor*>(this), newSSActor);

        _pDstScene->registerPickable(newSSActor);
        _pDstScene->processRegisterPending();

        if(_callOnLoaded)
        {
            if(!newSSActor->getEmbedScene())
                newSSActor->loadExternalScene();
            World* world = _pDstScene->getWorld();        
            world->addScene(newSSActor->getSubScene());

            newSSActor->onLoaded();
        }

        return newSSActor;
    }

    Scene* SubSceneActor::setSubScene(const String& _sceneRelativePath)
    {
        m_SceneRelativePath = FilePath::normalizePath(_sceneRelativePath);

        m_subsceneComponent->setExternalSubScene(_sceneRelativePath, this);
        m_sceneRef = m_subsceneComponent->getSubScene() ? m_subsceneComponent->getSubScene()->getRef() : ITF_INVALID_OBJREF;
        return getSubScene();
    }

    void SubSceneActor::setLoadedSubScene(Scene* _pScene)
    {
        ITF_ASSERT(_pScene);
        ITF_ASSERT(m_subsceneComponent);
#ifdef ASSERT_ENABLED
        World* pWorld = getWorld();
        ITF_ASSERT_CRASH(pWorld && pWorld->hasScene(_pScene), "The scene must be loaded!");
#endif // ASSERT_ENABLED

        m_SceneRelativePath = _pScene->getPath();

        if (m_subsceneComponent)
        {
            m_subsceneComponent->setLoadedSubScene(_pScene);
            
            _pScene->setSubSceneActorHolder(this);
            const Scene* pParentScene = getScene();
            if(pParentScene)
            {
                _pScene->setActive(pParentScene->isActive());
                _pScene->setIsAlwaysActive(pParentScene->getIsAlwaysActive());
            }

            m_sceneRef = _pScene->getRef();
        }
    }
    
    Scene* SubSceneActor::getSubScene()const
    {
        return m_subsceneComponent ? m_subsceneComponent->getSubScene() : NULL;
    }

    void SubSceneActor::replaceReinitDataByCurrent()
    {
        Super::replaceReinitDataByCurrent();
        
        Scene* pScene = getSubScene();
        if(pScene)
            pScene->replaceReinitDataByCurrent();
    }

    void SubSceneActor::storeCurrentToInitial()
    {
        Super::storeCurrentToInitial();

        // m_subsceneComponent->storeCurrentToInitial();
    }

    void SubSceneActor::setPos( const Vec3d& _pos )
    {
        const Vec3d deltaPos = _pos - getPos();

        Super::setPos(_pos);

        offsetSubScene(deltaPos, bfalse);
    }

    void SubSceneActor::offsetPosition (   const Vec3d& _delta, bbool _applyToInitialValue )
    {
        Super::offsetPosition(_delta, _applyToInitialValue );
    }

    
    void SubSceneActor::offsetSubScene( const Vec3d& _delta, bbool _applyToInitialValue )
    {
        //if( _delta.sqrnorm() < MTH_EPSILON ) // epsilon too big
        if(Vec3d::Zero == _delta)
            return;

        if (Scene* pScene = getSubScene())
        {
            // Process scene's elements to place them relatively to the new origin
            const PickableList &pickables = pScene->getPickableList();

            for(u32 i = 0; i < pickables.size(); ++i)
            {
                Pickable* pPickable = pickables[i];
                
                pPickable->offsetPosition(_delta, _applyToInitialValue);
            }
        }
    }

    void SubSceneActor::setAngle( f32 _angle )
    {
        const f32 deltaAngle = _angle - getAngle();

        Super::setAngle(_angle);

        rotateSubScene(deltaAngle, bfalse, NULL);
    }

    void SubSceneActor::rotateSubScene( f32 _deltaAngle, bbool _applyToInitialValue, Vec3d* _pOrigin /*= NULL*/ )
    {
        //if(f32_Abs(_deltaAngle) < MTH_EPSILON) // epsilon too big
        if(0.0f == _deltaAngle)
            return;

        if(Scene* pScene = getSubScene())
        {
            Vec3d pos = getPos();
            if(!_pOrigin)
            {
                _pOrigin = &pos;
            }

            const PickableList& content = pScene->getPickableList();
            for(u32 i = 0; i < content.size(); ++i)
            {
                Pickable* pPickable = content[i];

           /*     bbool canRotate=btrue;
                if (pPickable->getObjectType()==BaseObject::eFrise)
                {
                    Frise *frieze = (Frise*)pPickable;
                    canRotate = !frieze->isDynamic();
                }

                if (canRotate)*/
                {
                    pPickable->rotate(_deltaAngle, _applyToInitialValue, _pOrigin);
                }
            }
        }
    }

    void SubSceneActor::setScale( const Vec2d & _scale )
    {
        Vec2d deltaScale = _scale - getScale();

        Super::setScale(_scale);

        scaleSubScene(deltaScale, bfalse);
    }

    void SubSceneActor::scaleSubScene( const Vec2d& _deltaScale, bbool _applyToInitialValue )
    {
        ITF_ASSERT_MSG(!_applyToInitialValue, "Not implemented yet");
        if(Vec2d::Zero == _deltaScale)
            return;

        if(Scene* pScene = getSubScene())
        {
            const Vec2d prevScale = getScale() - _deltaScale;
    
            const Vec2d Coef = getScale() / prevScale;

            const PickableList& content = pScene->getPickableList();
            for(u32 i = 0; i < content.size(); ++i)
            {
                Pickable* pPickable = content[i];

                if ( Actor* pActor = pPickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) )
                {
                    Vec2d scaleToApply;
                    if ( pActor->getTemplate() && pActor->getTemplate()->getScaleForced() != Vec2d::Zero )
                        scaleToApply = pActor->getTemplate()->getScaleForced();
                    else
                    {
                        scaleToApply = pActor->getScale();
                        scaleToApply *= Coef;
                    }

                    pActor->setScale(scaleToApply);
                }

                const Vec3d dirToAnchor = pPickable->getAnchorPos() - getPos();
                Vec3d newDirToAnchor = dirToAnchor;
                newDirToAnchor.m_x *= Coef.m_x;
                newDirToAnchor.m_y *= Coef.m_y;

                const Vec3d deltaAnchor = newDirToAnchor - dirToAnchor;

                pPickable->setPos(pPickable->getPos() + deltaAnchor);
            }
        }
    }

    void SubSceneActor::setFlipped( bbool _value )
    {
        const bbool prevStatus = isFlipped();
        Super::setFlipped(_value);

        if(prevStatus != isFlipped())
        {
            flipContent();
        }
    }

    void SubSceneActor::hotReload( ActorHotReload& /*_hotreloadInfo*/, bbool /*_skipInstance*/ /* = bfalse */, bbool /*_delayPostreload */ )
    {
        // don't hotreload, it's meaningless for subscenes actors
    }

    void SubSceneActor::hotReload( ActorHotReload& /*_reloadInfo*/, bbool /*_delayPostreload */ )
    {
        // don't hotreload, it's meaningless for subscenes actors
    }

    void SubSceneActor::finalizeReload( bbool _wasResLoaded, bbool _wasActive, bbool _wasActiveInScene, bbool _wasAllowedForCell )
    {
        // The component have been reallocated during serialization
        m_subsceneComponent = GetComponent<SubSceneComponent>();

        if ( m_subsceneComponent )
        {
            m_subsceneComponent->SetActor(this);
        
            // The content has been replaced by serialized data, but this function should be called from mainthread
            if(m_embedScene)
            {
                Scene* subScene = m_subsceneComponent->getSubScene();
                const PickableList& content = subScene->getPickableList();
                for(u32 iObj = 0; iObj < content.size(); ++iObj)
                {
                    Pickable* pPickable = content[iObj];

                    SCENE_MANAGER->broadcastObjectAdded(subScene, pPickable);
                }
            }
        }

        Super::finalizeReload(_wasResLoaded, _wasActive, _wasActiveInScene, _wasAllowedForCell);
    }

    void SubSceneActor::saveCheckpointData()
    {
        if (getSubScene())
        {
            getSubScene()->forcedActorsSaveCheckpoint();
        }
    }

    void SubSceneActor::postLoadCheckpointData()
    {
        onLoadedCheckpoint();
    }

    void SubSceneActor::loadResources()
    {
        if(!isRequestedLoading())
        {
            Super::loadResources();
            if (isSinglePiece())
            {
                Scene* subScene = m_subsceneComponent->getSubScene();
                const PickableList& content = subScene->getPickableList();
                for(u32 iObj = 0; iObj < content.size(); ++iObj)
                {
                    Pickable* pPickable = content[iObj];
                    pPickable->loadResources();
                }
            }
        }
    }

    void SubSceneActor::processAlpha(f32 _deltaTime)
    {
        if (m_faidingValue <= 0.f|| m_faidingTime <= 0.f) 
            return;

        m_faidingValue -= _deltaTime;
        if (m_faidingValue <= 0.f)
        {
            m_alpha = m_alphaDst;
        }
        else
    {
            f32 t = 1.f - ( m_faidingValue / m_faidingTime );
            m_alpha = Interpolate(m_alphaSrc,m_alphaDst,t);
        }
        setAlpha(m_alpha);
    }

    void SubSceneActor::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        // relay events to children
        if ( Scene* pScene = static_cast<Scene*>(m_sceneRef.getObject()) )
        {
            const PickableList& children = pScene->getPickableList();
            const u32 childrenCount = children.size();

            for (u32 i = 0; i < childrenCount; i++)
            {
                children[i]->onEvent(_event);
            }
        }

        if ( EventShow* eventShow = _event->DynamicCast<EventShow>(ITF_GET_STRINGID_CRC(EventShow,111532112)) )
        {
            Scene* pScene = static_cast<Scene*>(GETOBJECT(m_sceneRef));
            if (pScene)
            {
                const PickableList& actlst = pScene->getActors();
                for (u32 i=0; i<actlst.size(); i++)
                {
                    Actor* pAct = (Actor*)actlst[i];
                    if (pAct)
                    {
                        pAct->onEvent(eventShow);
                    }
                }
            }

            // frize alpha
            {
                m_alphaSrc      = m_alpha;
                m_alphaDst      = eventShow->getAlpha();
                m_faidingTime   = eventShow->getTransitionTime();
                if (m_faidingTime <= 0.f)
                    m_alpha         = m_alphaDst;
                else
                    m_faidingValue  = m_faidingTime;
            }
        }
    }

    void  SubSceneActor::setAlpha(f32 _alpha)
    {
        Scene* pScene = static_cast<Scene*>(GETOBJECT(m_sceneRef));
        if(pScene)
        {
            const PickableList& frzList = pScene->getFrises();
            for (u32 i=0; i<frzList.size(); i++)
            {
                if (Frise* pFrise = (Frise*)frzList[i])
                {
                    pFrise->setglobalAlphaFade(_alpha);
                }
            } 
            const PickableList& actList = pScene->getActors();
            for (u32 i=0; i<actList.size(); i++)
            {
                if (Actor* pAct = (Actor*)actList[i])
                {
                    GraphicComponent* pGraphic = pAct->GetComponent<GraphicComponent>();
                    if (pGraphic)
                        pGraphic->setAlpha(_alpha);
                }
            } 
        }
    }

    void SubSceneActor::loadExternalScene()
    {
        ITF_ASSERT(!m_embedScene);
        ITF_ASSERT(m_subsceneComponent);

        PathString_t pathName;

        m_SceneRelativePath.getString(pathName);

        setSubScene(pathName);
    }

    void SubSceneActor::onLoadedCheckpoint()
    {
        m_alpha = 1.f;
        m_alphaSrc = 1.f;
        m_alphaDst = 1.f;
        m_faidingTime = 1.f;
        m_faidingValue = 0.f;

        setLocalAngle(0.f);
        setAngle(0.f);

        Scene* subScene = m_subsceneComponent->getSubScene();
        const PickableList& content = subScene->getPickableList();
        setPos(getWorldInitialPos());
        setAngle(getWorldInitialRot());
        for(u32 iObj = 0; iObj < content.size(); ++iObj)
        {
            Pickable* pPickable = content[iObj];
            if (pPickable)
            {
                if (!pPickable->getCellEnableData().m_isStatic)
                {
                    pPickable->setAngle(pPickable->getWorldInitialRot());
                    pPickable->setPos(pPickable->getWorldInitialPos());
                }
            }
        }

      m_subsceneComponent->rebuildAABB();  
      Super::onLoadedCheckpoint();
    }

    void SubSceneActor::setZLock( bbool _b )
    {
        m_zLock = _b;
    }

    void SubSceneActor::updateCurrentWorldDataContentFromLocal()
    {
        if(Scene* subScene = m_subsceneComponent->getSubScene())
        {
            const PickableList& content = subScene->getPickableList();
            for(u32 iObj = 0; iObj < content.size(); ++iObj)
            {
                Pickable* pPickable = content[iObj];

                pPickable->setPos(pPickable->getWorldInitialPos());
                pPickable->setScale(pPickable->getWorldInitialScale());
                pPickable->setAngle(pPickable->getWorldInitialRot());
            }
        }
    }

    void SubSceneActor::registerStaticFriezeAABB(const AABB& _aabb)
    {
        m_staticFriezesAABB.grow(AABB(_aabb.getMin()-get2DPos(), _aabb.getMax()-get2DPos()));
    }

    void SubSceneActor::growInternalRelativeAABBFromAbsolute(const AABB &_aabb, f32 _z)
    {
        m_subsceneComponent->growInternalRelativeAABBFromAbsolute(_aabb, _z);
    }

    u32 SubSceneActor::addObjectInGroupForUpdate(ObjectGroup* _listToFill, bbool _fromParent /*= bfalse*/)
    {
        if (!getBinding()->isBound() || isParentBindRunTimeDisabled() || _fromParent)    // bound objects can't register themselves, unless from thei parent querry
        {
            ITF_ASSERT( !isParentBindRunTimeDisabled() || !_fromParent ); //check that if parent bind is runtime disabled, my parent is not trying to push me in list
            if(isEnabled())
            {
                WORLD_MANAGER->getUpdateGroups()[ObjectGroup::GroupIndex_Max - 1].addSubSceneAABBRefresh(this);

                if(Scene* subScene = m_subsceneComponent->getSubScene())
                {
                    if (isSinglePiece() || m_isSinglePieceFromHierarchy)
                    {
                        // Visibility test is not relevant anymore as SubSceneActors are not always actives 
                        // and the element inside may be enabled before the group by cell and not by visibility.
                        // It prevent falling actors issue when they are spawned over a platform with a movement synchronized with the metronome

                        /*Camera* cam = CAMERA;
                    
                        f32 camMinX = cam->m_minAABBAtAZZero.m_x;
                        f32 camMinY = cam->m_maxAABBAtAZZero.m_y;
                        f32 camMaxX = cam->m_maxAABBAtAZZero.m_x;
                        f32 camMaxY = cam->m_minAABBAtAZZero.m_y;
                        const f32 deltaX = cam->getDeltaX();
                        const f32 deltaY = cam->getDeltaY();

                        AABB aabbAtZero = getAABB();
                        aabbAtZero.projectTo2D(deltaX,deltaY, getDepth());

                        if(
                            !(
                                camMinX > (aabbAtZero.getMax().m_x) ||  // optimized version of aabb::checkOverlap feturing pre-backed epsilon
                                camMinY > (aabbAtZero.getMax().m_y) ||
                                camMaxX < (aabbAtZero.getMin().m_x) ||
                                camMaxY < (aabbAtZero.getMin().m_y)
                             )
                            )
                        */
                        {
                            subScene->clearActiveObjectsList();
                            const PickableList& content = subScene->getPickableList();
                            for(u32 iObj = 0; iObj < content.size(); ++iObj)
                            {
                                Pickable* pPickable = content[iObj];

                                if(pPickable->isEnabled())
                                {
                                    if(!pPickable->isRequestedLoading())
                                        pPickable->loadResources();

                                    pPickable->updatePhysicalReady();
                                    if(pPickable->isPhysicalReady())
                                    {
#ifdef TRACK_PICKABLE_FRIENDLY_NAME
                                        if(strstr(pPickable->getUserFriendly().cStr(), TRACK_PICKABLE_FRIENDLY_NAME))
                                        {
                                            LOG("[Track pickable activation] Frame: %u activation by synchro group", CURRENTFRAME);
                                        }
#endif // TRACK_PICKABLE_FRIENDLY_NAME

                                        if (!pPickable->isActive())
                                            pPickable->onBecomeActive();
                                        
                                        pPickable->setLastActiveFrame(CURRENTFRAME);
                                        subScene->addActiveObject(pPickable->getRef());
                                    }
                                }
                            }

                            for(u32 i = 0; i < subScene->getSubSceneActors().size(); ++i)
                            {
                                SubSceneActor* pSSA = (SubSceneActor*)subScene->getSubSceneActors()[i].getObject();
                                pSSA->setSinglePieceFromHierarchy();
                            }
                        }
                    }
                    subScene->fillUpdateList(_listToFill);
                }
            }
            Super::addObjectInGroupForUpdate(_listToFill, _fromParent);
        }
        return getObjectGroupIndex();
    }
    
    void SubSceneActor::addObjectInGroupForDraw()
    {
        if (m_subsceneComponent)
        {
            if(Scene* subScene = m_subsceneComponent->getSubScene())
            {
                if (subScene)
                    subScene->fillDrawList();
            }
        }
    }

    bbool   SubSceneActor::updatePhysicalReady() 
    {
        if (hasDataError())
        {
            setPhysicalReady(btrue);
            return btrue;
        }

        if (m_subsceneComponent && !m_subsceneComponent->isPhysicalReady())
            return bfalse;
        return Super::updatePhysicalReady();
    }
    
    void SubSceneActor::registerContainedPickables(SafeArray<ActorRef>& _actorList, SafeArray<ObjectRef>& _frizeList) const
    {
        if (isDestructionRequested())
            return;

        _actorList.push_back(getRef());

        Scene* pScene = static_cast<Scene*>(m_sceneRef.getObject());
        if(pScene)
        {
            const PickableList& actList = pScene->getActors();
            for (u32 i=0; i<actList.size(); i++)
            {
                const Actor* pAct = (const Actor*)actList[i];
                pAct->registerContainedPickables(_actorList,_frizeList);
            } 
            const PickableList& friezeList = pScene->getFrises();
            for (u32 i=0; i<friezeList.size(); i++)
            {
                const Frise* pFrz = (const Frise*)friezeList[i];
               _frizeList.push_back(pFrz->getRef());
            } 
        }
    }

    void SubSceneActor::deleteSpawnedActors()
    {
        Scene* pScene = static_cast<Scene*>(m_sceneRef.getObject());
        if(pScene)
        {
            PickableList& actList = pScene->getActorsMutable();
            for (u32 i=0; i<actList.size(); i++)
            {
                Actor* pAct = (Actor*)actList[i];
                pAct->deleteSpawnedActors();
            } 
        }
    }

    void SubSceneActor::flipContent()
    {
        if(Scene* subScene = m_subsceneComponent->getSubScene())
        {
            const PickableList& actors = subScene->getActors();
            for(u32 iObj = 0; iObj < actors.size(); ++iObj )
            {
                Actor* pActor = static_cast<Actor*>(actors[iObj]);

                Vec3d newLocalPos = pActor->getLocalPos();
                newLocalPos.m_x = -newLocalPos.m_x;
                pActor->setLocalPos(newLocalPos);
                pActor->setLocalAngle(-pActor->getLocalAngle());

                pActor->setFlipped(!pActor->isFlipped());
            }
        }
    }

    void SubSceneActor::setUpdateType( UpdateType _t, bbool _propagate /*= bfalse*/ )
    {
        Super::setUpdateType(_t, _propagate);

        if(_propagate)
        {
            if(Scene* pScene = getSubScene())
            {
                for(u32 i = 0; i < pScene->getPickableList().size(); ++i)
                {
                    Pickable* pObj = (Pickable*)pScene->getPickableList()[i];
                    pObj->setUpdateType(_t, _propagate);
                }
            }
        }
    }

    void SubSceneActor::onSceneActive()
    {
        Super::onSceneActive();

        if(isSinglePiece() && isUpdateTypeFromTemplateOverridden())
        {
            setUpdateType(getUpdateType(), btrue);
        }
    }

}
