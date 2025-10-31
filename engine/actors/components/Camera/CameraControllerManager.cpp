#include "precompiled_engine.h"

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_CHEATMANAGER_H_
#include "engine/gameplay/CheatManager.h"
#endif //_ITF_CHEATMANAGER_H_

#ifndef _ITF_CAMERA_H_
#include "engine/display/Camera.h"
#endif //_ITF_CAMERA_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_GAMEPLAYEVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_GAMEPLAYEVENTS_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    CameraControllerManager* CameraControllerManager::s_instance = NULL;
    bbool initDone = bfalse;

    CameraControllerManager::CameraControllerManager()
        : m_camSubjectList(10)
        , m_camModifierList(50)
        , m_camControllerList(10)
        , m_camSubjectList_Active(10)
        , m_camModifierList_Active(50)
        , m_rampUpDestinationCoeff(0)
        , m_rampUpCoeff(0)
        , m_camFocaleInGame(0.f)
        , m_camPosInGame(Vec3d::Zero)
        , m_pauseMode(bfalse)
        , m_ignoreShake(bfalse)
        , m_ignoreShakeFromCageBehaviour(bfalse)
        , m_shakeCountdown(0.0f)
        , m_shake()
        , m_shakeConfig(NULL)
        , m_defaultDepth(13.f)
#ifdef ITF_SUPPORT_DEBUGFEATURE
        , m_debugDrawInfo(bfalse)
        , m_debugDrawScreen4By3(None)
        , m_shakeDebugDraw(bfalse)
        , m_debugDrawModifiers(bfalse)
#endif //ITF_SUPPORT_DEBUGFEATURE
        , m_controllerJustTeleported(bfalse)
        , m_depth(0.0f)
        , m_screenRatio(1.f)

        , m_isCheatDezoomMax(bfalse)
        , m_cheatZoom(0.f)

        , m_updateCamModifierList_Active(bfalse)
        , m_updateCamSubjectList_Active(bfalse)
        , m_updateCamControllers_SubjectList(bfalse)
    {
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventRegisterCameraSubject,1683052496),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventCameraShake,458805059),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventCameraIgnoreShake,1831643083),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252),this);

        m_cameraSequence.m_camSubjectList.reserve(10);

#ifdef DEVELOPER_JAY_CAMERA
#ifndef ITF_FINAL
        m_debugDrawInfo = btrue;
        m_debugDrawModifiers = btrue;
