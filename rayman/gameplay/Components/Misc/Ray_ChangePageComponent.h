#ifndef _ITF_RAY_CHANGEPAGECOMPONENT_H_
#define _ITF_RAY_CHANGEPAGECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

namespace ITF
{

    class Ray_ChangePageComponent : public ActorComponent, IUpdateAnimInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ChangePageComponent, ActorComponent,1321490025);
		DECLARE_VALIDATE_COMPONENT()

    public:
        DECLARE_SERIALIZE()

		Ray_ChangePageComponent();
		virtual ~Ray_ChangePageComponent();

        virtual void                onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

        virtual bbool               needsUpdate() const   { return btrue; }
        virtual bbool               needsDraw() const     { return bfalse; }
        virtual bbool               needsDraw2D() const   { return bfalse; }
#ifdef ITF_SUPPORT_EDITOR
		virtual void                drawEdit( ActorDrawEditInterface * _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR
        virtual void                onBecomeActive();
        virtual void                Update( f32 _dt );

		virtual void                onEvent( Event * _event);
        virtual void                updateAnimInput();

        // trigger function
        virtual Vec2d               getPlayerSpawnPos( u32 _playerIndex ) const;
        virtual bbool               getPlayerSpawnFlipped( u32 _playerIndex ) const { return ( ( m_finalDefaultPos - m_actor->get2DPos() ).dot(Vec2d::Right) < 0.0f ); }

        ITF_INLINE bbool            isWarpZone() const;
        ITF_INLINE bbool            isWarpZoneExit() const;
        ITF_INLINE f32              getWaitPlayersDuration() const;

        void                        setShut(bbool _isShut);
        bbool                       isCageMapDoor() const;

        enum ExitType
        {
            ExitType_Up,
            ExitType_Down,
            ExitType_Left,
            ExitType_Right,
            ExitType_None,
            ENUM_FORCE_SIZE_32( ExitType )
        };

        virtual ExitType		            ComputePoints(  Vec2d const & _enterDir,
                                                            Vec2d & _res_enterPoint,
                                                            Vec2d & _res_exitPoint,
                                                            Vec2d & _res_finalPoint,
                                                            bbool & _res_exitPointValid,
                                                            bbool & _res_finalPointValid,
                                                            bbool & _res_finalPointInWater,
                                                            bbool _enterPointOnly = bfalse ) const;

    protected:
        bbool                       isPointInWater( const Vec2d& _point ) const;
        Vec2d                       getEnterPoint( f32 _sign, const PhysShapeBox * _triggerShape, const Vec2d& _actorDir ) const;
        void						processChangePageEvent( Ray_EventChangePage * _evt );

    private:
        void                        processChangePageQueryPointsEvent( Ray_EventChangePageQueryPoints * _evt );
        void                        processEventTrigger( EventTrigger * _evt );

        void                        updatePageDoors( f32 _dt );
        void                        updateOpening( f32 _dt );
        void                        setOpeningFromOtherDoor( bbool _openState, f32 _openingTime );

		bbool						getPosOnGround( Vec2d & _res_foundPoint, Vec2d const & _startPoint, f32 const & _deltaDist ) const;

        Ray_ChangePageComponent*    getDestinationPageComponent() const;
        Ray_ChangePageComponent*    getStartPageComponent() const;

        Vec2d                       getExitPoint( f32 _sign, f32 _extent, const Vec2d& _actorDir ) const;
        Vec2d                       getFinalPoint( f32 _sign, const PhysShapeBox * _triggerShape, const Vec2d& _actorDir ) const;
        Vec2d                       getActorScale() const;
        ActorRef                    getActivateCheckpoint() const;

        bbool                       isRefDoor( ObjectRef _ref ) const;
        void                        closePageDoors();
        void                        updateElectoonSound(f32 _dt);

        void                        sendCloseCageDoor();



		class ShapeDetectorComponent*	m_shapeDetectorComponent;
        class LinkComponent *           m_linkComponent;
        class AnimatedComponent*        m_animatedComponent;
        class FXControllerComponent*    m_fxControllerComponent;

		ExitType                    m_computedExitType;
		Vec2d                       m_computedEnterPoint;
		Vec2d                       m_computedExitPoint;
		Vec2d                       m_computedFinalPoint;

		bbool                       m_computedExitPointValid;
		bbool                       m_computedFinalPointValid;
        bbool                       m_computedFinalPointInWater;

        bbool	                    m_warpInitialized;

        // For the warp doors
        f32                         m_openingTime;
        f32                         m_otherDoorTime;
        f32                         m_enableDelayCounter;
        u32                         m_openingFrameCheck;
        bbool                       m_opening;

        // For the page doors
        bbool                       m_pageDoorsOpened;

        //
        bbool                       m_isShut; //When set, don't react to change page events

        // Electoon sound
        u32                         m_currentElectoonSoundHandle;
        f32                         m_remainingTimeToRestartElectoonSound;

        ITF_INLINE const class Ray_ChangePageComponent_Template*  getTemplate() const;

        // loaded/saved data
        Vec2d   m_finalDefaultPos;
    };

    //---------------------------------------------------------------------------------------------------

    class Ray_ChangePageComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ChangePageComponent_Template, TemplateActorComponent,4146314714);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ChangePageComponent);

    public:

        Ray_ChangePageComponent_Template();
        ~Ray_ChangePageComponent_Template() {}

        ITF_INLINE f32		        getWaitPlayersDuration()        const { return m_waitPlayerDuration; }
        ITF_INLINE bbool		    getUseFadeOutIn()               const { return m_useFadeOutIn; }
        ITF_INLINE bbool		    isWarpZone()                    const { return m_warpZone; }
        ITF_INLINE bbool		    isWarpZoneExit()                const { return m_warpZoneExit; }
        ITF_INLINE const Vec2d &	getWarpZoneExitScaleFactor()    const { return m_warpZoneExitScaleFactor; }
        ITF_INLINE f32              getWarpZoneExitArrivalFadeOutDuration() const { return m_warpZoneExitArrivalFadeOutDuration; }
        ITF_INLINE f32              getOpeningRadius()              const { return m_openingRadius; }
        ITF_INLINE f32              getOpeningTime()                const { return m_openingTime; }
        ITF_INLINE f32              getEnableDelay()                const { return m_enableDelay; }

        ITF_INLINE f32				getExitUpSideOffset()	const { return m_exitUpSideOffset; }
        ITF_INLINE bbool			isExitUpSideDefined()	const { return ( m_exitUpLeftSide >= 0 ? btrue: bfalse); }
        ITF_INLINE bbool			isExitUpLeftSide()		const { return ( m_exitUpLeftSide == 1 ? btrue : bfalse); }
        ITF_INLINE f32				getExitDownSideOffset()	const { return m_exitDownSideOffset; }
        ITF_INLINE bbool			isExitDownSideDefined()	const { return ( m_exitDownLeftSide >= 0 ? btrue: bfalse); }
        ITF_INLINE bbool			isExitDownLeftSide()	const { return ( m_exitDownLeftSide == 1 ? btrue : bfalse); }
        ITF_INLINE f32				getExitLeftSideOffset()	const { return m_exitLeftSideOffset; }
        ITF_INLINE f32				getExitRightSideOffset()const { return m_exitRightSideOffset; }
        ITF_INLINE bbool            isOnWay()               const { return m_oneWay || isWarpZoneExit(); }
        ITF_INLINE bbool            isCageMapDoor()         const { return m_isCageMapDoor; }
        ITF_INLINE f32              getElectoonHelpPeriod() const { return m_electoonHelpPeriod;}

    private:
		f32         m_waitPlayerDuration;
        bbool       m_useFadeOutIn;
        bbool       m_warpZone;
        bbool       m_warpZoneExit;
        Vec2d       m_warpZoneExitScaleFactor;
        f32         m_warpZoneExitArrivalFadeOutDuration;
        f32         m_openingRadius;
        f32         m_openingTime;
        f32         m_enableDelay;
        f32		    m_exitUpSideOffset;
        f32		    m_exitDownSideOffset;
        i32		    m_exitUpLeftSide;
        i32		    m_exitDownLeftSide;
        f32		    m_exitLeftSideOffset;
        f32		    m_exitRightSideOffset;
        bbool	    m_oneWay;
        bbool       m_isCageMapDoor;
        f32         m_electoonHelpPeriod;
    };


    const Ray_ChangePageComponent_Template*     Ray_ChangePageComponent::getTemplate() const {return static_cast<const Ray_ChangePageComponent_Template*>(m_template);}
    ITF_INLINE bbool                            Ray_ChangePageComponent::isWarpZone() const  {return ( getTemplate()->isWarpZone() || getTemplate()->isWarpZoneExit() ); }
    ITF_INLINE bbool                            Ray_ChangePageComponent::isWarpZoneExit() const  {return getTemplate()->isWarpZoneExit(); }
    ITF_INLINE f32                              Ray_ChangePageComponent::getWaitPlayersDuration() const  {return getTemplate()->getWaitPlayersDuration(); }
}

#endif // _ITF_RAY_CHANGEPAGECOMPONENT_H_

