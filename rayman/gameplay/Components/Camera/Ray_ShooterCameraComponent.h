#ifndef _ITF_RAY_SHOOTERCAMERACOMPONENT_H_
#define _ITF_RAY_SHOOTERCAMERACOMPONENT_H_

#ifndef _ITF_FIXEDCAMERACOMPONENT_H_
#include "engine/actors/components/Camera/FixedCameraComponent.h"
#endif //_ITF_FIXEDCAMERACOMPONENT_H_

namespace ITF 
{
#define SHOOTERCAMERA   Ray_ShooterCameraComponent::getInstance()
    
    const Vec2d k_ShooterCameraDefaultSpeed                 = Vec2d( 3.5f, 0.0 );
    const f32   k_ShooterCameraDefaultZOffset               = 15.f;
    const f32   k_ShooterCameraTransitionDefaultDuration    = 2.f;

    //------------------------------------------------------------------------------------------------
    //                                  SHOOTER CAM MODIFIERS CLASSES
    //------------------------------------------------------------------------------------------------
    //--------------------------------------------------
    class ShooterCameraModifier
    {
    public:
        DECLARE_SERIALIZE()

        ShooterCameraModifier() 
            : m_moveSpeed( k_ShooterCameraDefaultSpeed )
            , m_zOffset( k_ShooterCameraDefaultZOffset )
            , m_depth( 0.f )
        {
        }
        // instance params
        Vec2d   m_moveSpeed;
        f32     m_zOffset;
        // real time params
        f32     m_depth;
    };

    //--------------------------------------------------
    class ShooterCameraModifier_Transition
    {
    public:
        DECLARE_SERIALIZE()

        enum TransitionType
        {
            Linear = 0,
            AccelDecel,
            X2,
            InvX2,
            X3,
            InvX3,
            //InvExpo,
        };

        ShooterCameraModifier_Transition() 
            : m_type( Linear )
            , m_duration( k_ShooterCameraTransitionDefaultDuration )
            , m_timer( 0.f )
            , m_blendCursor( 1.f )
            , m_notifyPlayerDelay( 0.f )
            , m_curAdditionalSpeed(Vec2d::Zero)
            , m_additionalSpeed(Vec2d::Zero)
            , m_isStarted( bfalse )
            //, m_factor( 8.f )
        {
        }
        
        static void         computeBlendCursorFromType( f32 & _blendCursor,
                                                        const TransitionType & _type, 
                                                        const f32 & _timerCursor, 
                                                        const f32 & _dt );
        void                start( bbool _fromCurrentTime = bfalse );
        void                update( f32 _dt );
        ITF_INLINE  f32     getBlendCursor()    const           { return m_blendCursor; }
        ITF_INLINE  f32     getTimeFactor()     const           { return ( m_duration > 0.f ? 1.f - ( m_timer/m_duration ) : 1.f ); }
        ITF_INLINE  bbool   isFinished()        const           { return ( m_blendCursor > 0.9999f ); }
        ITF_INLINE  bbool   isStarted()        const            { return m_isStarted; }
        ITF_INLINE  const Vec2d & getCurAdditionalSpeed() const { return m_curAdditionalSpeed; }
       
        // params
        TransitionType      m_type;
        f32                 m_duration;
        Vec2d               m_additionalSpeed;
        f32                 m_notifyPlayerDelay;
        //f32                 m_factor;

    private :
        bbool               m_isStarted;
        f32                 m_timer;
        f32                 m_blendCursor;
        Vec2d               m_curAdditionalSpeed;
    };

    typedef ShooterCameraModifier_Transition::TransitionType ShooterCameraTransitionType;

    //------------------------------------------------------------------------------------------------
    //                                  SHOOTER CAMERA CLASS
    //------------------------------------------------------------------------------------------------
    class Ray_ShooterCameraComponent : public BaseCameraComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterCameraComponent, BaseCameraComponent,1772393706)

    public:
        DECLARE_SERIALIZE()

        Ray_ShooterCameraComponent();
        virtual ~Ray_ShooterCameraComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }
        virtual void        Update(f32 _dt); 

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event* _event );
        virtual Vec3d       getCameraPosition() const;
        virtual f32         getDepth() const;
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

        //void                shooterTeleport(const Vec3d& _newPos );
        void                shooterTeleportToCheckPoint( Actor * _checkPoint, bbool _isCameraActivator );
        
		void				dbgTeleport(const Vec3d& _pos);
        Vec2d               getSpeed() const;
		static Ray_ShooterCameraComponent* getInstance()		{ return m_instance; }
        
        bbool               getCurTransitionInfo( ShooterCameraModifier_Transition * _transition, f32 & _res_DestDepth );
        bbool               isInTransition( f32 & _res_DestDepth );

        void                registerModifier( const ShooterCameraModifier & _newModifier, const ShooterCameraModifier_Transition & _transition );

    private:
        ITF_INLINE const class Ray_ShooterCameraComponent_Template* getTemplate() const;
        void                reset();
        void                presaveCheckpoint();
        void                onCheckpointLoaded();

        void                updateCameraTransition(f32 _dt); 
        void                updateCameraFromModifier(f32 _dt); 
        
        void                startTransition();
        void                computeModifierFromCamera( ShooterCameraModifier & _modifier );
        void                sendTransitionNotifyEvent();

    private:
        static Ray_ShooterCameraComponent* m_instance;

    
        // modifiers management
        ShooterCameraModifier   m_fromModifier;
        ShooterCameraModifier   m_toModifier;
        ShooterCameraModifier   m_initModifier;
        f32                     m_modifierBlendCursor;
        
        // transitions
        ShooterCameraModifier_Transition m_curTransition;
        bbool                   m_transitionActive;
        Vec2d                   m_transitionAdditionalSpeed;
        f32                     m_transitionPlayerNotificationTimer;

        // internal data
        bbool                   m_activated;
        Vec2d                   m_curMoveSpeed;
        f32                     m_curZoffset;
        f32                     m_curDepth;

        // params / saved params
        Vec3d                   m_posAtCheckpoint;
        bbool                   m_useInitModifier;

        SafeArray<Vec3d>        m_debugTransitionPoints;
    };

    //-------------------------------------------------------------------------------------
    class Ray_ShooterCameraComponent_Template : public BaseCameraComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShooterCameraComponent_Template, BaseCameraComponent_Template ,4112142190);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShooterCameraComponent);

    public:

        Ray_ShooterCameraComponent_Template();
        ~Ray_ShooterCameraComponent_Template() {}

        ITF_INLINE const Vec2d &    getDefaultSpeed()           const { return m_defaultSpeed; }
        ITF_INLINE const f32 &      getDefaultZOffset()         const { return m_defaultZOffset; }

    private:
        Vec2d       m_defaultSpeed;
        f32         m_defaultZOffset;

    };

    ITF_INLINE const Ray_ShooterCameraComponent_Template* Ray_ShooterCameraComponent::getTemplate() const
    {
        return static_cast<const Ray_ShooterCameraComponent_Template*>(m_template);
    }

    ITF_INLINE f32 Ray_ShooterCameraComponent::getDepth() const { return m_curZoffset; }
}

#endif // _ITF_RAY_SHOOTERCAMERACOMPONENT_H_