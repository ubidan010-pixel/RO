#ifndef _ITF_TRAJECTORYDATA_H_
#define _ITF_TRAJECTORYDATA_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

namespace ITF {

class ChildEntry;
class TrajectoryNodeComponent;

#define TrajectoryLinkType_Walk ITF_GET_STRINGID_CRC(walk,2941578500)
#define TrajectoryLinkType_Jump ITF_GET_STRINGID_CRC(jump,2374255179)
#define TrajectoryLinkType_ToFly ITF_GET_STRINGID_CRC(toFly,54085300)
#define TrajectoryLinkType_Fly ITF_GET_STRINGID_CRC(fly,1445727613)
#define TrajectoryLinkType_Land ITF_GET_STRINGID_CRC(land,4279572777)
#define TrajectoryLinkType_Teleport ITF_GET_STRINGID_CRC(teleport,4081626522)
#define TrajectoryLinkType_ToCeiling ITF_GET_STRINGID_CRC(toCeiling,331167277)
#define TrajectoryLinkType_ToFloor ITF_GET_STRINGID_CRC(toFloor,3482416459)
#define TrajectoryLinkType_Dive ITF_GET_STRINGID_CRC(dive,3430303948)
#define TrajectoryLinkType_Swim ITF_GET_STRINGID_CRC(swim,1258662562)
#define TrajectoryLinkType_SkidAndWait ITF_GET_STRINGID_CRC(skidAndWait,2283880576)
#define TrajectoryLinkType_SkidAndTaunt ITF_GET_STRINGID_CRC(skidAndTaunt,1165366279)

//------------------------------------------------------------------------------
/// Stores link info between two TrajectoryNodeComponent
class TrajectoryLinkInfo
{
public:
    TrajectoryLinkInfo();
    
    void reset();
    void init(
        const TrajectoryNodeComponent* _before,
        const TrajectoryNodeComponent* _after
        );

    ITF_INLINE bbool isValid() const { return m_isValid; }
    ITF_INLINE bbool isBackwards() const { return m_backwards; }
    ITF_INLINE const ChildEntry& getChildEntry() const { return m_childEntry; }
    ITF_INLINE const StringID& getType() const { return m_type; }
    ITF_INLINE f32 getSpeed() const { return m_speed; }
    ITF_INLINE const StringID& getAnim() const { return m_anim; }
    ITF_INLINE bbool getFlip() const { return m_flip; }

    bbool useCurvedEnd() const;

private:
    bbool findChildEntry(const TrajectoryNodeComponent* _from, const TrajectoryNodeComponent* _to);

    bbool m_isValid;
    bbool m_backwards; // going from child to parent
    ChildEntry m_childEntry;
    StringID m_type;
    f32 m_speed;
    StringID m_anim;
    bbool m_flip; 
};

//------------------------------------------------------------------------------
/// Stores trajectory following info to pass to the TrajectoryNodeComponent
class TrajectoryData
{
public:
    TrajectoryData();
    
    void reset();
    void init(
        const TrajectoryNodeComponent* _previousNode,
        const TrajectoryNodeComponent* _currentNode,
        const TrajectoryNodeComponent* _nextNode,
        const TrajectoryNodeComponent* _nextNextNode
        );
    void reverse();
        
    TrajectoryNodeComponent* getPreviousNode() const;
    TrajectoryNodeComponent* getCurrentNode() const;
    TrajectoryNodeComponent* getNextNode() const;
    TrajectoryNodeComponent* getNextNextNode() const;

    ITF_INLINE const ActorRef& getPreviousNodeRef() const { return m_previousNode; }
    ITF_INLINE const ActorRef& getCurrentNodeRef() const { return m_currentNode; }
    ITF_INLINE const ActorRef& getNextNodeRef() const { return m_nextNode; }
    ITF_INLINE const ActorRef& getNextNextNodeRef() const { return m_nextNextNode; }

    ITF_INLINE const TrajectoryLinkInfo& getPreviousLink() const { return m_previousLink; }
    ITF_INLINE const TrajectoryLinkInfo& getCurrentLink() const { return m_currentLink; }
    ITF_INLINE const TrajectoryLinkInfo& getNextLink() const { return m_nextLink; }

    ITF_INLINE f32 getCurrentNodeDist() const { return m_currentNodeDist; }
    ITF_INLINE void setCurrentNodeDist(f32 _val) { m_currentNodeDist = _val; }
    ITF_INLINE f32 getLinkCursor() const { return m_linkCursor; }
    ITF_INLINE void setLinkCursor(f32 _val) { m_linkCursor = _val; }
    ITF_INLINE bbool getUpsideDown() const { return m_isUpsideDown; }
    ITF_INLINE void setUpsideDown(bbool _val) { m_isUpsideDown = _val; }
    ITF_INLINE const Vec3d& getEntryPos() const { return m_entryPos; }
    ITF_INLINE const Vec3d& getPos() const { return m_pos; }
    ITF_INLINE void setPos(const Vec3d& _val) { m_pos = _val; }
    ITF_INLINE f32 getAngle() const { return m_angle; }
    ITF_INLINE void setAngle(f32 _val) { m_angle = _val; }
    ITF_INLINE bbool getFlip() const { return m_flip; }
    ITF_INLINE void setFlip(bbool _val) { m_flip = _val; }

private:
    // nodes
    ActorRef m_previousNode;
    ActorRef m_currentNode;
    ActorRef m_nextNode;
    ActorRef m_nextNextNode;

    // link info
    TrajectoryLinkInfo m_previousLink;
    TrajectoryLinkInfo m_currentLink;
    TrajectoryLinkInfo m_nextLink;

    // movement info
    f32 m_currentNodeDist;
    f32 m_linkCursor;
    bbool m_isUpsideDown;
    Vec3d m_entryPos;
    Vec3d m_pos;
    f32 m_angle;
    bbool m_flip;
};

} // namespace ITF

#endif //_ITF_TRAJECTORYDATA_H_
