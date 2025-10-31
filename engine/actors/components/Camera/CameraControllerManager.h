#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#define _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef _ITF_SAFEARRAY_H_
#include "core/container/SafeArray.h"
#endif //_ITF_SAFEARRAY_H_

#ifndef _ITF_ICAMERACONTROLLER_H_
#include "engine/display/ICameraController.h"
#endif //_ITF_ICAMERACONTROLLER_H_

#ifndef _ITF_CAMERAMODIFIERCOMPONENT_H_
#include "engine/actors/components/Camera/CameraModifierComponent.h"
#endif //_ITF_CAMERAMODIFIERCOMPONENT_H_

#ifndef _ITF_WORLD_EVENT_INTERFACE_H_
#include "engine/scene/worldEventInterface.h"
#endif //_ITF_WORLD_EVENT_INTERFACE_H_

#ifndef _ITF_CAMERASHAKE_H_
#include "engine/actors/components/Camera/CameraShake.h"
#endif // _ITF_CAMERASHAKE_H_

namespace ITF
{
    #define CAMERACONTROLLERMANAGER CameraControllerManager::get()

    class CameraControllerData 
    {
    public:
        CameraControllerData()
            : m_controller(NULL)
            , m_finalWeight(0)
            , m_destinationWeight(0)
            , m_weight(0)
            , m_prevFocale(MTH_PIBY4)
        {}

        ICameraController*      m_controller;
        f32                     m_finalWeight;
        f32                     m_destinationWeight;
        f32                     m_weight;
        Vec3d                   m_prevPos;
        Vec3d                   m_prevSpeed;
        f32                     m_prevFocale;

        ITF_INLINE Vec3d        getPosition() const  { return m_prevPos; }
        ITF_INLINE f32          getFocale() const { return m_prevFocale; }
    };

    struct SubjectPerform
    {
        SubjectPerform()
            : m_justJumped(bfalse)
            , m_justBounceJumped(bfalse)
            , m_justReceivedPunchUp(bfalse)
        {}

        bbool       m_justJumped;
        bbool       m_justBounceJumped;
        bbool       m_justReceivedPunchUp;
        void        reset() { m_justJumped=bfalse; m_justBounceJumped=bfalse; m_justReceivedPunchUp=bfalse; }
    };

    class CameraControllerSubject 
    {
    public:
        CameraControllerSubject()
            : m_delayToRegister(0.f)
            , m_isPlayer(btrue)
            , m_leadCamera(bfalse)
        {

        }
        ObjectRef   m_ref;
        ObjectRef   m_refOld;
        f32         m_delayToRegister;
        bbool       m_isPlayer;
        bbool       m_leadCamera;
        SubjectPerform m_subjectPerform;
    };

    class CameraControllerManager : public IEventListener, public Interface_WorldEventsListener
    {
    protected:
        static CameraControllerManager*    s_instance;
    public:

        static void                     create()    { s_instance = newAlloc(mId_Gameplay, CameraControllerManager()); }
        static void                     destroy()   { SF_DEL(s_instance); }
        static CameraControllerManager* get()       { return s_instance; }  //ITF_ASSERT_MSG(s_instance, "CameraControllerManager not ready, be sure to call CameraControllerManager::create() !");

        CameraControllerManager();
        virtual ~CameraControllerManager();

        void                    init();

        virtual void    onSwitchToWorld(World* _pWorld, bbool _bSwitchDone);// inherited from Interface_WorldEventsListener
        virtual void    onDeleteWorld(World* _pWorld){};// inherited from Interface_WorldEventsListener
        virtual void    onStartDrawProcess(){}; // inherited from Interface_WorldEventsListener
        virtual void    onEndDrawProcess(){}; // inherited from Interface_WorldEventsListener
        virtual void    onStartUpdateProcess(f32 /*_dt*/) {}; // inherited from Interface_WorldEventsListener
        virtual void    onEndUpdateProcess(f32 /*_dt*/); // inherited from Interface_WorldEventsListener
        virtual void    onUpdateProcess(f32 /*_dt*/){}; // inherited from Interface_WorldEventsListener

        bbool   m_controllerJustTeleported;
        f32     m_screenRatio;

        const AABB&     getAABB() const { return m_aabb; }
        bbool           getAABB( const f32 _z, AABB& _aabb ) const;
        const Vec3d&    getCamPos() const { return m_camPosInGame; }
        const Vec3d&    getCamSpeed() const { return m_camSpeedInGame; }
        void            setCamPos(Vec3d& _pos) { m_camPosInGame = _pos; }
        bbool           updateCamControllers_SubjectList() { return m_updateCamControllers_SubjectList;}
        f32             getDepth() const { return m_depth; }
        void            setDefaultDepth(f32 _depth) { m_defaultDepth = _depth; }
        f32             getDefaultDepth() const { return m_defaultDepth; }
        void            setCameraBlend(f32 _rampUpDestinationCoeff, f32 _rampUpCoeff ) { m_rampUpDestinationCoeff = _rampUpDestinationCoeff; m_rampUpCoeff = _rampUpCoeff;}
        void            predictViewAABB( AABB& _aabb, const Vec2d& _pos, f32 _depth = 23.f, f32 _focale = MTH_PIBY4 ) const;

