#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SWARMCHASEAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_SwarmChaseAIComponent.h"
#endif //_ITF_RAY_SWARMCHASEAICOMPONENT_H_

#ifndef _ITF_RAY_SWARMCHASENODECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SwarmChaseNodeComponent.h"
#endif //_ITF_RAY_SWARMCHASENODECOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

//#define SWARM_EDGE_SIZE         1.0f // Atlas texture
#define SWARM_MOD_ANIM          1

#define SWARM_WIDTH_POOL        10.0f 
#define SWARM_HEIGHT_POOL       10.0f

#define SWARM_LENGTH_POOL       4.0f

#define SWARM_LENGTH_HISTORY    200

#define SWARM_SMOOTH_OBJ        0.1f
#define SWARM_DEPTH_STIM        3.0f
#define SWARM_PERCENT_ADD_WITDH_STIM        0.3f

#define SWARM_TIME_INTRO        2.0f

//#define SWARM_DEBUG             1

    namespace ITF
    {
    
    IMPLEMENT_OBJECT_RTTI(Ray_SwarmChaseAIComponent)
    
    BEGIN_SERIALIZATION_CHILD(Ray_SwarmChaseAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_SwarmChaseAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "LinkComponent mandatory");
    END_VALIDATE_COMPONENT()

    //*****************************************************************************

    Ray_SwarmChaseAIComponent::Ray_SwarmChaseAIComponent()
    : m_linkComponent(NULL)
    , m_fxControllerComponent(NULL)
    , m_activated(bfalse)
    , m_handleSound(U32_INVALID)
    , m_root(NULL)
    , m_criticTree(NULL)
    , m_isInit(bfalse)
    , m_trame(0)
    , m_time(0.0f)
    , m_countAtlasUv(0)
    , m_curIndexAtlas(0)
    , m_leaderCurNode(NULL)
    , m_leaderNextNode(NULL)
    , m_leaderSpeed(0.0f)
    , m_leaderCurDist(0.0f)
    , m_leaderMustBeChooseNode(bfalse)
    , m_countTotalParticlesBackup(0)
    , m_isEnterInCam(bfalse)
    , m_timeIntro(0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_SwarmChaseAIComponent::~Ray_SwarmChaseAIComponent()
    {

        // Clean pool
        for(u32 i = 0; i < m_poolList.size(); i++)
        {
            SF_DEL(m_poolList[i]);
        }        

        // Delete all node in tree
        if (m_root)
        {
            NodeList nodeAlreadyTreat;
            deleteNode(m_root, nodeAlreadyTreat, m_root);
        }

        // Delete critical path
        if (m_criticTree)
        {
            NodeList nodeAlreadyTreat;
            deleteNode(m_criticTree, nodeAlreadyTreat, m_criticTree);
        }
        
        // Clean atlas
        m_atlasSwarm.clear();
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Compos
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        // Events
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);

        // Init Texture Atlas
        m_atlasSwarm.createTexture(m_actor->getResourceGroup(), getTemplate()->getPathAtlas());

        Texture * texture = m_atlasSwarm.getAtlasTexture();
        if(texture)
        {
            const UVAtlas* uvAtlas = texture->getUVAtlas();

            if (uvAtlas)
            {
                m_countAtlasUv = uvAtlas->getNumberUV();

				m_listUV.clear();
				Vec2d* listUV = &m_listUV.incrSize(m_countAtlasUv * 4);
				for (u32 i = 0; i < m_countAtlasUv; i++)
				{
					uvAtlas->get4UVAt(i, listUV+i*4);
				}
            }
        }
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::startSound()
    {
        if (m_fxControllerComponent)
        {
            StringID soundName = "SwarmChase";
            if (soundName.isValid() && m_handleSound == U32_INVALID)
            {
                m_handleSound = m_fxControllerComponent->playFX(soundName);
                m_fxControllerComponent->setFXPos(m_handleSound,m_actor->getPos()); 
            }
        }
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::stopSound()
    {
        if (m_fxControllerComponent)
        {
            m_fxControllerComponent->stopFX(m_handleSound);
            m_handleSound = U32_INVALID;
        }
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::moveSound(Vec3d _pos)
    {
        if (m_fxControllerComponent && m_handleSound != U32_INVALID)
        {
            m_fxControllerComponent->setFXPos(m_handleSound, _pos); 
        }
    }

    //*****************************************************************************

   void Ray_SwarmChaseAIComponent::createNodeTree()
   {
        ITF_ASSERT(m_linkComponent);
        if (!m_linkComponent)
        {
            return;
        }

        Actor* root(NULL);

        // Search first link child with Ray_SwarmChaseNodeComponent
        const LinkComponent::ChildrenList& children  = m_linkComponent->getChildren();
        for (u32 i = 0; i < children.size(); i++)
        {
            Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,children[i].getPath());

            if ( pickable )
            {
                Actor* childActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                if (childActor)
                {
                    if (Ray_SwarmChaseNodeComponent* firstNode = childActor->GetComponent<Ray_SwarmChaseNodeComponent>())
                    {
                        root = childActor;
                        break;
                    }
                }
            }
        }


        if (root)
        {
            // Create root
            m_root = newAlloc(mId_Gameplay, Node);
            m_root->m_actorRef = root->getRef();
            m_root->m_radiusNode = m_root->m_actorRef.getActor()->getScale().m_x;

#ifdef SWARM_DEBUG
            //LOG_JOSS("Create root node  : %s, %08x", m_root->m_actorRef.getActor()->getUserFriendly().cStr(), m_root);
#endif

            generateNodeTree(m_root);

            // Create root critic
            m_criticTree = newAlloc(mId_Gameplay, Node);
            m_criticTree->m_actorRef = root->getRef();

#ifdef SWARM_DEBUG
            //LOG_JOSS("Create m_criticTree node  : %s, %08x", m_criticTree->m_actorRef.getActor()->getUserFriendly().cStr(), m_criticTree);
#endif

            generateCriticTree(m_criticTree);
    
        }
        else
        {
            ITF_WARNING(m_actor, 0, "You must link this actor with an SwarmChaseNodeComponent !");
        }

   }

   //*****************************************************************************

    void Ray_SwarmChaseAIComponent::generateNodeTree(Node* _node)
    {
        Actor* actNode = _node->m_actorRef.getActor();

        if (actNode)
        {
            if (LinkComponent* linkCompo = actNode->GetComponent<LinkComponent>())
            {

                const LinkComponent::ChildrenList& children  = linkCompo->getChildren();
                for (u32 i = 0; i < children.size(); i++)
                {
                    Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,children[i].getPath());

                    if ( pickable )
                    {
                        Actor* childActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                        if (childActor)
                        {
                            if (Ray_SwarmChaseNodeComponent* childNode = childActor->GetComponent<Ray_SwarmChaseNodeComponent>())
                            {

                                // Critic
                                bbool linkCritic = bfalse;
                                const StringID nameTagCritic = ITF_GET_STRINGID_CRC(Critic,3209625369);
                                StringID valueTagCritic; 
                                if (children[i].getTagValue(nameTagCritic, valueTagCritic))
                                {
                                    linkCritic = btrue;
                                }

                                // Trigger
                                bbool linkTrigger = bfalse;
                                const StringID nameTagTrigger = ITF_GET_STRINGID_CRC(Trigger,161061850);
                                i32 valueTagTrigger = 0; 
                                if (children[i].getTagValue(nameTagTrigger, valueTagTrigger))
                                {
                                    linkTrigger = btrue;
                                    m_countTotalParticlesBackup += valueTagTrigger;
                                }

                                if (!linkCritic)
                                {
                                    NodeList nodeAlreadyTreat;
                                    Node* nodeFound = searchNodeByActorRef( childActor->getRef(), m_root, nodeAlreadyTreat);

                                    if (nodeFound)
                                    {
                                        // Just add child
                                        _node->m_childList.push_back(nodeFound);
                                    }
                                    else
                                    {
                                        // Create child + add in child list
                                        Node* newNode = newAlloc(mId_Gameplay, Node);
                                        newNode->m_actorRef = childActor->getRef();
                                        newNode->m_radiusNode =  newNode->m_actorRef.getActor()->getScale().m_x;

                                        newNode->m_isBackupNode = linkTrigger;
                                        newNode->m_countBackupParticles = valueTagTrigger;

                                        _node->m_childList.push_back(newNode);

#ifdef SWARM_DEBUG
                                        //LOG_JOSS("New node  : %s /  %08x", childActor->getUserFriendly().cStr(), newNode);
#endif

                                        generateNodeTree(newNode);

                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::generateCriticTree(Node* _node)
    {
        Actor* actNode = _node->m_actorRef.getActor();

        if (actNode)
        {
            if (LinkComponent* linkCompo = actNode->GetComponent<LinkComponent>())
            {
                const LinkComponent::ChildrenList& children  = linkCompo->getChildren();

                for (u32 i = 0; i < children.size(); i++)
                {
                    Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,children[i].getPath());

                    if ( pickable )
                    {
                        Actor* childActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                        if (childActor)
                        {
                            if (Ray_SwarmChaseNodeComponent* childNode = childActor->GetComponent<Ray_SwarmChaseNodeComponent>())
                            {

                                bbool linkCritic = bfalse;

                                if (children.size() == 1)
                                {
                                    linkCritic = btrue;
                                }
                                else
                                {
                                    const StringID nameTag = ITF_GET_STRINGID_CRC(Critic,3209625369);
                                    StringID valueTag; 
                                    if (children[i].getTagValue(nameTag, valueTag))
                                    {
                                        linkCritic = btrue;
                                    }
                                }


                                if (linkCritic)
                                {
                                    // Create child + add in child list
                                    Node* newNode = newAlloc(mId_Gameplay, Node);
                                    newNode->m_actorRef = childActor->getRef();

                                    // Link set specif param ?
                                    const StringID nameTag = ITF_GET_STRINGID_CRC(SpeedMin,1490388197);
                                    f32 valueTagSpeedMin; 
                                    if (children[i].getTagValue(nameTag, valueTagSpeedMin))
                                    {
                                        if (valueTagSpeedMin > 0.0f)
                                        {
                                            newNode->m_speedMin = valueTagSpeedMin;
                                        }
                                    }

                                    _node->m_childList.push_back(newNode);

#ifdef SWARM_DEBUG
    //LOG_JOSS("New critic node  : %s /  %08x", childActor->getUserFriendly().cStr(), newNode);
#endif

                                    generateCriticTree(newNode);
                                    break;
                                    
                                }
                            }
                        }
                    }
                }
            }
        }
    }

//*****************************************************************************

    Ray_SwarmChaseAIComponent::Node* Ray_SwarmChaseAIComponent::searchNodeByActorRef( ActorRef _actorRef, Node* _searchNode, NodeList& _nodesAlreadyTreat )
    {
        ITF_ASSERT(_searchNode);
        if (!_searchNode)
        {
            return NULL;
        }

        ActorRef actRef = _searchNode->m_actorRef;

        if (actRef == _actorRef)
        {
            return _searchNode;
        }

        // Check if this node is already treat
        for (u32 i = 0; i < _nodesAlreadyTreat.size(); i++)
        {
            if (_searchNode == _nodesAlreadyTreat[i])
            {
                return NULL;
            }
        }

        // Add in already treat list
        _nodesAlreadyTreat.push_back(_searchNode);


        for(u32 i = 0; i < _searchNode->m_childList.size(); i++)
        {
            Node* nodeFound = searchNodeByActorRef(_actorRef, _searchNode->m_childList[i], _nodesAlreadyTreat);

            if (nodeFound)
            {
                return nodeFound;
            }
        }

        return NULL;
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::removeMyPointer( Node* _node, Node* _ptrNodeToRemove )
    {

        ITF_ASSERT(_node);
        ITF_ASSERT(_ptrNodeToRemove);

        if (!_node || !_ptrNodeToRemove)
        {
            return;
        }

        ITF_ASSERT(m_root);

        for(u32 i = 0; i < _node->m_childList.size(); i++)
        {
            Node* nChild = _node->m_childList[i];

            if (nChild)
            {
                if (nChild == _ptrNodeToRemove)
                {
                    _node->m_childList[i] = NULL;
                }
                else
                {
                    removeMyPointer( nChild, _ptrNodeToRemove );
                } 
            }  
        }

    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::deleteNode( Node* _node, NodeList& _nodesAlreadyTreat, Node* _rootTree)
    {
        ITF_ASSERT(_node);
        if (!_node)
        {
            return;
        }

        // Check if this node is already treat
        for (u32 i = 0; i < _nodesAlreadyTreat.size(); i++)
        {
            if (_node == _nodesAlreadyTreat[i])
            {
                return;
            }
        }

        // Add in already treat list
        _nodesAlreadyTreat.push_back(_node);

        for(u32 i = 0; i < _node->m_childList.size(); i++)
        {
            Node* nChild = _node->m_childList[i];
            if (nChild)
            {
                deleteNode(nChild, _nodesAlreadyTreat, _rootTree);
            }
        }

#ifdef SWARM_DEBUG
        //LOG_JOSS("Delete node  : %08x", _node);
#endif

        // Reset my pointer from all listChild
        removeMyPointer(_rootTree, _node);

        // Delete effective
        _node->m_actorRef.invalidate();
        SF_DEL(_node);

    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::growAABB()
    {

        AABB aabb(m_actor->getPos());

        for (u32 i = 0; i < m_poolList.size(); i++)
        {
            Pool* curPool = m_poolList[i];

            if(curPool)
            {
                const u32 countObj = curPool->m_objList.size();
                for (u32 j = 0; j < countObj; j++)
                {
                    SwarmObj& curObj = curPool->m_objList[j];
                    Vec3d addPos = curObj.m_pos.to3d();
                    aabb.grow(addPos);
                }
   
            }

        }

        m_actor->growRelativeAABBFromAbsolute(aabb);
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::onBecomeActive( )
    {
        Super::onBecomeActive();

        if (!m_isInit)
        {
            // Init obj list
            initPool(getTemplate()->getCountParticles());

            // Init tree + critic path
            createNodeTree();

            // Init Atlas
            m_atlasSwarm.initVertexBuffer(getTemplate()->getCountParticles() + m_countTotalParticlesBackup);

            m_isInit = btrue;
        }

    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::Update( f32 _dt )
    {

       if (m_activated)
       {
           m_time += _dt;

           // Move leader on critical path
           updateLeaderPos(_dt);

           // Move pool
           updatePoolPos(_dt);
           updatePoolSplit(_dt);

           // Move Particles
           updateObj(_dt);

           // Merge pool
           mergePool();

           updateAnimation();
           
           growAABB();

       }

    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::getPlayerCurrentNode( Actor* _actorPlayer, Node* _searchNode, NodeList& _nodesAlreadyTreat, f32& _distMin, Node* _nodeFound )
    {

        ITF_ASSERT(_searchNode);
        if (!_searchNode)
        {
            return;
        }

        // Check if this node is already treat
        for (u32 i = 0; i < _nodesAlreadyTreat.size(); i++)
        {
            if (_searchNode == _nodesAlreadyTreat[i])
            {
                return;
            }
        }

        // Add in already treat list
        _nodesAlreadyTreat.push_back(_searchNode);

        Actor* actNode = _searchNode->m_actorRef.getActor();
        f32 dist = (_actorPlayer->getPos() - actNode->getPos()).norm();

        if (dist < _distMin)
        {
            _distMin = dist;
        }

        for(u32 i = 0; i < _searchNode->m_childList.size(); i++)
        {
            getPlayerCurrentNode(_actorPlayer, _searchNode->m_childList[i], _nodesAlreadyTreat, _distMin, _nodeFound);
        }
  
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::updateLeaderPos( f32 _dt )
    {

        if(!m_leaderCurNode)
        {
            return;
        }

        if(!m_leaderNextNode)
        {
            return;
        }


		if (!m_isEnterInCam)
		{
			m_timeIntro -= _dt;

			if (m_timeIntro < 0.0f)
			{
				m_isEnterInCam = btrue;
			}
		}


        m_leaderLastPos = m_leaderCurPos;

        Vec3d posCur  = m_leaderCurNode->m_actorRef.getActor()->getPos();
        Vec3d posNext = m_leaderNextNode->m_actorRef.getActor()->getPos();
        Vec3d curDir  = (posNext - posCur).normalize();
        f32 distMaxforCurSeg = (posNext - posCur).norm();

        // compute speed
        f32 radiusDetect = getTemplate()->getDistMaxFromCam();

        // Compute intersection with screen border
        Vec2d cameraPos = CAMERACONTROLLERMANAGER->getCamPos().truncateTo2D();
        Vec2d dirToFollow = cameraPos - m_leaderCurPos.truncateTo2D();
        

#ifdef SWARM_DEBUG
        DebugDraw::circle(cameraPos - curDir.truncateTo2D() * radiusDetect, 0.0f, 0.2f, Color::yellow(), 0.0f);
        DebugDraw::text(cameraPos - curDir.truncateTo2D() * radiusDetect, 0.0f, Color::white(), "LIMIT");
#endif

        f32 speedMin = 0.0f;
        if (m_leaderNextNode->m_speedMin > 0.0f)
        {
            speedMin = m_leaderNextNode->m_speedMin;
        }
        else
        {
            speedMin = getTemplate()->getLeaderSpeedMin();
        }
        
        f32 speedMax = getTemplate()->getLeaderSpeedMax();
        f32 speed    = speedMin;

        // player is before or after ?
        f32 distProj = dirToFollow.dot(curDir.truncateTo2D());

        if (distProj >= 0.0f)
        {
            if (distProj > radiusDetect && (distProj - radiusDetect) > 0.1f)// 0.1f EPSILON ?
            {
                // En retard
                f32 ratrap = distProj - radiusDetect;

                if (!m_isEnterInCam)
                {
                    f32 progressIntro = 1.0f - (m_timeIntro / SWARM_TIME_INTRO);
                    ratrap = ratrap * progressIntro;
                    //speed = speedMax;
                }

                speed = ratrap / _dt;
            }
            else
            {
                f32 percentPenetration = distProj / radiusDetect;
                speed = speedMin + ((speedMax - speedMin) * percentPenetration);
            }
        }
        else
        {
            speed = speedMin;
        }

   
#ifdef SWARM_DEBUG
        DebugDraw::circle(m_leaderCurPos, radiusDetect, Color::green(), 0.0f);
        DebugDraw::text(m_leaderCurPos + Vec3d(0.5f, 0.5f, 0.0f), Color::white(), "Speed %f", speed);
#endif



        m_leaderCurDist += speed * _dt;

        if (m_leaderCurDist >= distMaxforCurSeg)
        {
            m_leaderCurPos = posNext;
            m_leaderCurDist = 0.0f;

            // Switch node
            m_leaderCurNode = m_leaderNextNode;
            if (m_leaderNextNode->m_childList.size() > 0)
            {
                m_leaderNextNode = m_leaderNextNode->m_childList[0];
            }
            else
            {
                m_leaderNextNode = NULL;
            }

        }
        else
        {
            m_leaderCurPos = posCur + (curDir * m_leaderCurDist);
        }


        // Move actor
        m_actor->setPos(m_leaderCurPos);

        // Compute normal
        Vec2d dir = (m_leaderCurPos - m_leaderLastPos).truncateTo2D();
        m_leaderNormal = dir.getPerpendicular().normalize();

        m_leaderSpeed = (m_leaderLastPos - m_leaderCurPos).norm();


        moveSound(m_leaderCurPos);


#ifdef SWARM_DEBUG
        DebugDraw::circle(m_leaderCurPos, 0.5f, Color::white(), 0.0f);
        DebugDraw::circle(m_leaderCurPos, 0.1f, Color::white(), 0.0f);
        //DebugDraw::text(curPool->m_curPos + Vec3d(0.5f, 0.5f, 0.0f), Color::white(), "Pool (%d) %08x", curPool->m_objList.size(), curPool);
#endif

     

    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::updateObj( f32 _dt )
    {
        const u32 countPool = m_poolList.size();
        for (u32 i = 0; i < countPool; i++)
        {
            Pool* curPool = m_poolList[i];
            ITF_ASSERT(curPool);

            if(curPool)
            {
                Vec2d curPoolPos = curPool->m_curPos.truncateTo2D();
                Vec2d poolNormal = curPool->m_normal;

                const u32 countObj = curPool->m_objList.size();
                for (u32 j = 0; j < countObj; j++)
                {
                    SwarmObj& curObj = curPool->m_objList[j];
                
                    curObj.m_lastPos = curObj.m_pos;
                    
                    // Compute SinX
                    f32 sinX = curObj.m_intensity_2 + (curObj.m_intensity_2 * f32_SinOpt((curObj.m_dephase_2 + m_time) * curObj.m_frequency_2));

                    f32 lengthInHistory = 0.0f;
                    Vec2d selectTargetPos = curPoolPos;
                    Vec2d selectTargetNormal = poolNormal;

                    Vec2d prevPos = curPoolPos;
                    u32 index = (curPool->m_countTrame-1) % curPool->m_lengthHistory;
                    if (curPool->m_historyPos.size() > 0)
                    {
                        bbool found = bfalse;
                        for(u32 k = index; k > 0; k--)
                        {
                            lengthInHistory += (prevPos - curPool->m_historyPos[k].truncateTo2D()).norm();
                            prevPos = curPool->m_historyPos[k].truncateTo2D();

                            if (sinX < lengthInHistory)
                            {
                                selectTargetNormal = (selectTargetPos - prevPos).getPerpendicular();
                                selectTargetNormal.normalize();

                                selectTargetPos = curPool->m_historyPos[k].truncateTo2D();

                                found = btrue;
                                break;
                            }
                        }

                        if (!found)
                        {
                            for(u32 k = curPool->m_historyPos.size() - 1; k > 0; k--)
                            {
                                lengthInHistory += (prevPos - curPool->m_historyPos[k].truncateTo2D()).norm();
                                prevPos = curPool->m_historyPos[k].truncateTo2D();

                                if (sinX < lengthInHistory)
                                {
                                    selectTargetNormal = (selectTargetPos - prevPos).getPerpendicular();
                                    selectTargetNormal.normalize();

                                    selectTargetPos = curPool->m_historyPos[k].truncateTo2D();

                                    found = btrue;
                                    break;
                                }
                            }
                        }

                    }
                    curObj.m_targetPos = selectTargetPos + ((poolNormal * curPool->m_curRadiusTube) * curObj.m_randomTargetY);

                    // Compute SinY
                    f32 sinY = curObj.m_intensity * f32_SinOpt((curObj.m_dephase + m_time) * curObj.m_frequency);
                    curObj.m_targetPos +=  selectTargetNormal * sinY;

#ifdef SWARM_DEBUG
                    DebugDraw::circle( curObj.m_targetPos, m_actor->getDepth(), 0.3f, Color::red());                    
#endif


                    // Compute Smooth factor (about leader speed)
                    f32 ratioSpeedleader = m_leaderSpeed / getTemplate()->getLeaderSpeedMax();
                    ratioSpeedleader = Clamp(ratioSpeedleader, 0.0f, 1.0f);
                    f32 smoothFactor = SWARM_SMOOTH_OBJ + ((1.0f - SWARM_SMOOTH_OBJ) * ratioSpeedleader);

                    // Compute position
                    curObj.m_pos += (( curObj.m_targetPos - curObj.m_pos) * smoothFactor);
          
                    curObj.m_speed = curObj.m_pos - curObj.m_lastPos;
                }

            }

        }

    }

    //*****************************************************************************

   void Ray_SwarmChaseAIComponent::sendStim(Vec2d _pos, Vec2d _dir, f32 _width)
   {
        PhysShapeBox shapeBox(Vec2d(SWARM_DEPTH_STIM, _width  + (_width * SWARM_PERCENT_ADD_WITDH_STIM)));

        PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>( &shapeBox );

        if (!stim)
        {
            ITF_ASSERT_MSG(0, "Stim error !");
            return;
        }

        _dir.normalize();

        f32 offset = -(0.3f + SWARM_DEPTH_STIM); // TODO : exposer offset
        _pos = _pos + _dir * offset;

        stim->setPos(_pos);
        stim->setDirection(_dir);
        stim->setAngle(_dir.getAngle());
        stim->setDepth(m_actor->getDepth());
        //stim->setFxPos(m_actor->getPos());
        stim->setOriginalSender( m_actor->getRef() );
        stim->setSender( m_actor->getRef() );
        stim->setLevel( getTemplate()->getHitLevel() );
        stim->setReceivedHitType( getTemplate()->getHitType() );
        stim->setFaction(getTemplate()->getFaction());

        STIMSMANAGER->sendStim(stim);
    
    }

    //*****************************************************************************
    
    void Ray_SwarmChaseAIComponent::mergePool()
    {

        for (u32 i = 0; i < m_poolList.size(); i++)
        {
            Pool* curPool = m_poolList[i];
            ITF_ASSERT(curPool);

            if(!curPool->m_curNode)
            {
                break;
            }

            for (u32 j = 0; j < m_poolList.size();j++)
            {
                Pool* checkPool = m_poolList[j];

                

                if (curPool != checkPool && 
                    curPool->m_curNode == checkPool->m_curNode &&
                    curPool->m_nextNode == checkPool->m_nextNode)
                {
                    f32 dotCoLinear = checkPool->m_normal.dot(curPool->m_normal);

                    if (dotCoLinear == 1.0f)
                    {     
                        // merge checkPool with curPool / no increment
                        const u32 countObj = checkPool->m_objList.size();

                        for (u32 k = 0; k < countObj; k++)
                        {
                            SwarmObj& obj = checkPool->m_objList[k];
                            curPool->m_objList.push_back(obj);
                        }

                        m_poolList.eraseNoOrder(j);

                        //repartObjInPool(curPool); // fait une saute

                        j--;
                    }

                }
              

            }

        }

    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::updatePoolPos( f32 _dt )
    {

		const u32 countPool = m_poolList.size();
        for (u32 i = 0; i < countPool; i++)
        {
            Pool* curPool = m_poolList[i];
            ITF_ASSERT(curPool);

            if(!curPool->m_curNode)
            {
                break;
            }

            if(!curPool->m_nextNode)
            {
                Vec2d dir = (curPool->m_curPos - curPool->m_lastPos).truncateTo2D();
                sendStim(curPool->m_posStim.truncateTo2D(), dir, curPool->m_curRadiusTube);
                break;
            }

            curPool->m_countTrame++;
            curPool->m_lastPos = curPool->m_curPos;

            Vec3d posCur  = curPool->m_curNode->m_actorRef.getActor()->getPos();
            Vec3d posNext = curPool->m_nextNode->m_actorRef.getActor()->getPos();
            Vec3d curDir  = (posNext - posCur).normalize();


            // Compute target pos by leaderPos            
            Vec2d p1 = m_leaderCurPos.truncateTo2D();
            Vec2d p2 = p1 + (m_leaderNormal * 10.0f);
            Vec2d p3 = posCur.truncateTo2D();
            Vec2d p4 = posNext.truncateTo2D();

            Vec2d intersectionPoint;
            u32 foundIntersec = intersectionLineLine(p1, p2, p3, p4, intersectionPoint);
            
            f32 distMaxforCurSeg = (posNext - posCur).norm();
            
            // Check if intersection is on segment
            Vec2d vecToIntersec = intersectionPoint - posCur.truncateTo2D();
            Vec2d vecSegment = (posNext.truncateTo2D() - posCur.truncateTo2D()).normalize();
            f32 dotIntersec = vecToIntersec.dot(vecSegment);

            if (dotIntersec < 0.0f || !foundIntersec)
            {
                intersectionPoint = posCur.truncateTo2D();
            }
           
            #ifdef SWARM_DEBUG
                DebugDraw::line2D(p1, intersectionPoint, m_actor->getDepth());                
                DebugDraw::circle(intersectionPoint, m_actor->getDepth(), 1.5f, Color::yellow());
            #endif

            curPool->m_curDist = (intersectionPoint - posCur.truncateTo2D()).norm(); // Follow leader

            if (curPool->m_curDist >= distMaxforCurSeg)
            {
                curPool->m_mustBeChooseNode = btrue;
                curPool->m_targetPos = posNext;
                curPool->m_curDist = distMaxforCurSeg;
            }
            else
            {
                curPool->m_targetPos = intersectionPoint.to3d();
            }


            // Compute Smooth factor (about leader speed)
            f32 ratioSpeedleader = m_leaderSpeed / getTemplate()->getLeaderSpeedMax();
            ratioSpeedleader = Clamp(ratioSpeedleader, 0.0f, 1.0f);
            f32 smoothFactor = getTemplate()->getSmoothFactor() + ((1.0f - getTemplate()->getSmoothFactor()) * ratioSpeedleader);
            
            // Compute current position
            curPool->m_curPos += ((curPool->m_targetPos - curPool->m_curPos) * smoothFactor);

            // Compute normal
            Vec2d dir = (curPool->m_curPos - curPool->m_lastPos).truncateTo2D();
            curPool->m_normal = dir.getPerpendicular().normalize();


            // Compute radius
            f32 progress = (curPool->m_curDist / distMaxforCurSeg);
            f32 startRadius = curPool->m_curNode->m_radiusNode;
            f32 stopRadius = startRadius;
            if (curPool->m_nextNode)
            {
                stopRadius = curPool->m_nextNode->m_radiusNode;
            }
            curPool->m_curRadiusTube = startRadius + ((stopRadius - startRadius) * progress);

            // Compute speed
            curPool->m_speed = (curPool->m_lastPos - curPool->m_curPos).norm();

            // History
            if (curPool->m_historyPos.size() < curPool->m_lengthHistory)
            {
                curPool->m_historyPos.push_back(curPool->m_curPos);
            }
            else
            {
                u32 index = (curPool->m_countTrame-1) % curPool->m_lengthHistory;
                curPool->m_historyPos[index] = curPool->m_curPos;
            }

            
            // Compute Smooth factor (about leader speed)
            ratioSpeedleader = m_leaderSpeed / getTemplate()->getLeaderSpeedMax();
            ratioSpeedleader = Clamp(ratioSpeedleader, 0.0f, 1.0f);
            smoothFactor = SWARM_SMOOTH_OBJ + ((1.0f - SWARM_SMOOTH_OBJ) * ratioSpeedleader);

            // Stim
            curPool->m_posStim += (( curPool->m_curPos - curPool->m_posStim) * smoothFactor); // TODO : expose
            sendStim(curPool->m_posStim.truncateTo2D(), dir, curPool->m_curRadiusTube);


            #ifdef SWARM_DEBUG
                DebugDraw::circle(curPool->m_targetPos, 0.5f, Color::red());
                DebugDraw::circle(curPool->m_curPos, 0.5f, Color::white(), 0.0f);
                //DebugDraw::text(curPool->m_curPos + Vec3d(0.5f, 0.5f, 0.0f), Color::white(), "Pool (%d) %08x", curPool->m_objList.size(), curPool);
            #endif

        }

    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::updatePoolSplit( f32 _dt )
    {
        for (u32 i = 0; i < m_poolList.size(); i++)
        {
            Pool* curPool = m_poolList[i];

            if (curPool->m_mustBeChooseNode)
            {
                chooseNextNode(curPool);
                curPool->m_mustBeChooseNode = bfalse;
            }
        }
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::updateAnimation()
    {
        m_trame++;
        u32 mod = m_trame%SWARM_MOD_ANIM;

        if (mod == 0)
        {
            if (m_countAtlasUv > 0)
            {
                m_curIndexAtlas++;
                m_curIndexAtlas = m_curIndexAtlas%m_countAtlasUv;
            }
        }
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);

        if (EventTrigger* onTriggerEvt = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            processTrigger(onTriggerEvt);
        }        

    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::initPool( u32 _CountNewObj )
    {

        // Create the first pool
        Pool* newPool = newAlloc(mId_Gameplay, Pool);
        ITF_ASSERT(newPool);
        m_poolList.push_back(newPool);

        for (u32 i = 0; i < _CountNewObj; i++)
        {
            SwarmObj newObj;

            newObj.m_intensity = Seeder::getSharedSeeder().GetFloat(0.0f, 0.5f); 
            newObj.m_frequency = Seeder::getSharedSeeder().GetFloat(2.0f, 5.0f);
            newObj.m_dephase = Seeder::getSharedSeeder().GetFloat(2.0f, 5.0f);

            newObj.m_intensity_2 = Seeder::getSharedSeeder().GetFloat(0.0f, SWARM_LENGTH_POOL); 
            newObj.m_frequency_2 = Seeder::getSharedSeeder().GetFloat(2.0f, 5.0f);
            newObj.m_dephase_2 = Seeder::getSharedSeeder().GetFloat(2.0f, 5.0f);

            // Add in pool
            newPool->m_objList.push_back(newObj);
        }


        repartObjInPool(newPool);
    
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::Draw()
    {
        Super::Draw();

        if (!m_activated)
        {
            return;
        }

        const f32 zActor =  m_actor->getDepth();
        const f32 semiEdge = getTemplate()->getSizeParticles() * 0.5f;

#ifdef SWARM_DEBUG
        DebugDraw::text("Count Pool : %d ",m_poolList.size());
#endif

		m_atlasSwarm.startFillVertexBuffer();
		ITF_VertexBuffer * vtxBuff = m_atlasSwarm.getCurrentVertexBuffer();
		const u32 color = Color::white().getAsU32();

		const Vec2d decalPoint1(-semiEdge, semiEdge);
		const Vec2d decalPoint2(-semiEdge, -semiEdge);
		const Vec2d decalPoint3(semiEdge, -semiEdge);
		const Vec2d decalPoint4(semiEdge, semiEdge);

		Vec2d p1, p2, p3, p4;
		Vec2d cs;

		u32 countTotalObj = 0;

        for (u32 i = 0; i < m_poolList.size(); i++)
        {
            const Pool* curPool = m_poolList[i];
            ITF_ASSERT(curPool);

            if(curPool)
            {

                #ifdef SWARM_DEBUG
                for (u32 j = 0; j < curPool->m_historyPos.size(); j++)
                {
                    //DebugDraw::text(curPool->m_historyPos[j], Color::white(), "%d", j);
                    DebugDraw::circle(curPool->m_historyPos[j], 0.1f, Color::white(), 0.0f);
                }
                #endif

                for (u32 k = 0; k < curPool->m_objList.size(); k++)
                {
                    const SwarmObj curObj = curPool->m_objList[k];
                    const Vec2d curObjPos = curObj.m_pos;

					countTotalObj++;

                    //p1 ----- p4
                    //|        |
                    //|        |
                    //p2 ----- p3

                    p1 = curObjPos + decalPoint1; 
                    p2 = curObjPos + decalPoint2; 
                    p3 = curObjPos + decalPoint3; 
                    p4 = curObjPos + decalPoint4; 

                    // Orientation
                    Vec2d speedDir = curObj.m_speed;
                    speedDir.normalize();
                    f32 angleRotate = speedDir.getAngle();

					Vec2d::CosSin(&cs, angleRotate);
					p1 = p1.RotateAroundCS(curObjPos, cs );
					p2 = p2.RotateAroundCS(curObjPos, cs );
					p3 = p3.RotateAroundCS(curObjPos, cs );
					p4 = p4.RotateAroundCS(curObjPos, cs );

					// index atlas
					u32 index = m_curIndexAtlas * 4;

					vtxBuff->wgp_write(color, zActor, p4, m_listUV[index]);
					vtxBuff->wgp_write(color, zActor, p3, m_listUV[index+1]);
					vtxBuff->wgp_write(color, zActor, p2, m_listUV[index+2]);
					vtxBuff->wgp_write(color, zActor, p1, m_listUV[index+3]);

#ifdef SWARM_DEBUG
                    DebugDraw::line2D(p4, p3, zActor, Color::yellow());    
                    DebugDraw::line2D(p3, p2, zActor, Color::yellow());    
                    DebugDraw::line2D(p2, p1, zActor, Color::yellow());    
                    DebugDraw::line2D(p1, p4, zActor, Color::yellow());    
#endif

                }

    

            }

#ifdef SWARM_DEBUG
            Vec2d lineP0 = curPool->m_curPos.truncateTo2D() - (curPool->m_normal * (curPool->m_curRadiusTube));
            Vec2d lineP1 = curPool->m_curPos.truncateTo2D() + (curPool->m_normal * (curPool->m_curRadiusTube));
            DebugDraw::line2D(lineP0, lineP1, m_actor->getDepth());                    
#endif

        }

		m_atlasSwarm.endFillVertexBuffer();
		m_atlasSwarm.drawVertexBuffer(zActor, countTotalObj);

    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::repartObjInPool( Pool* _pool )
    {

        u32 countObj = _pool->m_objList.size();
        f32 spaceY = 0.0f;
        f32 totalSpaceY = 0.0f;

        if (countObj > 0)
        {
            spaceY = 2.0f / countObj;
            totalSpaceY = -1.0f - (spaceY * 0.5f);
        }

        for(u32 i = 0; i < _pool->m_objList.size(); i++)
        {
            SwarmObj& obj = _pool->m_objList[i];

            totalSpaceY += spaceY;
            obj.m_randomTargetY = totalSpaceY;
        }

    }

    //*****************************************************************************


    void Ray_SwarmChaseAIComponent::chooseNextNode( Pool* _pool )
    {
        ITF_ASSERT(_pool);
        if (!_pool)
        {
            return;
        }

        Node* nextNode = _pool->m_nextNode;
        ITF_ASSERT(nextNode);

        const u32 countObj = _pool->m_objList.size();

        if (nextNode)
        {
            const u32 countChild = nextNode->m_childList.size();
            u32 countObjInNewPool = 0;
            u32 countMoved  = 0;  
            
            f32 layerSize = 0.0f;
            f32 layerLimit = -1.0f;

            // Filter child : Count real child (real path)
            Node* realChildListBuff[256];
            Node* triggerChildListBuff[256];
            NodeList realChildList(sizeof(realChildListBuff)/sizeof(Node*), MemoryId::mId_Temporary, realChildListBuff);
            NodeList triggerChildList(sizeof(triggerChildListBuff)/sizeof(Node*), MemoryId::mId_Temporary, triggerChildListBuff);

            for (u32 i = 0; i < countChild; i++)
            {
                Node* cNode = nextNode->m_childList[i];
                if (cNode->m_isBackupNode && !cNode->m_backupIsSended)
                {
					cNode->m_backupIsSended = btrue;
                    triggerChildList.push_back(cNode);
                }
                else
                {
                    realChildList.push_back(cNode);
                }
            }
            const u32 countRealChild = realChildList.size();

            // Execute trigger -> create new pool          
            const u32 countTriggerChild = triggerChildList.size();
            for (u32 i = 0; i < countTriggerChild; i++)
            {
                Node* cNode = triggerChildList[i];
                Node* cNextNode = NULL;

                if (cNode->m_childList.size() > 0) // TODO : warning, we must check if is real child ?
                {
                    cNextNode = cNode->m_childList[0];
                }

                // New backup pool
                Pool* newPool = newAlloc(mId_Gameplay, Pool);
                newPool->m_curNode   = cNode;

                newPool->m_nextNode  = cNextNode;
                newPool->m_curPos    = cNode->m_actorRef.getActor()->getPos();
                newPool->m_targetPos = cNode->m_actorRef.getActor()->getPos();
                newPool->m_posStim   = cNode->m_actorRef.getActor()->getPos();
                newPool->m_curDist   = 0.0f;

                for (u32 i = 0; i < cNode->m_countBackupParticles; i++)
                {
                    SwarmObj newObj;

                    newObj.m_intensity = Seeder::getSharedSeeder().GetFloat(0.0f, 0.5f); 
                    newObj.m_frequency = Seeder::getSharedSeeder().GetFloat(2.0f, 5.0f);
                    newObj.m_dephase = Seeder::getSharedSeeder().GetFloat(2.0f, 5.0f);

                    newObj.m_intensity_2 = Seeder::getSharedSeeder().GetFloat(0.0f, SWARM_LENGTH_POOL); 
                    newObj.m_frequency_2 = Seeder::getSharedSeeder().GetFloat(2.0f, 5.0f);
                    newObj.m_dephase_2 = Seeder::getSharedSeeder().GetFloat(2.0f, 5.0f);

                    newObj.m_pos = newPool->m_curPos.truncateTo2D();

                    // Add in pool
                    newPool->m_objList.push_back(newObj);
                }

                repartObjInPool(newPool);

                // Add the new pool in list
                m_poolList.push_back(newPool);

            }
            



            if (countRealChild > 0)
            {
                countObjInNewPool = countObj / countRealChild; // division entiere !
                layerSize = 2.0f / countRealChild;
            }
            
 
            if (countRealChild > 1)
            {
                // Split swarm pool


                for (u32 i = 0; i < countRealChild; i++)
                {
                    Node* cNode = realChildList[i];

                    layerLimit += layerSize;

                    if(i < countRealChild - 1)
                    {
                        // New pool
                        Pool* newPool = newAlloc(mId_Gameplay, Pool);
                        newPool->m_curNode   = nextNode;
                        newPool->m_nextNode  = cNode;
                        newPool->m_curPos    = _pool->m_curPos;
                        newPool->m_targetPos = nextNode->m_actorRef.getActor()->getPos();
                        newPool->m_posStim   = _pool->m_curPos;
                        newPool->m_curDist   = 0.0f;

                        // Copy data
                        newPool->m_historyPos = _pool->m_historyPos;
                        newPool->m_countTrame = _pool->m_countTrame;

                        // Move obj to the new pool
                        for (SwarmObjList::iterator it = _pool->m_objList.begin(); it != _pool->m_objList.end();)
                        {
                            // move
                            SwarmObj& obj = *it;

                            if (obj.m_randomTargetY <= layerLimit)
                            {

                                newPool->m_objList.push_back(obj);

                                // erase + update iterator
                                it = _pool->m_objList.erase(it);

                                countMoved++;
                                //if (countMoved >= countObjInNewPool)
                                //{
                                //    break;
                                //}

                            }
                            else
                            {
                                ++it;
                            }
                        }


                        repartObjInPool(newPool);

                        // Add the new pool in list
                        m_poolList.push_back(newPool);
                    }
                    else
                    {

                        _pool->m_curNode = nextNode;
                        _pool->m_nextNode = cNode;
                        _pool->m_targetPos = nextNode->m_actorRef.getActor()->getPos();
                        _pool->m_curDist = 0.0f;


                        repartObjInPool(_pool);

                    }
                }
            }
            else if (countRealChild == 1)
            {
                // Don't need split, just move to next bode
                Node* cNode = realChildList[0];

                _pool->m_curNode = nextNode;
                _pool->m_nextNode = cNode;
                _pool->m_curDist = 0.0f;

            }
            else
            {
                // Its the end of tree
                _pool->m_curNode = nextNode;
                _pool->m_nextNode = NULL;
                _pool->m_targetPos = nextNode->m_actorRef.getActor()->getPos();
                _pool->m_curDist = 0.0f;
            }
        }
    }

    //*****************************************************************************

    void Ray_SwarmChaseAIComponent::processTrigger(EventTrigger* _triggerEvt)
    {
        // Skip deactivate event
        if (!_triggerEvt->getActivated())
        {
            return;
        }

        // Skip re-trigger
        if (m_activated)
        {
            return;
        }

        ITF_ASSERT(m_root);
        if (!m_root)
        {
            return;
        }

        // TODO : function start/activate
        m_activated = btrue;
        m_timeIntro = SWARM_TIME_INTRO; // TODO : expose

        startSound();

        // Init first current node (pool ?)
        ITF_ASSERT(m_poolList.size() == 1);

        if (m_poolList.size() == 1)
        {
            Pool* firstPool = m_poolList[0];
            ITF_ASSERT(firstPool);

            if (firstPool)
            {
                firstPool->m_curNode = m_root;

                if (m_root->m_childList.size() > 0)
                {
                    firstPool->m_nextNode = m_root->m_childList[0];
                    firstPool->m_curPos   = m_root->m_actorRef.getActor()->getPos();
                    firstPool->m_posStim  = m_root->m_actorRef.getActor()->getPos();
                }
                else
                {
                    firstPool->m_nextNode = NULL;
                }


                // Init particles pos
                const u32 countObj = firstPool->m_objList.size();
                for (u32 i = 0; i < countObj; i++)
                {
                    SwarmObj& curObj = firstPool->m_objList[i];

                    // Random relative position
                    f32 x = Seeder::getSharedSeeder().GetFloat(-SWARM_WIDTH_POOL, 0.0f);
                    f32 y = Seeder::getSharedSeeder().GetFloat(-SWARM_HEIGHT_POOL*0.5, SWARM_HEIGHT_POOL*0.5);

                    curObj.m_pos = firstPool->m_curPos.truncateTo2D() + Vec2d(x, y);

                }



            }
        }
      

        // Init Leader
        m_leaderCurNode = m_criticTree;
        
        if (m_criticTree->m_childList.size() > 0)
        {
            m_leaderNextNode = m_criticTree->m_childList[0];
        }

        m_leaderCurPos   = m_criticTree->m_actorRef.getActor()->getPos();



    }

#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void Ray_SwarmChaseAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Ray_SwarmChaseAIComponent::SwarmObj
    //*****************************************************************************

    Ray_SwarmChaseAIComponent::SwarmObj::SwarmObj()
    : m_randomTargetY(0.0f)
    , m_intensity(0.0f)
    , m_frequency(0.0f)
    , m_dephase(0.0f)
    , m_intensity_2(0.0f)
    , m_frequency_2(0.0f)
    , m_dephase_2(0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_SwarmChaseAIComponent::SwarmObj::~SwarmObj()
    {
        // none
    }

    //*****************************************************************************
    // Ray_SwarmChaseAIComponent::Pool
    //*****************************************************************************

    Ray_SwarmChaseAIComponent::Pool::Pool()
    : m_curNode(NULL)
    , m_nextNode(NULL)
    , m_curDist(0.0f)
    , m_mustBeChooseNode(bfalse)
    , m_speed(0.0f)
    , m_curRadiusTube(0.0f)
    , m_lengthHistory(SWARM_LENGTH_HISTORY)
    , m_countTrame(0)
    , m_normal(Vec2d::Zero)
    {
        // none
    }

    //*****************************************************************************

    Ray_SwarmChaseAIComponent::Pool::~Pool()
    {
        // none
    }
   
    //*****************************************************************************
    // Ray_SwarmChaseAIComponent::Node
    //*****************************************************************************

    Ray_SwarmChaseAIComponent::Node::Node()
    : m_radiusNode(0.0f)
    , m_speedMin(-1.0f)
    , m_isBackupNode(bfalse) 
	, m_backupIsSended(bfalse)
    , m_countBackupParticles(0)
    {
        // none
    }

    //*****************************************************************************

    Ray_SwarmChaseAIComponent::Node::~Node()
    {
        // none
    }

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_SwarmChaseAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_SwarmChaseAIComponent_Template)

        SERIALIZE_MEMBER("pathAtlas",                   m_pathAtlas);
        SERIALIZE_MEMBER("hitLevel",                    m_hitLevel);
        SERIALIZE_RECEIVEDHITTYPE("hitType",            m_hitType);
        SERIALIZE_RAY_FACTION("faction",                m_faction);
        SERIALIZE_MEMBER("smoothFactor",                m_smoothFactor);
        SERIALIZE_MEMBER("leaderSpeedMin",              m_leaderSpeedMin);
        SERIALIZE_MEMBER("leaderSpeedMax",              m_leaderSpeedMax);
        SERIALIZE_MEMBER("distMaxFromCam",              m_distMaxFromCam);
        SERIALIZE_MEMBER("countParticles",              m_countParticles);
		SERIALIZE_MEMBER("sizeParticles",               m_sizeParticles);


    END_SERIALIZATION()

    //*****************************************************************************

    Ray_SwarmChaseAIComponent_Template::Ray_SwarmChaseAIComponent_Template()
    : m_hitLevel(0)
    , m_hitType(RECEIVEDHITTYPE_EJECTXY)
    , m_faction(RAY_FACTION_ENEMY) 
    , m_smoothFactor(0.1f)
    , m_leaderSpeedMin(7.0f)
    , m_leaderSpeedMax(15.0f)
    , m_distMaxFromCam(10.f)
    , m_countParticles(50)
	, m_sizeParticles(1.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_SwarmChaseAIComponent_Template::~Ray_SwarmChaseAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************



}