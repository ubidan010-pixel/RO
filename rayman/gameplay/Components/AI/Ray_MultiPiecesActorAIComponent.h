#ifndef _ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_
#define _ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

namespace ITF
{
    //----------------------------------------
    class PieceData
    {
    public:
        DECLARE_SERIALIZE();
        PieceData() : m_detachOrder(-1), m_startBhvUnderDamage(-1) {}

        Path        m_actorPath;
        StringID    m_name;
        //StringID    m_boneName;
        //Vec2d       m_localPos;
        //f32         m_localAngle;
        //f32         m_depth;
        i32         m_detachOrder;
        i32         m_startBhvUnderDamage;
        StringID    m_behaviorName;
    };

    //----------------------------------------
    class Piece
    {
    public:
        Piece() : m_pieceData( NULL ), m_bhvStarted( bfalse ) {}
        ActorRef            m_actor;
        const PieceData *   m_pieceData;
        bbool               m_bhvStarted;
    };


    //-----------------------------------------------------------------------------------------
    class Ray_MultiPiecesActorAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MultiPiecesActorAIComponent,Ray_AIComponent,758641620)
        DECLARE_SERIALIZE();

    public:

        Ray_MultiPiecesActorAIComponent();
        virtual ~Ray_MultiPiecesActorAIComponent();

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onBecomeActive();
        virtual void        Update( f32 _dt );
        virtual void        onBehaviorFinished();
        virtual void        onEvent(class Event * _event);

    protected:
     
        virtual void        processHit( HitStim* _hit ) {}
        virtual void        onPieceHit( Actor * _piece );
        bbool               isPieceCanBeVacuum( const ActorRef & _pieceRef );

        ITF_INLINE const class Ray_MultiPiecesActorAIComponent_Template * getTemplate() const;

        bbool               isMyPiece( const ObjectRef & _objRef );
        Piece *             getPieceData( const ObjectRef & _objRef );
        void                checkPiecesCoherency();
        void                destroyPieces();

    protected:

        SafeArray<Piece>    m_piecesList;

        u32                 m_initPieceCount;
        u32                 m_curDetachOrder;
    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    //class TemplateAIBehavior;

    class Ray_MultiPiecesActorAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_MultiPiecesActorAIComponent_Template,Ray_AIComponent_Template,3527430123);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_MultiPiecesActorAIComponent);

    public:
        Ray_MultiPiecesActorAIComponent_Template();
        ~Ray_MultiPiecesActorAIComponent_Template();

        const PieceData *               getPieceData( const Path &  _actorPath ) const;
        const ITF_VECTOR<PieceData>&    getPieceDataList() const    { return m_piecesDataList; }

    private:
        ITF_VECTOR<PieceData>   m_piecesDataList;
    };

    ITF_INLINE const class Ray_MultiPiecesActorAIComponent_Template * Ray_MultiPiecesActorAIComponent::getTemplate() const
    {
        return static_cast<const class Ray_MultiPiecesActorAIComponent_Template *>(m_template);
    }
}

#endif //_ITF_RAY_MULTIPIECESACTORAICOMPONENT_H_