    private:

        class CameraSequenceData 
        {
        public:
            CameraSequenceData()
                : m_finalWeight(0)
                , m_destinationWeight(0)
                , m_weight(0)
                , m_pos(Vec3d::Zero)
                , m_focale(MTH_PIBY4)
                , m_depth(13.f)
            {}

            f32      m_finalWeight;
            f32      m_destinationWeight;
            f32      m_weight;
            Vec3d    m_pos;
            f32      m_focale;
            f32      m_depth;

            SafeArray<CameraControllerSubject>  m_camSubjectList;
        };

        Vec3d       m_camPosInGame;
        Vec3d       m_camSpeedInGame;
        f32         m_camFocaleInGame;        
        f32         m_depth;
        AABB        m_aabb;
        bbool       m_isCheatDezoomMax;
        f32         m_cheatZoom;      
        f32         m_defaultDepth;

        // Mixer
        typedef SafeArray<ICameraController*> MainCamStack;
        MainCamStack            m_mainCamStack;
        f32                     m_rampUpDestinationCoeff;
        f32                     m_rampUpCoeff;

        CameraSequenceData                     m_cameraSequence;
        SafeArray<CameraControllerData>     m_camControllerList;
        SafeArray<CameraModifierComponent*> m_camModifierList, m_camModifierList_Active;
        SafeArray<CameraControllerSubject>  m_camSubjectList, m_camSubjectList_Active;

        bbool       m_updateCamModifierList_Active;
        bbool       m_updateCamSubjectList_Active;
        bbool       m_updateCamControllers_SubjectList;

        void        updateCameraControllerSubjectActiveList();
        void        updateCameraModifierComponentActiveList();
        bbool       isCameraControllerActive( ICameraController* _controller);
        bbool       cameraControllerHasInfluence( const CameraControllerData& data ) const { return data.m_weight > MTH_EPSILON; } 

        void        updateScreenRatio();

#ifdef ITF_SUPPORT_DEBUGFEATURE
        void        drawDebug();
#endif //ITF_SUPPORT_DEBUGFEATURE
 
    public:
        bbool       isCamModifierComponentActive( CameraModifierComponent* _camModifier );
        void        registerCamModifierComponent ( CameraModifierComponent* _camModifier );
        void        unregisterCamModifierComponent( CameraModifierComponent* _camModifier );
        void        registerCameraController ( ICameraController* _cam );
        void        unregisterCameraController ( ICameraController* _cam );
        void        setMainCameraController ( ICameraController* _mainCam, f32 _rampUpDestinationCoeff, f32 _rampUpCoeff );
        void        removeMainCameraController ( ICameraController* _mainCam, f32 _rampDownDestinationCoeff, f32 _rampDownCoeff );
        void        setMainCameraControllerActive( f32 _destinationCoeff, f32 _coeff );
        ICameraController* getMainCameraController() ;
        bbool       isMainCameraController( const ICameraController* _cam );
        bbool       cameraControllerHasInfluence( const ICameraController* _cam ) const;

        void        startCameraSequence ( const Vec3d& _pos, f32 _blendIn = 0.05f, f32 _depth = 13.f, bbool _unregisterPlayer = bfalse, f32 _focale = MTH_PIBY4 );
        void        stopCameraSequence ( f32 _blendOut = 0.05f );
        void        setCameraSequencePosition ( const Vec3d& _pos, f32 _depth = F32_INFINITY );
        bbool       isCameraSequenceFinished() const { return !isCameraSequenceActive() && m_cameraSequence.m_weight < MTH_EPSILON; }
           
        // cheat
        void        setCheatZoom(f32 _increment);
        void        resetCheatZoom();
        f32         getCheatZoom() const { return m_cheatZoom; }
        void        switchCheatDezoomMaxWithPad();
        void        switchCheatDezoomMax();
        bbool       isCheatDezoomMax() const { return m_isCheatDezoomMax; }

        void        removeInvalidSubjects    (   ); 
        void        registerSubject         (   ObjectRef _subject, f32 _delayToRegister = 0.f, bbool _isPlayer = btrue, bbool _doTeleport = btrue, bbool _leadCamera = bfalse );
        void        unregisterSubject       (   ObjectRef _subject);
        void        swapSubjects           (   ObjectRef _subjectRefFrom, ObjectRef _subjectRefTo );
        void        unregisterNPCFromCamera();
        void        unregisterAllSubjects   (   );
        bbool       isSubjectRegistered     (   ObjectRef _subject ) const { return getSubjectIndex(_subject ) != -1; }
        u32         getSubjectListCount     (   );
        u32         getSubjectPlayerListCount(  );
        void        getSubjectByIndex       (   u32 _index, CameraControllerSubject& _subject);
        i32         getSubjectIndex         (   ObjectRef _subject ) const;
        void        setSubjectJustJumped    (   ObjectRef _subject );
        bbool       isSubjectJustJumped     (   ObjectRef _subject  );
        void        setSubjectJustBounceJumped    (   ObjectRef _subject );
        void        setSubjectJustReceivedPunchUp  (   ObjectRef _subject );
        bbool       isSubjectJustBounceJumped     (   ObjectRef _subject  );
        SubjectPerform getSubjectPerform( ObjectRef _subject );
        bbool       hasLeadCameraSubject() const;
        void        teleportCameraControllers(   );
        void        onEvent( Event* _event );   // IEventListener
        void        onMapLoaded();
        f32         getScreenRatio          (   ) { return m_screenRatio; }
        void        backToGame();

