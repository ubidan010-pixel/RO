#ifndef _ITF_RAY_SWARMCHASEAICOMPONENT_H_
#define _ITF_RAY_SWARMCHASEAICOMPONENT_H_

#ifndef _ITF_RAY_AICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_AIComponent.h"
#endif //_ITF_RAY_AICOMPONENT_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

namespace ITF
{

    class LinkComponent;
    class FXControllerComponent;

    class Ray_SwarmChaseAIComponent : public Ray_AIComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SwarmChaseAIComponent, Ray_AIComponent,2796589410)
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT()

  private:
                
        //*****************************************************************************
        // Ray_SwarmChaseAIComponent::Node
        //*****************************************************************************
    
        class Node;
        typedef SafeArray<Node*> NodeList;

        class Node
        {
        public:

            Node();
            ~Node();

            ActorRef    m_actorRef;
            NodeList    m_childList;
            f32         m_radiusNode;

            // Param overwrite by link data
            f32         m_speedMin;
            bbool       m_isBackupNode;
			bbool		m_backupIsSended;
            u32         m_countBackupParticles;


        };

        //*****************************************************************************
        // Ray_SwarmChaseAIComponent::Pool
        //*****************************************************************************

        class Pool;
        class SwarmObj;

        typedef SafeArray<Pool*> PoolList;
        typedef ITF_VECTOR<SwarmObj> SwarmObjList;


        class Pool
        {
        public:

            Pool();
            ~Pool();

            // Infos node
            Node*       m_curNode;
            Node*       m_nextNode;

            // Infos leader move
            Vec3d       m_curPos;
            Vec3d       m_lastPos;
            Vec2d       m_normal;
            f32         m_speed;
            Vec3d       m_targetPos;
            f32         m_curDist;

            Vec3d       m_posStim;
            f32         m_curRadiusTube;

            // Flag
            bbool       m_mustBeChooseNode;

            SwarmObjList     m_objList;

            // History position 
            u32 m_lengthHistory; 
            u32 m_countTrame;
            SafeArray<Vec3d> m_historyPos;


        };

        //*****************************************************************************
        // Ray_SwarmChaseAIComponent::SwarmObj
        //*****************************************************************************

        class SwarmObj
        {
        public:
            SwarmObj();
            ~SwarmObj();

            Vec2d	m_pos;
            Vec2d	m_lastPos;
            Vec2d	m_speed;
            Vec2d   m_targetPos;
            f32     m_randomTargetY;

            // Sin
            f32     m_intensity;
            f32     m_frequency;
            f32     m_dephase;

            f32     m_intensity_2;
            f32     m_frequency_2;
            f32     m_dephase_2;

        };

     
        //*****************************************************************************

    public:

        Ray_SwarmChaseAIComponent();
        virtual ~Ray_SwarmChaseAIComponent();

        virtual bbool   needsUpdate() const     { return btrue; }
        virtual bbool   needsDraw() const       { return btrue; }
        virtual bbool   needsDraw2D() const     { return bfalse; }

        virtual void    Update( f32 _deltaTime );
        virtual void    onBecomeActive();
        virtual void    onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void    onEvent( Event * _event);

        virtual void    Draw();
#ifdef ITF_SUPPORT_EDITOR
        virtual void    drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const;