#endif 
#endif
    }

    CameraControllerManager::~CameraControllerManager()
    {
        if (TEMPLATEDATABASE && GAMEMANAGER)
            TEMPLATEDATABASE->removeTemplateClient(GAMEMANAGER->getCameraShakeConfig().getStringID(), this);

        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventRegisterCameraSubject,1683052496), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventCameraShake,458805059), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventCameraIgnoreShake,1831643083), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252), this);

        if(initDone)
            WORLD_MANAGER->removeListener(this);
    }  

    void CameraControllerManager::init()
    {
        WORLD_MANAGER->addListener(s_instance);

        // load camera shake config file
        const Path& configFile = GAMEMANAGER->getCameraShakeConfig();
        TEMPLATEDATABASE->addTemplateClient(configFile.getStringID(), this);
        m_shakeConfig = TEMPLATEDATABASE->getTemplate<CameraShakeConfig_Template>(this, configFile);
        if (!m_shakeConfig)
        {
            ITF_FATAL_ERROR("Couldn't load camera shake config: %s", configFile.getString8().cStr());
        }

        m_bindedObjects.clear();
        m_bindCameraMoveDelta.set( 0.f,0.f,0.f );
        m_bindLastCameraPos.set( 0.f,0.f,0.f );

        initDone = btrue;
    }

    void CameraControllerManager::setTeleported()
    {
        m_controllerJustTeleported = btrue;
    
        // notify the controllers
        const u32 camControllerCount = m_camControllerList.size();   
        for (u32 i = 0; i < camControllerCount; i++)
        {
            CameraControllerData& data = m_camControllerList[i];
            if (data.m_controller)
            {
                data.m_controller->teleport(m_camPosInGame);
                data.m_destinationWeight = data.m_finalWeight;
                data.m_weight = data.m_finalWeight;
                data.m_prevSpeed = Vec3d::Zero;
                data.m_prevPos = m_camPosInGame;
            }
            else
                data.m_weight = 0.f;
        }

        // notify the modifiers
        for(u32 i= 0; i < m_camModifierList.size(); ++i)
        {
            m_camModifierList[i]->resetExtension();
        }

        // reset sequence
        stopCameraSequence(1.f);

        // reset mix
        m_rampUpCoeff = 1.f;
        m_rampUpDestinationCoeff = 1.f;

        // reset shake
        resetShake();
    }


    void CameraControllerManager::teleport(const Vec3d& _newPos)
    {
        setIgnoreShakeCamFromCageBehaviour(bfalse);
        updateCameraControllerSubjectActiveList();
        updateCameraModifierComponentActiveList();

        m_camPosInGame = _newPos;
        setTeleported();
        
        // Because teleport may be called before WorldManager::startFrame
        // The main view must be updated right now to prevent load/unload issues with GameManagerSequence's prefetch
        onEndUpdateProcess(LOGICDT);

        m_camPosInGame = _newPos;
        setTeleported();
    }

    void CameraControllerManager::updateScreenRatio()
    {
        m_screenRatio = CAMERA->m_screen_ratio;

#ifdef ITF_SUPPORT_DEBUGFEATURE
        if ( isDebugDrawScreen4By3() )
        {
            m_screenRatio = Min( m_screenRatio, 1.333333f);
        }
#endif
    }

    void CameraControllerManager::onEndUpdateProcess(f32 _dt)
    {
        if(LOADINGSCREEN->isVisible()) // Very important : avoids camera teleportation while loading, that provokes unwanted resources loading
            return;

        updateScreenRatio();
        updateCameraControllerSubjectActiveList();
        updateCameraModifierComponentActiveList();

        if ( m_controllerJustTeleported || ( !GAMEMANAGER->isInPause() && !m_pauseMode 
            && (!CHEATMANAGER->getPause() || CHEATMANAGER->getPauseStep())) )
        {
            // update or remove camera controllers
            for (i32 i = 0; i < (i32)m_camControllerList.size(); i++)
            {
                const CameraControllerData& data = m_camControllerList[i];

                if (data.m_controller)
                {
                    if ( isCameraControllerActive(data.m_controller))
                    {
                        data.m_controller->onManagerUpdate(_dt);
                    }
                }
                else if ( !cameraControllerHasInfluence(data) )
                {
                    m_camControllerList.eraseNoOrder(i);
                    i--;
                }
            }

            Vec3d prevCamPosInGame = m_camPosInGame;

            // mix the cameras (components)
            if ( m_camControllerList.size() && m_mainCamStack.size() )
            {
                updateCameraSequenceWeight();

                m_camPosInGame = m_cameraSequence.m_pos *m_cameraSequence.m_weight;
                m_camFocaleInGame = m_cameraSequence.m_focale *m_cameraSequence.m_weight;
                m_depth = m_cameraSequence.m_depth *m_cameraSequence.m_weight;

                f32 totalWeight = m_cameraSequence.m_weight;

                for (u32 i = 0; i < m_camControllerList.size(); i++)
                {
                    CameraControllerData& data = m_camControllerList[i];

                    data.m_destinationWeight = data.m_destinationWeight + (data.m_finalWeight - data.m_destinationWeight) * m_rampUpDestinationCoeff;
                    data.m_weight = data.m_weight + (data.m_destinationWeight - data.m_weight) * m_rampUpCoeff;
                    totalWeight += data.m_weight;

                    Vec3d pos;
                    f32 focale;
                    f32 depth = 0.0f;
                    if (data.m_controller)
                    {
                        pos = data.m_controller->getCameraPosition();
                        focale = data.m_controller->getCameraFocale();

                        if (data.m_prevPos == Vec3d::Infinity)
                            data.m_prevSpeed = Vec3d::Zero;
                        else if ( _dt )
                            data.m_prevSpeed = (pos - data.m_prevPos) / _dt;
                        data.m_prevPos = pos;
                        data.m_prevFocale = focale;
                        depth = data.m_controller->getDepth();
                    }
                    else // cam was de-registered but still has influence: let it drift
                    {
                        ITF_ASSERT(data.m_prevPos != Vec3d::Infinity);
                        data.m_prevPos += data.m_prevSpeed * _dt;
                        pos = data.m_prevPos;
                        focale = data.m_prevFocale;
                    }
                    //GFX_ADAPTER->drawDBGCircle(pos.truncateTo2D(), 0.3f, data.m_controller ? Color::green() : Color::blue());

                    m_camPosInGame += pos * data.m_weight;
                    m_camFocaleInGame += focale * data.m_weight;
                    m_depth += depth * data.m_weight;
                }

                if (totalWeight > 0.f)
                {
                    const f32 oneOverWeight = 1.0f/totalWeight;

                    m_camPosInGame *= oneOverWeight;
                    m_camFocaleInGame *= oneOverWeight;
                    m_depth *= oneOverWeight;
                }
                else
                {
                    m_camPosInGame = Vec3d::Zero;
                    m_camFocaleInGame = MTH_PIBY4;
                    m_depth = 0.0f;
                }

                //GFX_ADAPTER->drawDBGCircle(m_camPosWanted.truncateTo2D(), 0.3f, Color::red());
            }

            if (m_shakeCountdown>0.0f)
            {
                // offset camera position
                f32 t = (m_shake.getDuration() - m_shakeCountdown);
                m_camPosInGame.m_x += m_shake.getShakeX().getValue(t);
                m_camPosInGame.m_y += m_shake.getShakeY().getValue(t);
                m_camPosInGame.m_z += m_shake.getShakeZ().getValue(t);

                // countdown, reset to shake when done
                m_shakeCountdown -= _dt;
                if (m_shakeCountdown < 0.0f)
                {
                    resetShake();
                }
            }

            if (_dt)
            {
                m_camSpeedInGame = (m_camPosInGame - prevCamPosInGame) / _dt;
            }


            if ( CAMERA->getMode() == Camera::Camera_InGame )     
                GFX_ADAPTER->setCamera(m_camPosInGame.m_x, m_camPosInGame.m_y, m_camPosInGame.m_z, CAMERA);
            else if ( isCameraSequenceActive() )
                stopCameraSequence( 1.f );               

            // Update "camera" binded objects
            //
            if ( m_bindLastCameraPos.IsEqual(Vec3d::Zero) )
            {
                m_bindLastCameraPos = m_camPosInGame;
            }
            m_bindCameraMoveDelta = m_camPosInGame - m_bindLastCameraPos;

            for( u32 i = 0; i < m_bindedObjects.size(); ++i )
            {
                BindedObject & bindedObj = m_bindedObjects[i];
                BaseObject * obj = bindedObj.m_objectRef.getObject();
                if ( obj != NULL )
                {
                    if ( Pickable * pickable = obj->DynamicCast<Pickable>(ITF_GET_STRINGID_CRC(Pickable,594660361)) )
                    {
                       Vec3d newPos = pickable->getPos();
                        newPos += m_bindCameraMoveDelta;

                        pickable->setPos( newPos );
                        if ( bindedObj.m_updateInitPos )
                        {
                            pickable->setWorldInitialPos( pickable->getWorldInitialPos() + m_bindCameraMoveDelta );
                        }
                    }
                    else
                    {
                        ITF_ASSERT_MSG( 0, "Camera Binded objects must be a pickable object !" );
                    }
                }
                else
                {
                    m_bindedObjects.eraseNoOrder( i );
                    i--;
                }
            }
            m_bindLastCameraPos = m_camPosInGame;

            // reset values
            m_controllerJustTeleported = bfalse;
            m_updateCamControllers_SubjectList = bfalse;
        }

        for ( u32 i=0; i<m_camSubjectList.size(); i++)
        {
            CameraControllerSubject& subject = m_camSubjectList[i];

            subject.m_subjectPerform.reset();
            subject.m_refOld = ITF_INVALID_OBJREF;
        }

        // update aabb
        Vec2d screenRealLongBy2;
        screenRealLongBy2.m_y = f32_Abs( tanf(m_camFocaleInGame * 0.5f) * m_depth);
        screenRealLongBy2.m_x = screenRealLongBy2.m_y *m_screenRatio;
        m_aabb.setMin( m_camPosInGame.truncateTo2D() -screenRealLongBy2 );
        m_aabb.setMax( m_camPosInGame.truncateTo2D() +screenRealLongBy2 );   

#ifdef ITF_SUPPORT_DEBUGFEATURE
        drawDebug();
#endif //ITF_SUPPORT_DEBUGFEATURE
    }

    void CameraControllerManager::onEvent( Event* _event )
    {
        if ( EventRegisterCameraSubject* eventRegister = _event->DynamicCast<EventRegisterCameraSubject>(ITF_GET_STRINGID_CRC(EventRegisterCameraSubject,1683052496)) )
        {
            processEventRegisterSubject(eventRegister);
        }
        else if ( EventCameraShake* shake = _event->DynamicCast<EventCameraShake>(ITF_GET_STRINGID_CRC(EventCameraShake,458805059)) )
        {
            addShake(shake->getName());
        }
        else if ( EventCameraIgnoreShake* ignoreShake = _event->DynamicCast<EventCameraIgnoreShake>(ITF_GET_STRINGID_CRC(EventCameraIgnoreShake,1831643083)) )
        {
            m_ignoreShake = ignoreShake->getIgnoreShake();
        }
#ifdef ITF_SUPPORT_HOTRELOAD
        else if (EventSingletonConfigChanged* configChanged = _event->DynamicCast<EventSingletonConfigChanged>(ITF_GET_STRINGID_CRC(EventSingletonConfigChanged,32858252)))
        {
            // reload camera shake config file
            String relativeFile = configChanged->getAbsoluteFilename();
            FilePath::transformToRelative(relativeFile);
            Path relativeFilePath(relativeFile);
            const Path& configFile = GAMEMANAGER->getCameraShakeConfig();
            if (relativeFilePath == configFile)
            {
                m_shakeConfig = TEMPLATEDATABASE->reloadTemplate<CameraShakeConfig_Template>(configFile);
                if (!m_shakeConfig)
                {
                    ITF_FATAL_ERROR("Couldn't reload camera shake config: %s", configFile.getString8().cStr());
                }
            }
        }
#endif //ITF_SUPPORT_HOTRELOAD

    }

    void CameraControllerManager::addShake(const StringID& _name)
    {
        if ( m_ignoreShake || m_ignoreShakeFromCageBehaviour )
        {
            return;
        }

        // warn and give up if name is invalid
        if (!_name.isValid())
        {
            ITF_WARNING(NULL, bfalse, "Invalid camera shake ID");
            return;
        }

        // camera shake config *has to* be loaded
        if (!m_shakeConfig)
        {
            ITF_ASSERT_MSG(m_shakeConfig, "Camera shake config missing");
            return;
        }

        // search for shake
        ITF_VECTOR<CameraShake>::const_iterator it; 
        for (it = m_shakeConfig->getShakes().begin();
            it != m_shakeConfig->getShakes().end();
            ++it)
        {
            if (it->getName() == _name)
            {
                break;
            }
        }

        // warn and give up if not found
        if (it == m_shakeConfig->getShakes().end())
        {
            ITF_WARNING(NULL, bfalse, "Camera shake %s undefined", _name.getDebugString());
            return;
        }

        // give up if not setup properly (something went wrong with serialization)
        if (!it->isSetup())
        {
            return;
        }

        // copy shake, apply master intensity & init countdown timer
        m_shake = *it;
        m_shakeCountdown = m_shake.getDuration();
    }

    void CameraControllerManager::onMapLoaded()
    {
        // flush unregistered controllers
        for (u32 i = 0; i < m_camControllerList.size();)
        {
            CameraControllerData& data = m_camControllerList[i];

            if (!data.m_controller)
            {
                m_camControllerList.eraseNoOrder(i);
            }
            else
            {
                i++;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////    
    // Camera Controllers
    //////////////////////////////////////////////////////////////////////////

    void CameraControllerManager::registerCameraController( ICameraController* _cam )
    {
        // don't register twice
        const u32 camControllerCount = m_camControllerList.size();
        for (u32 i = 0; i < camControllerCount; i++)
        {
            if (m_camControllerList[i].m_controller == _cam)
            {
                return;
            }
        }

        CameraControllerData data;
        data.m_controller = _cam;
        data.m_prevPos = Vec3d::Infinity;
        m_camControllerList.push_back( data );
    }

    void CameraControllerManager::unregisterCameraController( ICameraController* _cam )
    {
        const u32 camControllerCount = m_camControllerList.size();
        for (u32 i = 0; i < camControllerCount; i++)
        {
            if (m_camControllerList[i].m_controller == _cam)
            {
                m_camControllerList[i].m_controller = NULL;
                m_camControllerList[i].m_finalWeight = 0;

                if (m_camControllerList[i].m_prevPos == Vec3d::Infinity)
                {
                    // Register and unregister in the SAME frame
                    m_camControllerList.eraseNoOrder(i);
                }
                return;
            }
        }
    }

    bbool CameraControllerManager::cameraControllerHasInfluence( const ICameraController* _cam ) const
    {        
        for (u32 i = 0; i < m_camControllerList.size(); i++)
        {
            const CameraControllerData& data = m_camControllerList[i];

            if ( data.m_controller == _cam)
            {
                return cameraControllerHasInfluence(data);
            }
        }

        return bfalse;
    }

    bbool CameraControllerManager::isMainCameraController( const ICameraController* _cam )
    {
        const u32 mainCamStackSize = m_mainCamStack.size();
        if ( mainCamStackSize )
        {
            if ( m_mainCamStack[mainCamStackSize -1] == _cam )
                return btrue;
        }

        return bfalse;
    }

    void CameraControllerManager::setMainCameraController( ICameraController* _mainCam, f32 _rampUpDestinationCoeff, f32 _rampUpCoeff )
    {
        if (m_mainCamStack.find(_mainCam) < 0)
            m_mainCamStack.push_back(_mainCam);

        m_rampUpCoeff = _rampUpCoeff;
        m_rampUpDestinationCoeff = _rampUpDestinationCoeff;

        const u32 camControllerCount = m_camControllerList.size();
        for (u32 i = 0; i < camControllerCount; i++)
        {
            m_camControllerList[i].m_finalWeight = (m_camControllerList[i].m_controller == _mainCam) ? 1.f : 0.f;
        }
    }

    void CameraControllerManager::setMainCameraControllerActive( f32 _destinationCoeff, f32 _coeff )
    {
        for ( i32 i =m_mainCamStack.size()-1; i>=0; i--)
        {
            ICameraController* mainCam = m_mainCamStack[i];

            if ( isCameraControllerActive(mainCam) )
            {
                setMainCameraController( mainCam, _destinationCoeff, _coeff );
                break;
            }
        }
    }

    void CameraControllerManager::removeMainCameraController( ICameraController* _mainCam, f32 _rampDownDestinationCoeff, f32 _rampDownCoeff )
    {
        i32 pos = m_mainCamStack.find(_mainCam);
        if (pos >= 0)
        {
            m_mainCamStack.eraseNoOrder(pos);

            setMainCameraControllerActive( _rampDownDestinationCoeff, _rampDownCoeff );
        }
        // else: assert?
    }

    ICameraController* CameraControllerManager::getMainCameraController() 
    {
        const u32 camControllerCount = m_camControllerList.size();
        for (u32 i = 0; i < camControllerCount; i++)
        {
            const CameraControllerData& data = m_camControllerList[i];
            if ( data.m_finalWeight == 1.f )
            {
                ITF_ASSERT_MSG(data.m_controller, "the main controller is not supposed to be null!");
                return data.m_controller;
            }
        }

        return NULL;
    }

    //////////////////////////////////////////////////////////////////////////
    // Camera Modifiers
    //////////////////////////////////////////////////////////////////////////

    bbool CameraControllerManager::isCamModifierComponentActive( CameraModifierComponent* _camModifier )
    {
        if ( m_camModifierList_Active.find( _camModifier) < 0 )
            return bfalse;

        return btrue;
    }

    void CameraControllerManager::unregisterCamModifierComponent( CameraModifierComponent* _camModifier )
    {
        i32 index = m_camModifierList.find( _camModifier);

        if ( index > -1 )
        {
            m_camModifierList.eraseNoOrder( index );
            m_updateCamModifierList_Active = btrue;
        }

        i32 index_Active = m_camModifierList_Active.find( _camModifier);
        if ( index_Active > -1 )
        {
            m_camModifierList_Active.eraseNoOrder( index_Active );
        }
    }

    void CameraControllerManager::registerCamModifierComponent( CameraModifierComponent* _camModifier )
    {
        i32 index = m_camModifierList.find( _camModifier);

        if( index == -1 )
        {
            m_camModifierList.push_back( _camModifier );
            m_updateCamModifierList_Active = btrue;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Subjects
    //////////////////////////////////////////////////////////////////////////
    void CameraControllerManager::removeInvalidSubjects()
    {
        for ( u32 i=0; i<m_camSubjectList.size();)
        {
            const Actor* actor = (Actor*) GETOBJECT(m_camSubjectList[i].m_ref);

            if ( !actor )
            {
                m_camSubjectList.eraseNoOrder(i); 
                m_updateCamSubjectList_Active = btrue;
            }
            else
            {
                i++;
            }
        }        
    }

    i32 CameraControllerManager::getSubjectIndex( ObjectRef _subject ) const
    {
        const u32 subjectCount = m_camSubjectList.size();
        for(u32 i=0; i<subjectCount; i++)
        {
            if( m_camSubjectList[i].m_ref==_subject )
            {
                return i;
            }
        }

        return -1;
    }

    void CameraControllerManager::registerSubject( ObjectRef _subjectRef, f32 _delayToRegister, bbool _isPlayer, bbool _doTeleport, bbool _leadCamera )
    {
        removeInvalidSubjects();

        if( getSubjectIndex(_subjectRef ) == -1 )
        {
            CameraControllerSubject newSubject;
            newSubject.m_ref = _subjectRef;
            newSubject.m_delayToRegister = _delayToRegister;
            newSubject.m_isPlayer = _isPlayer;
            newSubject.m_leadCamera = _leadCamera;

            m_camSubjectList.push_back( newSubject );

            if ( _doTeleport && _isPlayer && getSubjectPlayerListCount() == 1 )
            {
                if ( const Actor* actor = (Actor*) GETOBJECT(_subjectRef) )
                {
                    teleport( actor->getPos() );
                }                
            }

            m_updateCamSubjectList_Active = btrue;
        }
    }

    void CameraControllerManager::unregisterAllSubjects()
    {
        m_camSubjectList.clear(); 
        m_updateCamSubjectList_Active = btrue; 
    }

    void CameraControllerManager::unregisterSubject( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );

        if( index >= 0 )
        {
            if ( m_camSubjectList[index].m_isPlayer && getSubjectPlayerListCount() == 1 )
            {
                unregisterAllSubjects();
                return;
            }

            m_camSubjectList.eraseNoOrder(index); 
            m_updateCamSubjectList_Active = btrue; 
        }      

        removeInvalidSubjects();
    }

    void CameraControllerManager::swapSubjects( ObjectRef _subjectRefFrom, ObjectRef _subjectRefTo )
    {
        i32 index = getSubjectIndex( _subjectRefFrom );

        if( index >= 0 )
        {
            CameraControllerSubject& subject = m_camSubjectList[index];
            subject.m_ref = _subjectRefTo;
            subject.m_refOld = _subjectRefFrom;

            m_updateCamSubjectList_Active = btrue;
        }
        else
        {
            ITF_ASSERT_MSG(1, "Subject was not registered");
            registerSubject( _subjectRefTo );
        }        
    }

    u32 CameraControllerManager::getSubjectListCount()
    {
        return m_camSubjectList.size();
    }

    u32 CameraControllerManager::getSubjectPlayerListCount()
    {
        u32 subjectListCount = m_camSubjectList.size();
        u32 playerCount = 0;

        for ( u32 i = 0; i < subjectListCount; i++ )
        {
            if ( m_camSubjectList[i].m_isPlayer )
            {
                playerCount++;
            }
        }

        return playerCount;
    }

    void CameraControllerManager::getSubjectByIndex( u32 _index, CameraControllerSubject& _subject)
    {
        if( _index >= getSubjectListCount() )
        {
            _subject.m_ref = NULL;
            ITF_ASSERT_MSG(1, "Subject Camera not found in list");
            return;
        }

        _subject = m_camSubjectList[_index];    
    }

    void CameraControllerManager::setSubjectJustJumped( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            m_camSubjectList[index].m_subjectPerform.m_justJumped = btrue;
        }
    }

    bbool CameraControllerManager::isSubjectJustJumped( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            return m_camSubjectList[index].m_subjectPerform.m_justJumped;
        }

        return bfalse;
    }

    void CameraControllerManager::setSubjectJustBounceJumped( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            m_camSubjectList[index].m_subjectPerform.m_justBounceJumped = btrue;
        }
    }

    bbool CameraControllerManager::isSubjectJustBounceJumped( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            return m_camSubjectList[index].m_subjectPerform.m_justBounceJumped;
        }

        return bfalse;
    }

    SubjectPerform CameraControllerManager::getSubjectPerform( ObjectRef _subject )
    {
        SubjectPerform perform;

        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            perform = m_camSubjectList[index].m_subjectPerform;
        }

        return perform;
    }

    void CameraControllerManager::setSubjectJustReceivedPunchUp( ObjectRef _subject )
    {
        i32 index = getSubjectIndex( _subject );
        if ( index >= 0 )
        {
            m_camSubjectList[index].m_subjectPerform.m_justReceivedPunchUp = btrue;
        }
    }

    bbool CameraControllerManager::hasLeadCameraSubject() const
    {
        const u32 subjectCount = m_camSubjectList.size();
        for (u32 i=0; i<subjectCount; ++i)
        {
            if (m_camSubjectList[i].m_leadCamera)
            {
                return btrue;
            }
        }
        return bfalse;
    }

    //////////////////////////////////////////////////////////////////////////
    // Cheat
    //////////////////////////////////////////////////////////////////////////
    void CameraControllerManager::backToGame()
    {
        resetCheatZoom();
        m_isCheatDezoomMax = bfalse;

#ifdef ITF_SUPPORT_DEBUGFEATURE
        m_debugDrawInfo = bfalse;
        m_debugDrawModifiers = bfalse;
#endif //ITF_SUPPORT_DEBUGFEATURE
    }

    void CameraControllerManager::setCheatZoom(f32 _increment)
    {
        m_cheatZoom += _increment;
    }

    void CameraControllerManager::resetCheatZoom()
    {
        m_cheatZoom = 0.0f;
    }

    void CameraControllerManager::switchCheatDezoomMaxWithPad()
    { 
        static f64 timeLast = 0.f;

        f64 thisTime = SYSTEM_ADAPTER->getTime();
        f64 deltaTime = thisTime -timeLast;
        timeLast = thisTime;

        if (deltaTime > 0.3f)
            return;

        switchCheatDezoomMax();
    }

    void CameraControllerManager::switchCheatDezoomMax()
    { 
        m_isCheatDezoomMax = !m_isCheatDezoomMax; 
        resetCheatZoom();
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void CameraControllerManager::drawDebug()
    {
        if ( m_debugDrawModifiers )
        {
            //  cameras modifiers components
            const u32 camModifiersCount = m_camModifierList_Active.size();
            for(u32 i=0; i<camModifiersCount; i++)
            {
                m_camModifierList_Active[i]->drawDebug();
            }
        }

        if ( m_debugDrawInfo )
        {
            //  cameras components
            const u32 camControllerCount = m_camControllerList.size();
            for(u32 i=0; i<camControllerCount; i++)
            {
                const CameraControllerData& data = m_camControllerList[i];
                if ( data.m_controller )
                {
                    data.m_controller->drawDebug(data);
                }
            }
            GFX_ADAPTER->drawDBGAABB( m_aabb, Color::orange());
        }

        if ( isDebugDrawScreen4By3() && CAMERA->getMode() == Camera::Camera_InGame )
        {           
            f32 height = (f32)GFX_ADAPTER->getScreenHeight();
            f32 deltaScreenBy2 = ( (f32)GFX_ADAPTER->getScreenWidth() - height *1.333333f ) *0.5f;

            if ( deltaScreenBy2 > 0.f )
            {
                u32 color;

                switch ( m_debugDrawScreen4By3 )
                {
                case Transparent:
                    color = ITFCOLOR_FROM_ARGB(2130706432u);
                    break;

                case Opaque:
                    color = ITFCOLOR_FROM_ARGB(4278190080u);
                    break;

                default:
                    color = ITFCOLOR_FROM_ARGB(2130706432u);
                    break;
                }

                const AABB& screenAABB = GFX_ADAPTER->getScreenAABB();
                GFX_ADAPTER->draw2dBox(screenAABB.getMin(), deltaScreenBy2, height, color, color, color, color, 0);
                GFX_ADAPTER->draw2dBox(screenAABB.getMax(), -deltaScreenBy2, -height, color, color, color, color, 0);
            }
        }
        if (m_shakeDebugDraw)
        {
            // camera shake curves
            if (m_shakeCountdown>0.0f)
            {
                DEBUG_DRAW_CURVE_OFS(m_shake.getShakeX(), getMainCameraController()->getActorRef(), -3.0f, 0.0f, 0.0f);
                DEBUG_DRAW_CURVE_OFS(m_shake.getShakeY(), getMainCameraController()->getActorRef(), 0.0f, 0.0f, 0.0f);
                DEBUG_DRAW_CURVE_OFS(m_shake.getShakeZ(), getMainCameraController()->getActorRef(), 3.0f, 0.0f, 0.0f);
                f32 t = (m_shake.getDuration() - m_shakeCountdown);
                m_shake.getShakeX().getValue(t);
                m_shake.getShakeY().getValue(t);
                m_shake.getShakeZ().getValue(t);
            }
        }       
    }
#endif //ITF_SUPPORT_DEBUGFEATURE

    void CameraControllerManager::onSwitchToWorld( World* _pWorld, bbool _bSwitchDone )
    {
        if(_pWorld && _bSwitchDone)
        {
            m_updateCamSubjectList_Active = btrue;
            m_updateCamModifierList_Active = btrue;

            setMainCameraControllerActive( 1.f, 1.f);
        }
    }

    bbool CameraControllerManager::isCameraControllerActive( ICameraController* _controller)
    {
        if (_controller->isAlwaysActive())
            return btrue;

        if(Actor* pActor = static_cast<Actor*>(_controller->getActorRef().getObject()))
        {
            Scene* scene = pActor->getScene();

            if( scene && scene->isActive())
                return btrue;
        }

        return bfalse;
    }

    void CameraControllerManager::updateCameraModifierComponentActiveList()
    {
        if ( !m_updateCamModifierList_Active)
            return;

        m_updateCamModifierList_Active = bfalse;
        m_camModifierList_Active.clear();

        for(u32 i= 0; i < m_camModifierList.size(); ++i)
        {
            CameraModifierComponent* cmc = m_camModifierList[i];

            if(Actor* pActor = cmc->GetActor())
            {
                if(pActor->getScene()->isActive())
                    m_camModifierList_Active.push_back(cmc);
            }
        }
    }

    void CameraControllerManager::updateCameraControllerSubjectActiveList()
    {
        if ( !m_updateCamSubjectList_Active)
            return;

        m_updateCamSubjectList_Active = bfalse;
        m_updateCamControllers_SubjectList = btrue;
        m_camSubjectList_Active.clear();

        for(u32 i= 0; i < m_camSubjectList.size(); ++i)
        {
            const CameraControllerSubject& ccs = m_camSubjectList[i];

            if (Actor* pActor = static_cast<Actor*>(ccs.m_ref.getObject()))
            {
                if ( ccs.m_isPlayer )
                {
                    m_camSubjectList_Active.push_back(ccs);
                }
                else
                {
                    Scene* scene = pActor->getScene();
                    if (scene && scene->isActive())
                    {
                        m_camSubjectList_Active.push_back(ccs);
                    }
                }
            }
        }
    }

    // Project screen AABB at given z.
    // Doesn't make sense if z is behind cam (returns false)
    bbool CameraControllerManager::getAABB( const f32 _z, AABB& _aabb ) const
    {
        f32 depth = m_camPosInGame.m_z - _z;
        if (depth > 0)
        {
            Vec2d screenRealLongBy2;
            screenRealLongBy2.m_y = f32_Abs( tanf(m_camFocaleInGame * 0.5f) * depth );
            screenRealLongBy2.m_x = screenRealLongBy2.m_y * m_screenRatio;

            const Vec2d& camPos = m_camPosInGame.truncateTo2D();
            _aabb.setMin( camPos - screenRealLongBy2 );
            _aabb.setMax( camPos + screenRealLongBy2 );

            return btrue;
        }

        return bfalse;
    }

    void CameraControllerManager::predictViewAABB( AABB& _aabb, const Vec2d& _pos, f32 _depth, f32 _focale ) const
    {
        Vec2d screenRealLongBy2;
        screenRealLongBy2.m_y = f32_Abs( tanf(_focale * 0.5f) * _depth );
        screenRealLongBy2.m_x = screenRealLongBy2.m_y * m_screenRatio;

        _aabb.setMin( _pos - screenRealLongBy2 );
        _aabb.setMax( _pos + screenRealLongBy2 );
    }

    void CameraControllerManager::processEventRegisterSubject( EventRegisterCameraSubject* eventRegister )
    {
        if ( eventRegister->getActionActivator() != EventRegisterCameraSubject::ACTION_NONE )
        {
            Actor* subject = Actor::SafeDynamicCast<Actor>(eventRegister->getActivator().getObject(),ITF_GET_STRINGID_CRC(Actor,2546623115));
            ITF_ASSERT(subject);
            if (subject)
            {
                if ( eventRegister->getActionActivator() == EventRegisterCameraSubject::ACTION_REGISTER )
                {
                    registerSubject(subject->getRef(), eventRegister->getDelay(), bfalse, bfalse, btrue);
                }
                else
                {
                    unregisterSubject(subject->getRef());
                }
            }
        }

        if ( eventRegister->getActionChildren() != EventRegisterCameraSubject::ACTION_NONE )
        {
            if ( Actor* sender = Actor::SafeDynamicCast<Actor>(eventRegister->getSender().getObject(),ITF_GET_STRINGID_CRC(Actor,2546623115)) )
            {
                if ( LinkComponent* linkComponent = sender->GetComponent<LinkComponent>() )
                {
                    AIUtils::LinkIterator iterator(linkComponent);

                    if ( eventRegister->getActionChildren() == EventRegisterCameraSubject::ACTION_REGISTER )
                    {
                        while ( Actor* subject = iterator.getNextRelativeActor() )
                        {
                            registerSubject(subject->getRef(), eventRegister->getDelay(), bfalse, bfalse, btrue);
                        }
                    }
                    else
                    {
                        while ( Actor* subject = iterator.getNextRelativeActor() )
                        {
                            unregisterSubject(subject->getRef());
                        }
                    }
                }
            }
        }
    }

    void CameraControllerManager::unregisterNPCFromCamera()
    {
        // clear all subjects but the players (apparently they don't re-register on checkpoint...)

        for (u32 i = 0; i < m_camSubjectList.size(); )
        {
            if (m_camSubjectList[i].m_isPlayer)
            {
                i++;
                continue;
            }

            unregisterSubject(m_camSubjectList[i].m_ref);
        }

        m_bindedObjects.clear();
    }

    // Camera sequence
    void CameraControllerManager::startCameraSequence( const Vec3d& _pos, f32 _blendIn, f32 _depth, bbool _unregisterPlayer, f32 _focale )
    {        
        if ( isCameraSequenceActive() )
        {
            ITF_ASSERT_MSG( 0, "camera sequence was already activated");
            return;
        }

        m_cameraSequence.m_pos = _pos;
        m_cameraSequence.m_depth = _depth;
        m_rampUpCoeff = _blendIn;
        m_rampUpDestinationCoeff = _blendIn;        
        m_cameraSequence.m_focale = _focale;

        m_cameraSequence.m_finalWeight = 1.f;
        m_cameraSequence.m_destinationWeight = 0.f;
        m_cameraSequence.m_weight = 0.f;

        for (u32 i = 0; i < m_camControllerList.size(); i++)
        {
            m_camControllerList[i].m_finalWeight = 0.f;
        }

        // unregister subjects
        if( _unregisterPlayer)
        {
            m_cameraSequence.m_camSubjectList = m_camSubjectList;
            m_camSubjectList.clear();
            m_updateCamSubjectList_Active = btrue;
        }
    }

    void CameraControllerManager::setCameraSequencePosition ( const Vec3d& _pos, f32 _depth )
    { 
        ITF_ASSERT_MSG( isCameraSequenceActive(), "camera sequence isn't activated");

        m_cameraSequence.m_pos = _pos; 

        if ( _depth!=F32_INFINITY)
            m_cameraSequence.m_depth = _depth;
    } 

    void CameraControllerManager::stopCameraSequence( f32 _blendOut )
    {
        if ( !isCameraSequenceActive() )
            return;

        m_rampUpCoeff = _blendOut;
        m_rampUpDestinationCoeff = _blendOut;        

        m_cameraSequence.m_finalWeight = 0.f;

        // register subjects
        if ( m_cameraSequence.m_camSubjectList.size())
        {
            m_camSubjectList = m_cameraSequence.m_camSubjectList;
            m_cameraSequence.m_camSubjectList.clear();

            m_updateCamSubjectList_Active = btrue;

            CameraSequenceTeleport();
        }

        setMainCameraControllerActive( _blendOut, _blendOut );
    }

    void CameraControllerManager::CameraSequenceTeleport()
    {
        Vec3d posTeleport = m_cameraSequence.m_pos -Vec3d::Front*m_depth;

        // notify the controllers
        const u32 camControllerCount = m_camControllerList.size();   
        for (u32 i = 0; i < camControllerCount; i++)
        {
            CameraControllerData& data = m_camControllerList[i];
            if (data.m_controller)
                data.m_controller->teleport(posTeleport);
        }

        m_controllerJustTeleported = btrue;
    }

    void CameraControllerManager::updateCameraSequenceWeight()
    {
        m_cameraSequence.m_destinationWeight += (m_cameraSequence.m_finalWeight - m_cameraSequence.m_destinationWeight) * m_rampUpDestinationCoeff;
        m_cameraSequence.m_weight += (m_cameraSequence.m_destinationWeight - m_cameraSequence.m_weight) * m_rampUpCoeff;
    }

    const CameraModifierComponent* CameraControllerManager::getCameraModifierComponent( const AABB& _aabb, f32 _depth ) const
    {
        i32 priorityRef = I32_MIN;
        CameraModifierComponent* camModRef = NULL;       

        for ( u32 i=0; i<m_camModifierList_Active.size(); i++ )
        {      
            CameraModifierComponent* camMod = m_camModifierList_Active[i];

            i32 priority = camMod->getCM().m_cameraModifierPriority;

            if ( priority >= priorityRef )
            {               
                if( _aabb.checkOverlap( camMod->getModifierAABBMax() ) )
                {
                    if ( priority == priorityRef && camModRef )
                    {
                        f32 deltaZRef = f32_Abs( camModRef->GetActor()->getDepth() -_depth);
                        f32 deltaZ = f32_Abs( camMod->GetActor()->getDepth() -_depth);

                        if ( deltaZ< deltaZRef)                
                            camModRef = camMod;
                    }
                    else
                    {
                        priorityRef = priority;                     
                        camModRef = camMod;
                    }                   
                }
            }
        }

        return camModRef;
    }

    bbool CameraControllerManager::getMapDirection( Vec2d& _mapDir, const Vec3d& _pos )
    {
        updateCameraModifierComponentActiveList();

        const AABB posAABB(_pos.truncateTo2D());
        const CameraModifierComponent* camModRef = getCameraModifierComponent( posAABB, _pos.m_z ); 

        if ( !camModRef )
            return bfalse;

        _mapDir = camModRef->getUpdateData().getDirection();
        ITF_ASSERT(_mapDir.isNormalized());

        return btrue;
    }

    //*****************************************************************************************************************
    void CameraControllerManager::bindObject( Pickable * _pickable, bbool _updateInitPos/*=bfalse*/ )
    {
        if ( _pickable == NULL )
        {
            return;
        }

        for( u32 i = 0; i < m_bindedObjects.size(); ++i )
        {
            if ( m_bindedObjects[i].m_objectRef == _pickable->getRef() )
            {
                return;
            }
        }
        BindedObject bindedObj;
        bindedObj.m_objectRef   = _pickable->getRef();
        bindedObj.m_updateInitPos = _updateInitPos;

        m_bindedObjects.push_back(bindedObj);
    }

    //*****************************************************************************************************************
    void CameraControllerManager::unbindObject( Pickable * _pickable )
    {
        if ( _pickable == NULL )
        {
            return;
        }

        for( u32 i = 0; i < m_bindedObjects.size(); ++i )
        {
            if ( m_bindedObjects[i].m_objectRef == _pickable->getRef() )
            {
                m_bindedObjects.eraseNoOrder( i );
                return;
            }
        }
    }

    //*****************************************************************************************************************
    bbool CameraControllerManager::isObjectBound( Pickable * _pickable ) const
    {
        if ( _pickable == NULL ) 
        {
            return bfalse;
        }

        for( u32 i = 0; i < m_bindedObjects.size(); ++i )
        {
            if ( m_bindedObjects[i].m_objectRef == _pickable->getRef() )
            {
                return btrue;
            }
        }
        return bfalse;
    }
} //namespace ITF