        void                        bindObject( Pickable * _pickable, bbool _updateInitPos = bfalse );
        void                        unbindObject( Pickable * _pickable );
        bbool                       isObjectBound( Pickable * _pickable ) const;
        ITF_INLINE const Vec3d &    getBindCameraMoveDelta() const { return m_bindCameraMoveDelta; }


        const SafeArray<CameraControllerData>&      getCameraControllerList() const { return m_camControllerList; }
        const SafeArray<CameraModifierComponent*>&  getCameraModifierList() const { return m_camModifierList_Active; }
        const SafeArray<CameraControllerSubject>&   getCameraSubjectList() const { return m_camSubjectList_Active; }
        const CameraModifierComponent*              getCameraModifierComponent( const AABB& _aabb, f32 _depth) const;
        bbool                                       getMapDirection( Vec2d& _mapDir, const Vec3d& _pos );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// request position change; request is dispatched to controllers, or applied on next update()
        /// @param _pos the new position
        void        teleport                (   const Vec3d& _pos);


        ///////////////////////////////////////////////////////////////////////////////////////////
        /// add shake movement
        ///@param _name shake definition to use (cf GameConfig/camerashakeconfig.isg)
        void addShake(const StringID& _name);
        /// reset shake movement
        void resetShake() { m_shake = CameraShake(); m_shakeCountdown = 0.0f; }

#ifdef ITF_SUPPORT_DEBUGFEATURE
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to debug info flag
        bbool       m_debugDrawInfo;
        bbool       isDebugDrawInfo             (   ){ return m_debugDrawInfo; }
        void        switchDebugDrawInfo         (   ){ m_debugDrawInfo = !m_debugDrawInfo; }  
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to show 4/3 screen flag
        enum DebugDrawScreen4By3Type
        {
            None,
            Opaque,
            Transparent,
            Count
        };
        DebugDrawScreen4By3Type                     m_debugDrawScreen4By3;
        bbool       isDebugDrawScreen4By3           (   ){ return m_debugDrawScreen4By3 != None; }
        void        setDebugDrawScreen4By3          (   DebugDrawScreen4By3Type _type ) { m_debugDrawScreen4By3 = _type; } 
        const DebugDrawScreen4By3Type& getDebugDrawScreen4By3() const { return m_debugDrawScreen4By3; }            
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to shake debug draw flag
        bbool       m_shakeDebugDraw;
        bbool       isShakeDebugDraw        (   ){ return m_shakeDebugDraw; }
        void        switchShakeDebugDraw    (   ){ m_shakeDebugDraw = !m_shakeDebugDraw; }  
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// accessor to modifiers debugdraw flag
        bbool       m_debugDrawModifiers;
        bbool       isDebugDrawModifiers             (   ){ return m_debugDrawModifiers; }
        void        switchDebugDrawModifiers         (   ){ m_debugDrawModifiers = !m_debugDrawModifiers; } 
#endif // ITF_SUPPORT_DEBUGFEATURE

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Pause all cameras (eg for gameplay purpose). This is different from global pause
        void        pauseMode           ( bbool _pause) {m_pauseMode = _pause;}        
        bbool       isPauseMode         (             ) { return m_pauseMode;}        
        
        void        setIgnoreShakeCamFromCageBehaviour( bbool _ignoreShake ) { m_ignoreShakeFromCageBehaviour = _ignoreShake; }

    private:
        void processEventRegisterSubject( class EventRegisterCameraSubject* eventRegister );
        
        void        updateCameraSequenceWeight();
        bbool       isCameraSequenceActive() const { return m_cameraSequence.m_finalWeight == 1.f; }
        void        CameraSequenceTeleport();
        void        setTeleported();
        bbool m_pauseMode;

        // shake
        bbool m_ignoreShake;
        bbool m_ignoreShakeFromCageBehaviour;

        f32 m_shakeCountdown;
        CameraShake m_shake;
        const CameraShakeConfig_Template* m_shakeConfig;

        struct BindedObject
        {
            ObjectRef   m_objectRef;
            bbool       m_updateInitPos;
        };

        SafeArray<BindedObject>     m_bindedObjects;
        Vec3d                       m_bindLastCameraPos;
        Vec3d                       m_bindCameraMoveDelta;
    };
}

#endif //_ITF_CAMERACONTROLLERMANAGER_H_