#endif // ITF_SUPPORT_EDITOR

    private:

        ITF_INLINE const class Ray_SwarmChaseAIComponent_Template*  getTemplate() const;
  
                void    growAABB();

                void    initPool( u32 _CountNewObj );
                void    processTrigger(EventTrigger* _triggerEvt);

                void    createNodeTree();
                void    generateNodeTree(Node* _node);
                void    generateCriticTree(Node* _node);
                void    deleteNode( Node* _node, NodeList& _nodesAlreadyTreat, Node* _rootTree );
                void    removeMyPointer( Node* _node, Node* _ptrNodeToRemove );
                void    updateLeaderPos( f32 _dt );
                void    mergePool();
                void    updatePoolPos( f32 _dt );
                void    updatePoolSplit( f32 _dt );
                void    updateObj( f32 _dt );
                void    updateAnimation();
                void    updateRelativeObjPos( f32 _dt );
                void    sendStim(Vec2d _pos, Vec2d _dir, f32 _width);
                void    getPlayerCurrentNode( Actor* _actorPlayer, Node* _searchNode, NodeList& _nodesAlreadyTreat, f32& _distMin, Node* _nodeFound);
                void    chooseNextNode( Pool* _pool );
                void    repartObjInPool( Pool* _pool );

                void    startSound();
                void    stopSound();
                void    moveSound(Vec3d _pos);

                Ray_SwarmChaseAIComponent::Node* searchNodeByActorRef( ActorRef _actorRef, Node* _searchRoot, NodeList& _nodesAlreadyTreat );


    private:
     
        LinkComponent*          m_linkComponent;
        FXControllerComponent*  m_fxControllerComponent;
        PoolList                m_poolList;

        // Atlas
        u32         m_trame;
        f32         m_time;
        u32         m_countAtlasUv;
        u32         m_curIndexAtlas;
        SafeArray<Vec2d> m_listUV;
        AtlasDynamicObject   m_atlasSwarm;

        // Root of node tree
        Node*       m_root;
        Node*       m_criticTree;

        // State
        bbool       m_isInit;
        bbool       m_activated;

        // Leader
        Node*       m_leaderCurNode;
        Node*       m_leaderNextNode;
        Vec3d       m_leaderCurPos;
        Vec3d       m_leaderLastPos;
        Vec2d       m_leaderNormal;
        f32         m_leaderSpeed;
        f32         m_leaderCurDist;
        bbool       m_leaderMustBeChooseNode;

        bbool       m_isEnterInCam;
        f32         m_timeIntro;

        // Backup
        u32         m_countTotalParticlesBackup;


        // Fx / Sound
        u32 m_handleSound;

    };

    //*****************************************************************************
    // Template
    //*****************************************************************************

    class Ray_SwarmChaseAIComponent_Template : public Ray_AIComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_SwarmChaseAIComponent_Template, Ray_AIComponent_Template,4027380365);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_SwarmChaseAIComponent);

    public:

        Ray_SwarmChaseAIComponent_Template();
        ~Ray_SwarmChaseAIComponent_Template();

        ITF_INLINE Path getPathAtlas() const                { return m_pathAtlas; }
        ITF_INLINE u32 getHitLevel() const                  { return m_hitLevel; }
        ITF_INLINE EReceivedHitType getHitType() const      { return m_hitType; }
        ITF_INLINE Faction getFaction() const               { return m_faction; }
        ITF_INLINE f32 getSmoothFactor() const              { return m_smoothFactor; }
        ITF_INLINE f32 getLeaderSpeedMin() const            { return m_leaderSpeedMin; }
        ITF_INLINE f32 getLeaderSpeedMax() const            { return m_leaderSpeedMax; }
        ITF_INLINE f32 getDistMaxFromCam() const  { return m_distMaxFromCam; }
        ITF_INLINE u32 getCountParticles() const            { return m_countParticles; }
		ITF_INLINE f32 getSizeParticles() const             { return m_sizeParticles; }

    private:


        // Atlas
        Path m_pathAtlas;

        // Hit
        u32 m_hitLevel;
        EReceivedHitType m_hitType;

        // Faction
        Faction m_faction;

        // Leader run
        f32 m_smoothFactor;
        f32 m_leaderSpeedMin;
        f32 m_leaderSpeedMax;
        f32 m_distMaxFromCam;

        // Particles
        u32 m_countParticles;
		f32 m_sizeParticles;
   

    };

    const Ray_SwarmChaseAIComponent_Template*  Ray_SwarmChaseAIComponent::getTemplate() const {return static_cast<const Ray_SwarmChaseAIComponent_Template*>(m_template);}
}

#endif // _ITF_RAY_SWARMCHASEAICOMPONENT_H_