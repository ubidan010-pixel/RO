
#ifndef _ITF_FLYINGCOMPONENT_H_
#define _ITF_FLYINGCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "../actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_
#include "gameplay/Components/common/StickToPolylinePhysComponent.h"
#endif //_ITF_STICKTOPOLYLINEPHYSCOMPONENT_H_

namespace ITF
{
	//******************************************************************
	// Flying component manage air movements.
	// - Take into account physic component
	//******************************************************************
	class FlyingComponent : public ActorComponent
	{
		DECLARE_OBJECT_CHILD_RTTI(FlyingComponent, ActorComponent,907001313);

	public :
		// Specific struct for debug
		class FlyingComponentDebugData
		{
			DECLARE_SERIALIZE()
			FlyingComponentDebugData() : m_SnapToWaypointId(""), m_Used(false), m_OnPlayer(false), m_CollideInvForce(0.0){}

			bbool		isValid()		 { return m_Used; }

			bbool		m_Used;
			StringID    m_SnapToWaypointId;
			bbool		m_OnPlayer;
			f32			m_CollideInvForce;
		};
	
	public:
		DECLARE_SERIALIZE()

		FlyingComponent();
		~FlyingComponent();

		virtual bbool           needsUpdate() const { return btrue; }
		virtual bbool           needsDraw() const { return bfalse; }
		virtual bbool           needsDraw2D() const { return bfalse; }
		virtual void            onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
		//virtual void            onCheckpointLoaded();

		virtual void            Update( f32 _deltaTime );
		virtual void            onEvent( Event * _event);
#ifdef ITF_SUPPORT_EDITOR
		virtual void			onEditorMove(bbool _modifyInitialPos = btrue);
#endif // ITF_SUPPORT_EDITOR

	protected:

		void					MoveTo( Vec2d _DestinationPos );
		void					MoveUpdate( f32 _deltaTime );
		void					MoveUpdate_WithPhysic(f32 _deltaTime );
		void					MoveUpdate_Normal( f32 _deltaTime );
		void					MoveDirCompute( f32 _CurAngle, f32 _deltaTime );
		void					MovePosCompute( Vec2d _MovePos );
		void					MoveModeChange( bbool _Flying );
		void					DirectionCompute( Vec2d _moveDir, Vec2d _contactNormal, f32 _deltaTime );

		void					FlyModeInitialize();
		void					FlyModeCompute( f32 _deltaTime );

		void					LandModeInitialize();
		void					LandModeCompute( f32 _deltaTime );

		void					EnvironmentEvaluation();
		Vec2d					getNearestEdgeExtremityFromPos( const PolyLineEdge& _edge, Vec2d _pos, f32 &_res_sqrNorm );

	protected:


        ITF_INLINE const class FlyingComponent_Template*  getTemplate() const;
		
		// Intenal vars
		StickToPolylinePhysComponent* m_physComponent;

		Vec2d		m_DirAvgAngle;
		Vec2d		m_DestinationPos;
		Vec2d		m_MoveDir;
		Vec2d		m_MoveVector;
		Vec2d		m_MoveAddForce;
		f32			m_MoveVecAngle;
		Vec2d		m_MoveAvgAngle;
		//Angle		m_MoveAngleTest;
		
		Vec2d		m_MoveCurPos;
		f32			m_MoveCurPosDist;
		bbool		m_FlyingMode;

		Vec2d		m_EnvContactPos;
		Vec2d		m_EnvContactNormal;
		u32			m_EnvContactEdgeIdx;
		PolyLine*	m_EnvContactPolyline;
		bbool		m_EnvContactValid;
		bbool		m_EnvContactNextEdge;
		Vec2d		m_EnvContactNextEdgePos;

		Vec2d		m_FlyInterPos;
		bbool		m_FlyInterPosValid;

		Vec2d		m_LandMoveDir;
		//bbool		m_FlyInterPosValid;
	};

    //---------------------------------------------------------------------------------------------------

    class FlyingComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(FlyingComponent_Template,TemplateActorComponent,2120704391);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(FlyingComponent);

    public:

        struct FlyingComponentDebugData
        {
            DECLARE_SERIALIZE()
            FlyingComponentDebugData() : m_SnapToWaypointId(""), m_Used(false), m_OnPlayer(false), m_CollideInvForce(0.0){}

            const bbool		isValid()	const	 { return m_Used; }

            bbool		m_Used;
            StringID    m_SnapToWaypointId;
            bbool		m_OnPlayer;
            f32			m_CollideInvForce;
        };

        FlyingComponent_Template();
        ~FlyingComponent_Template() {}

        f32                             getMoveVecBlendFactor() const { return m_MoveVecBlendFactor; }
        f32                             getMoveSpeed() const { return m_MoveSpeed; }
        f32                             getMoveDirMaxAngle() const { return m_MoveDirMaxAngle; }
        f32                             getMoveDirMinAngle() const { return m_MoveDirMinAngle; }
        f32                             getMoveDirAmplitude() const { return m_MoveDirAmplitude; }
        const FlyingComponentDebugData& getDebugData() const { return m_DebugData; }

    private:

        f32			m_MoveVecBlendFactor;
        f32			m_MoveSpeed;
        f32			m_MoveDirMaxAngle;
        f32			m_MoveDirMinAngle;
        f32			m_MoveDirAmplitude;
        FlyingComponentDebugData m_DebugData;
    };


    const FlyingComponent_Template*  FlyingComponent::getTemplate() const {return static_cast<const FlyingComponent_Template*>(m_template);}
}

#endif // _ITF_FLYINGCOMPONENT_H_

