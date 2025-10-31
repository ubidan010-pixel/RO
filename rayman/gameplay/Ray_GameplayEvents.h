#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#define _ITF_RAY_GAMEPLAYEVENTS_H_

#ifndef _ITF_EVENTS_H_
#include "gameplay/GameplayEvents.h"
#endif //_ITF_EVENTS_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_RAY_GAMEPLAYTYPES_H_
#include "rayman/gameplay/Ray_GameplayTypes.h"
#endif //_ITF_RAY_GAMEPLAYTYPES_H_

#ifndef _ITF_RAY_CAMERACONTROLLERMANAGER_H_
#include "rayman/gameplay/Managers/Ray_CameraControllerManager.h"
#endif //_ITF_RAY_CAMERACONTROLLERMANAGER_H_

namespace ITF
{
class Ray_EventFruitAttachQuery : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventFruitAttachQuery,Event,1115792865);
    DECLARE_SERIALIZE()

public:

    Ray_EventFruitAttachQuery()
        : m_fruit(ITF_INVALID_OBJREF)
        , m_speed(Vec2d::Zero)
        , m_pos(Vec2d::Zero)
        , m_attached(bfalse)
        , m_useSnap(bfalse)
    {}
    Ray_EventFruitAttachQuery( ObjectRef _fruit, const Vec2d& _speed, const Vec2d& _pos )
        : m_fruit(_fruit)
        , m_speed(_speed)
        , m_pos(_pos)
        , m_attached(bfalse)
        , m_useSnap(bfalse)
    {}
    virtual ~Ray_EventFruitAttachQuery() {}

    ObjectRef               getFruit() const { return m_fruit; }
    const Vec2d&            getSpeed() const { return m_speed; }
    const Vec2d&            getPos() const { return m_pos; }

    bbool                   getAttached() const { return m_attached; }
    bbool                   getUseSnap() const { return m_useSnap; }
    void                    setAttached( bbool _attached, bbool _useSnap ) { m_attached = _attached; m_useSnap = _useSnap; }

private:

    ObjectRef               m_fruit;
    Vec2d                   m_speed;
    Vec2d                   m_pos;
    bbool                   m_attached;
    bbool                   m_useSnap;
};

class Ray_EventFruitDetach : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventFruitDetach,Event,60508415);
    DECLARE_SERIALIZE()

public:

    Ray_EventFruitDetach() : m_fruit(ITF_INVALID_OBJREF) {}
    Ray_EventFruitDetach( ObjectRef _fruit ) : m_fruit(_fruit) {}
    ~Ray_EventFruitDetach() {}

    const ObjectRef&        getFruit() const { return m_fruit; }

private:

    ObjectRef               m_fruit;
};

class Ray_EventPowerUp : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventPowerUp,Event,875814764);
    DECLARE_SERIALIZE()

public:
    Ray_EventPowerUp() : m_id(), m_enable(bfalse) {}
    Ray_EventPowerUp(const StringID& _id, bbool _enable) : m_id(_id), m_enable(_enable){}
    ~Ray_EventPowerUp() {}

    ITF_INLINE const StringID& getId() const { return m_id; }
    ITF_INLINE bbool enable() const { return m_enable; }

private:
    StringID m_id;
    bbool m_enable;
};

class Ray_EventPlayLockAnim : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventPlayLockAnim,Event,3679647184);
    DECLARE_SERIALIZE()

public:
    Ray_EventPlayLockAnim() : m_activate(bfalse), m_lookRight(btrue), m_snapActor(ITF_INVALID_OBJREF){}
    Ray_EventPlayLockAnim(bbool _activate) : m_activate(_activate), m_lookRight(btrue), m_snapActor(ITF_INVALID_OBJREF) {}  
    Ray_EventPlayLockAnim(bbool _activate, bbool _lookright, ObjectRef _snapActor) 
        : m_activate(_activate)
        , m_lookRight(_lookright)
        , m_snapActor(_snapActor)
    {}
    bbool getActivate() const { return m_activate;}
    bbool getLookRight() const { return m_lookRight; }
    ObjectRef getSnapActor() const { return m_snapActor; }

private:

    bbool m_activate;
    bbool m_lookRight;
    ObjectRef m_snapActor;
};

class Ray_EventSwingLatch : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSwingLatch,Event,2013309652);
    DECLARE_SERIALIZE()

public:

    Ray_EventSwingLatch();
    Ray_EventSwingLatch( ObjectRef _sender, f32 _length, f32 _angle, f32 _gravityMultiplier );

    f32                 getLength() const { return m_length; }
    f32                 getAngle() const { return m_angle; }
    f32                 getGravityMultiplier() const { return m_gravMultiplier; }

private:

    f32                 m_length;
    f32                 m_angle;
    f32                 m_gravMultiplier;
};

class Ray_EventSwingDetach : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSwingDetach,Event,3105940814);
    DECLARE_SERIALIZE()

public:

    Ray_EventSwingDetach() {}
    Ray_EventSwingDetach( ObjectRef _sender ) : Super(_sender) {}

private:

};

class Ray_EventQueryIsAccrobatic : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryIsAccrobatic,Event,1083236336);
    DECLARE_SERIALIZE()

public:

    Ray_EventQueryIsAccrobatic()
        : m_value(bfalse)
    {}
    ~Ray_EventQueryIsAccrobatic() {}

    bbool               getValue() const { return m_value; }
    void                setValue(bbool _value) { m_value = _value; }

private:

    bbool               m_value;
};

class Ray_EventQueryIsReceivingHit : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryIsReceivingHit, Event,1252094534);
    DECLARE_SERIALIZE()

public:

    Ray_EventQueryIsReceivingHit()
        : m_value(bfalse)
    {}
    ~Ray_EventQueryIsReceivingHit() {}

    bbool               getValue() const { return m_value; }
    void                setValue(bbool _value) { m_value = _value; }

private:

    bbool               m_value;
};

class Ray_EventHangOnCharacter : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventHangOnCharacter, Event,1277553679);
    DECLARE_SERIALIZE()

public:

    Ray_EventHangOnCharacter() : m_hanging(bfalse) {}
    Ray_EventHangOnCharacter( ObjectRef _sender, bbool _hanging ) : Super(_sender), m_hanging(_hanging) {}
    ~Ray_EventHangOnCharacter() {}

    bbool               getHanging() const { return m_hanging; }

private:

    bbool               m_hanging;
};



class Ray_EventQueryHangInfo : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryHangInfo, Event,4077208499);
    DECLARE_SERIALIZE()

public:

    Ray_EventQueryHangInfo();
    Ray_EventQueryHangInfo( ObjectRef _poly, u32 _edge, f32 _t, EOrientationPose _orientationPose, bbool _climb, bbool _hang );
    ~Ray_EventQueryHangInfo() {}

    ObjectRef           getPoly() const { return m_polyRef; }
    u32                 getEdge() const { return m_edgeIndex; }
    f32                 getT() const { return m_t; }
    bbool               getIsClimb() const { return m_climb; }
    bbool               getIsHang() const { return m_hang; }
    EOrientationPose    getOrientationPose() const { return m_orientationPose; }

    void                setPoly( ObjectRef _ref ) { m_polyRef = _ref; }
    void                setEdge( u32 _edge ) { m_edgeIndex = _edge; }
    void                setT( f32 _t ) { m_t = _t; }
    void                setIsClimb( bbool _climb ) { m_climb = _climb; }
    void                setIsHang( bbool _hang ) { m_hang = _hang; }
    void                setOrientationPose( EOrientationPose _orientationPose ) { m_orientationPose = _orientationPose; }

private:

    ObjectRef           m_polyRef;          // Polyline where we hang
    u32                 m_edgeIndex;        // Edge index of the polyline
    f32                 m_t;                // Relative distance from the first point of the edge (from 0 to 1)
    EOrientationPose    m_orientationPose;  // Horizontal/Vertical
    bbool               m_climb;            // If it's climb
    bbool               m_hang;             // If it's hang
};

class Ray_EventQueryStanceInfo : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryStanceInfo, Event,1627894922);
    DECLARE_SERIALIZE()

public:

    Ray_EventQueryStanceInfo();
    Ray_EventQueryStanceInfo( EStance _stance, EOrientationPose _orientationPose );
    ~Ray_EventQueryStanceInfo() {}

    EStance             getStance() const { return m_stance; }
    EOrientationPose    getOrientationPose() const { return m_orientationPose; }

    void                setStance( EStance _stance ) { m_stance = _stance; }
    void                setOrientationPose( EOrientationPose _orientationPose ) { m_orientationPose = _orientationPose; }

private:

    EStance             m_stance;           // Stance
    EOrientationPose    m_orientationPose;  // Horizontal/Vertical
};

class Ray_EventHangCoopJump : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventHangCoopJump, Event,930438087);
    DECLARE_SERIALIZE()

public:

    Ray_EventHangCoopJump() {}
    ~Ray_EventHangCoopJump() {}

private:

};

class Ray_EventSetPickedUp : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSetPickedUp, Event,2186198130);
    DECLARE_SERIALIZE()
public :
    Ray_EventSetPickedUp() : m_actor(ITF_INVALID_OBJREF), m_activator(ITF_INVALID_OBJREF) {}
    Ray_EventSetPickedUp(ObjectRef _actor, ObjectRef _activator) : m_actor(_actor), m_activator(_activator) {}
    ObjectRef getActor() const { return m_actor; }
    ObjectRef getActivator() const { return m_activator; }
private:
    ObjectRef m_actor;
    ObjectRef m_activator;
};

class Ray_EventReuse : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventReuse, Event,3973034730);
    DECLARE_SERIALIZE()
public :
    Ray_EventReuse() : m_actor() {}
    Ray_EventReuse(ObjectRef _actor) : m_actor(_actor) {}
    ObjectRef getActor() const { return m_actor; }
private:
    ObjectRef m_actor;
};

class Ray_EventLumEjection : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventLumEjection, Event,368805757);
    DECLARE_SERIALIZE()
public:
    Ray_EventLumEjection() 
        : m_ejectionForce(0.f,200.f)
        , m_ejectionDuration(0.f)
        , m_ejectionGravityAngle(bfalse, 0.0f)
        , m_isAutoPickup(bfalse)
    {
    }

    void setup(const ObjectRef &_sender, const Vec2d &_ejectionForce, f32 _ejectionDuration, Angle _ejectionGravityAngle, bbool _isAutoPickup) {m_ejectionForce = _ejectionForce; m_ejectionDuration = _ejectionDuration; m_isAutoPickup = _isAutoPickup; m_ejectionGravityAngle = _ejectionGravityAngle; setSender(_sender);}
    ITF_INLINE const Vec2d &getEjectionForce() const {return m_ejectionForce;}
    ITF_INLINE void setEjectionForce(const Vec2d &_force) {m_ejectionForce = _force;}
    ITF_INLINE f32 getEjectionDuration() const {return m_ejectionDuration;}
    ITF_INLINE void setEjectionDuration(f32 _duration) {m_ejectionDuration=_duration;}
    ITF_INLINE bbool getIsAutoPickup() const {return m_isAutoPickup;}
    ITF_INLINE void setIsAutoPickup(bbool _isAutoPickup) {m_isAutoPickup = _isAutoPickup;}
    ITF_INLINE Angle getEjectionGravityAngle() const { return m_ejectionGravityAngle; }
    ITF_INLINE void setEjectionGravityAngle(Angle _val) { m_ejectionGravityAngle = _val; }

private:
    Vec2d m_ejectionForce;
    f32 m_ejectionDuration;
    bbool m_isAutoPickup;
    Angle m_ejectionGravityAngle;
};

class Ray_EventBossSpawnActor : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventBossSpawnActor,Event,2063507162);
    DECLARE_SERIALIZE()

public:
    Ray_EventBossSpawnActor()
        : m_actorIndex(0xffffffff)
        , m_offset(Vec3d::Zero)
        , m_flipped(bfalse)
        {}
    Ray_EventBossSpawnActor(u32 _actorIndex, const Vec3d& _offset, bbool _flipped)
        : m_actorIndex(_actorIndex)
        , m_offset(_offset)
        , m_flipped(_flipped)
        {}

    ITF_INLINE u32             getActorIndex() const    { return m_actorIndex;  }
    ITF_INLINE const Vec3d&    getOffset() const        { return m_offset;      }
    ITF_INLINE bbool           getFlipped() const       { return m_flipped;     }

private:
    u32     m_actorIndex;
    Vec3d   m_offset;
    bbool   m_flipped;

};

class Ray_EventSpawnReward : public Event
{
    DECLARE_OBJECT_CHILD_RTTI_ABSTRACT(Ray_EventSpawnReward,Event,145553727);
    DECLARE_SERIALIZE()

public :

    Ray_EventSpawnReward()
        : m_numRewards(1)
        , m_autoPickup(btrue)
        , m_rewardReceiver(ITF_INVALID_OBJREF)
        , m_scene(NULL)
        , m_ejectionForce(200.0f)
        , m_ejectionRandomForce(100.0f)
        , m_ejectionRandomAngle(btrue, 30.0f)
        , m_ejectionDuration(0.0f)
        , m_ejectionGravityAngle(bfalse, 0.0f)
        , m_ejectionDir(1,0)

    {
        clear();
    }

    ITF_INLINE u32 getNumRewards() const { return m_numRewards; }
    ITF_INLINE void setNumRewards(u32 _num) { m_numRewards = _num; }
    ITF_INLINE bbool getAutoPickup() const { return m_autoPickup; }

    ITF_INLINE Scene*   getScene() const                    { return m_scene; } 
    ITF_INLINE void     setScene( Scene * _scene )          { m_scene = _scene; } 
    ITF_INLINE const Vec3d& getPos() const { return m_pos; }
    ITF_INLINE void     setPos( const Vec3d& _pos )         { m_pos = _pos; } 

    ITF_INLINE ObjectRef getRewardReceiver() const { return m_rewardReceiver; }
    ITF_INLINE void setRewardReceiver(ObjectRef _val) { m_rewardReceiver = _val; }

    ITF_INLINE ObjectRefList& getRewards() { return m_rewards; }

    ITF_INLINE Vec2d getEjectionDir() const { return m_ejectionDir; }
    ITF_INLINE void setEjectionDir(Vec2d _val) { m_ejectionDir = _val; }
    ITF_INLINE f32 getEjectionForce() const { return m_ejectionForce; }
    ITF_INLINE void setEjectionForce(f32 _val) { m_ejectionForce = _val; }
    ITF_INLINE f32 getEjectionRandomForce() const { return m_ejectionRandomForce; }
    ITF_INLINE void setEjectionRandomForce(f32 _val) { m_ejectionRandomForce = _val; }
    ITF_INLINE Angle getEjectionRandomAngle() const { return m_ejectionRandomAngle; }
    ITF_INLINE void setEjectionRandomAngle(Angle _val) { m_ejectionRandomAngle = _val; }
    ITF_INLINE f32 getEjectionDuration() const { return m_ejectionDuration; }
    ITF_INLINE void setEjectionDuration(f32 _val) { m_ejectionDuration = _val; }
    ITF_INLINE Angle getEjectionGravityAngle() const { return m_ejectionGravityAngle; }
    ITF_INLINE void setEjectionGravityAngle(Angle val) { m_ejectionGravityAngle = val; }

    void clear() 
    {
        m_rewards.clear();
        m_rewardReceiver = ITF_INVALID_OBJREF;
        m_scene = NULL;
        m_pos = Vec3d::Zero;
    }

    void setup( Actor* _sender, Vec3d _offset = Vec3d::Zero )
    {
        m_scene = _sender->getScene();
        Vec2d offset = _offset.truncateTo2D();
        offset = offset.Rotate(_sender->getAngle());
        m_pos = _sender->getPos() + offset.to3d(_offset.m_z);
    }

protected:

    u32 m_numRewards;
    bbool m_autoPickup;     // if true, the reward will be picked up by the receiver as soon as it is spawned
    Scene* m_scene;
    Vec3d m_pos;
    ObjectRef m_rewardReceiver;
    ObjectRefList m_rewards;
    Vec2d m_ejectionDir;
    f32   m_ejectionForce;
    f32   m_ejectionRandomForce;
    Angle m_ejectionRandomAngle;
    f32   m_ejectionDuration;
    Angle m_ejectionGravityAngle;
};




class Ray_EventSpawnRewardLum : public Ray_EventSpawnReward
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSpawnRewardLum, Ray_EventSpawnReward,812488722);
    DECLARE_SERIALIZE()
public :
    Ray_EventSpawnRewardLum() : m_forceYellow(bfalse) {}
    bbool getForceYellow() const {return m_forceYellow;}
    void setForceYellow(bbool _forceYellow) {m_forceYellow = _forceYellow;}
private:
    bbool m_forceYellow;
};

class Ray_EventSpawnRewardTun : public Ray_EventSpawnReward
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSpawnRewardTun, Ray_EventSpawnReward,1506323374);
    DECLARE_SERIALIZE()
public :
    Ray_EventSpawnRewardTun() {}
private:
};

class Ray_EventSpawnRewardHeart : public Ray_EventSpawnReward
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSpawnRewardHeart, Ray_EventSpawnReward,1546601933);
    DECLARE_SERIALIZE()
public :
    Ray_EventSpawnRewardHeart() {}
private: 
};

class Ray_EventRewardPickedUp : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventRewardPickedUp, Event,3660427225);
    DECLARE_SERIALIZE()
public :
    Ray_EventRewardPickedUp()
        : Super()
        , m_isLumsAutoPickupCircle(bfalse)
        , m_pos(Vec3d::Zero)
        , m_radius(0.0f)
        , m_rank(0)
        , m_maxRank(0)
        , m_forceYellowLum(bfalse)
        , m_immediateExplosion(bfalse)
    {}
    Ray_EventRewardPickedUp(
        const Vec3d& _pos,
        f32 _radius,
        u32 _rank,
        u32 _maxRank
        ) : Super()
        , m_isLumsAutoPickupCircle(btrue)
        , m_pos(_pos)
        , m_radius(_radius)
        , m_rank(_rank)
        , m_maxRank(_maxRank)
        , m_forceYellowLum(bfalse)
        , m_immediateExplosion(bfalse)
    {}
    ITF_INLINE bbool isLumsAutoPickupCircle() const { return m_isLumsAutoPickupCircle; }
    ITF_INLINE const Vec3d& getPos() const { return m_pos; }
    ITF_INLINE f32 getRadius() const { return m_radius; }
    ITF_INLINE u32 getRank() const { return m_rank; }
    ITF_INLINE u32 getMaxRank() const { return m_maxRank; }
    ITF_INLINE void setForceYellowLum(bbool _force) {m_forceYellowLum = _force;}
    ITF_INLINE bbool getForceYellowLum() const {return m_forceYellowLum;}
    ITF_INLINE bbool getExplodesImmediately() const {return m_immediateExplosion;}
    ITF_INLINE void setExplodesImmediately(bbool _explodes) {m_immediateExplosion = _explodes;}

    virtual void setActivator(ObjectRef _val) { m_activator = _val; }//We override that function for triggers
    virtual ObjectRef getActivator() const { return m_activator; } 

private:
    ObjectRef m_activator;
    bbool m_isLumsAutoPickupCircle;
    Vec3d m_pos;
    f32 m_radius;
    u32 m_rank;
    u32 m_maxRank;
    bbool m_forceYellowLum;
    bbool m_immediateExplosion; //used for skullcoin, for example
};

class Ray_EventSetDeathReward : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSetDeathReward, Event,2743373648);
    DECLARE_SERIALIZE()
public :
    Ray_EventSetDeathReward() : m_reward(NULL) {}
    ITF_INLINE Ray_EventSpawnReward * getReward() const { return m_reward; }
    ITF_INLINE void setReward(Ray_EventSpawnReward * _val) { m_reward = _val; }
private:
    Ray_EventSpawnReward * m_reward;
};

class Ray_EventMagnetTrigger : public EventTrigger
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventMagnetTrigger, EventTrigger,106997702);
    DECLARE_SERIALIZE()

public:

    Ray_EventMagnetTrigger()
        : Super()
        , m_magnetForce(0)
    {}
    ~Ray_EventMagnetTrigger() {}

    f32     getMagnetForce() const { return m_magnetForce; }

private:

    f32     m_magnetForce;
};



// sent from the FightManager to the victim to know if it can receive an attack (ie not immune)
// true by default: it's up to the victim to protest; when in doubt, attack!
class Ray_EventQueryCanBeAttacked : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryCanBeAttacked, Event,3134229644);

public:

    Ray_EventQueryCanBeAttacked()
        : Super()
        , m_canBeAttacked(btrue)
    {}
    ~Ray_EventQueryCanBeAttacked() {}

    bbool       getCanBeAttacked() const { return m_canBeAttacked; }
    void        setCanBeAttacked(bbool _value) { m_canBeAttacked = _value; }

private:

    bbool       m_canBeAttacked;
};



// sent from the FightManager to the attacker to let him know that he can attack its victim
class Ray_EventAttackGranted : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventAttackGranted, Event,3058211789);

public:

    Ray_EventAttackGranted()
        : Super()
        , m_attackUsed(bfalse)
    {}
    ~Ray_EventAttackGranted() {}

    bbool       getAttackUsed() const { return m_attackUsed; }
    void        setAttackUsed(bbool _value) { m_attackUsed = _value; }

private:

    bbool       m_attackUsed;
};



class Ray_EventMainLumDestroyed : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventMainLumDestroyed, Event,2883039676);

public:
private:
};

class Ray_EventQueryDarktoonificationPosition : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryDarktoonificationPosition, Event,785050175);
public:
    ITF_INLINE const Vec3d & getPos() const { return m_position; }
    ITF_INLINE void setPos(Vec3d & _pos) { m_position = _pos; }
private:
    Vec3d m_position;
};



class Ray_EventQueryCanDarktoonify : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryCanDarktoonify, Event,1194771444);
public:
    ITF_INLINE bbool getCanDarktoonify() const { return m_canDarktoonify; }
    ITF_INLINE void setCanDarktoonify(bbool _val) { m_canDarktoonify = _val; }
private:
    bbool m_canDarktoonify;
};



// sent by the victim to its darktoon to kick him off
class Ray_EventUndarktoonify : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventUndarktoonify, Event,2297042636);

public:
private:

};



class Ray_EventAddHealth : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventMagnetTrigger, Event,106997702);
    DECLARE_SERIALIZE()

public:

    Ray_EventAddHealth() : m_health(0)
    {}
    Ray_EventAddHealth(i32 _health) :  m_health(_health)
    {}
    ~Ray_EventAddHealth() {}
    ITF_INLINE i32 getHealth() const { return m_health; }
    ITF_INLINE void setHealth(i32 _val) { m_health = _val; }

private:

    i32     m_health;
};



// tells the AI to start dancing when approaching a dance stone
class Ray_EventDance : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventDance, Event,83576490);
    DECLARE_SERIALIZE()
};


class Ray_DoorTeleport : public EventTeleport
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_DoorTeleport, EventTeleport,3845549645);
    DECLARE_SERIALIZE()

public:

    Ray_DoorTeleport() {}
    Ray_DoorTeleport( const Vec3d& _pos, f32 _angle, f32 _entryAngle, f32 _exitAngle );
    ~Ray_DoorTeleport() {}

    f32                 getEntryAngle() const { return m_entryAngle; }
    f32                 getExitAngle() const { return m_exitAngle; }

private:

    f32                 m_entryAngle;
    f32                 m_exitAngle;
};



// tells the boss plant what his first node is
class Ray_EventSetInitialNode : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSetInitialNode, Event,3466697001);
    DECLARE_SERIALIZE()

public:

    Ray_EventSetInitialNode()
        : m_getNodeFromSender(btrue)
        , m_interruptCurrentNode(bfalse)
    {}
    ~Ray_EventSetInitialNode() {}

    // get the node from the sender or from the boss itself
    ITF_INLINE bbool getGetNodeFromSender() const { return m_getNodeFromSender; }
    ITF_INLINE bbool getInterruptCurrentNode() const { return m_interruptCurrentNode; }

private:

    bbool   m_getNodeFromSender;
    bbool   m_interruptCurrentNode;
};


// gives the boss (moray) a new selector to choose nodes from
class Ray_EventSetNodeSelector : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSetNodeSelector, Event,116563250);
    DECLARE_SERIALIZE()

public:

    Ray_EventSetNodeSelector() {}
    ~Ray_EventSetNodeSelector() {}
};


// send by the player to know if the current camera controller limits its movement
class Ray_EventQueryCameraLimiter : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryCameraLimiter, Event,358012532);

public:

    Ray_EventQueryCameraLimiter()
        : m_queryHandled(bfalse)
    {}
    ~Ray_EventQueryCameraLimiter() {}

    ITF_INLINE Ray_CameraLimiterData& getLimiterData() { return m_limiterData; }
    ITF_INLINE void setLimiterData( Ray_CameraLimiterData& _value ) { m_limiterData = _value; }

    ITF_INLINE bbool getQueryHandled() const { return m_queryHandled; }
    ITF_INLINE void setQueryHandled( const bbool _value ) { m_queryHandled = _value; }

private:

    Ray_CameraLimiterData m_limiterData;
    bbool m_queryHandled;
};

// sent by upside down node
class Ray_EventChestUpsideDown : public EventTrigger
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventChestUpsideDown, EventTrigger,186946345);
    DECLARE_SERIALIZE()

public:
    Ray_EventChestUpsideDown() {}
    ~Ray_EventChestUpsideDown() {}
};

// send by the boss nodes when they're reached
class Ray_EventNodeReached : public EventTrigger
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventNodeReached, EventTrigger,2473939155);
    DECLARE_SERIALIZE()

public:

    Ray_EventNodeReached()
        : m_cameraOn(bfalse)
        , m_cameraOff(bfalse)
        , m_cameraZOffset(0.f)
        , m_cameraOffset(0, 0)
        , m_cameraZOffsetDuration(1.f)
        , m_cameraOffsetDuration(1.f)
    {}
    ~Ray_EventNodeReached() {}

    ITF_INLINE bbool    getCameraOn() const { return m_cameraOn; }
    ITF_INLINE bbool    getCameraOff() const { return m_cameraOff; }
    ITF_INLINE f32      getCameraZOffset() const { return m_cameraZOffset; }
    ITF_INLINE const Vec2d& getCameraOffset() const { return m_cameraOffset; }
    ITF_INLINE f32      getCameraZOffsetDuration() const { return m_cameraZOffsetDuration; }
    ITF_INLINE f32      getCameraOffsetDuration() const { return m_cameraOffsetDuration; }
    ITF_INLINE const Margin& getCameraEjectMargin() const { return m_cameraEjectMargin; }
    ITF_INLINE const Margin& getCameraDeathMargin() const { return m_cameraDeathMargin; }

private:

    bbool       m_cameraOn;
    bbool       m_cameraOff;
    f32         m_cameraZOffset;
    Vec2d       m_cameraOffset;
    f32         m_cameraZOffsetDuration;
    f32         m_cameraOffsetDuration;
    Margin      m_cameraEjectMargin;
    Margin      m_cameraDeathMargin;
};


// sent by the boss moray nodes when they're reached
class Ray_EventBossMorayNodeReached : public EventTrigger
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventBossMorayNodeReached, EventTrigger,491301045);
    DECLARE_SERIALIZE()

public:

    Ray_EventBossMorayNodeReached()
        : m_speed(10.f)
        , m_acceleration(0.f)   // default: don't change
        , m_disableSpeedMultiplier(bfalse)
        , m_startDash(bfalse)
        , m_stopDash(bfalse)
    {}
    ~Ray_EventBossMorayNodeReached() {}

    ITF_INLINE f32      getSpeed() const { return m_speed; }
    ITF_INLINE f32      getAcceleration() const { return m_acceleration; }
    ITF_INLINE bbool    getDisableSpeedMultiplier() const { return m_disableSpeedMultiplier; }

    ITF_INLINE bbool    getStartDash() const { return m_startDash; }
    ITF_INLINE bbool    getStopDash() const { return m_stopDash; }

private:

    f32         m_speed;
    f32         m_acceleration;
    bbool       m_disableSpeedMultiplier;

    bbool       m_startDash;
    bbool       m_stopDash;
};


// sent by the boss moray to its body parts
class Ray_EventBossMorayFireMissile : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventBossMorayFireMissile, Event,1956304621);
    DECLARE_SERIALIZE()

public:

    Ray_EventBossMorayFireMissile()
        : m_bodyPartIndex(0)
    {}
    ~Ray_EventBossMorayFireMissile() {}

    ITF_INLINE u32 getBodyPartIndex() const { return m_bodyPartIndex; }

private:

    u32 m_bodyPartIndex;
};


// Triggers the victory sequence at the end of the current map
class Ray_EventTriggerVictory : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventTriggerVictory, Event,1674293021);
    DECLARE_SERIALIZE()

public:

    Ray_EventTriggerVictory() {}
    ~Ray_EventTriggerVictory() {}

private:

};

class Ray_EventChangePage : public EventTrigger
{
	DECLARE_OBJECT_CHILD_RTTI(Ray_EventChangePage, EventTrigger,1004490573);
	DECLARE_SERIALIZE()
public:
	Ray_EventChangePage():Super(), m_airTravelActive(bfalse), m_enterMovePosActive(bfalse), m_enterMove3DPos(Vec3d::Zero), m_finished(bfalse), m_hideActor(btrue), m_playFx(bfalse) {}
	~Ray_EventChangePage() {}

	ITF_INLINE void		        airTravelActivate()								{ m_airTravelActive = btrue; }
	ITF_INLINE bbool	        isAirTravelActive() const						{ return m_airTravelActive; }
	ITF_INLINE void		        enterMovePosActivate( Vec3d const & _enterPos, Vec2d const & _ejectForce )	{ m_enterMovePosActive = btrue; m_enterMove3DPos = _enterPos; m_enterEjectForce = _ejectForce; }
	ITF_INLINE bbool	        isEnterMovePosActive() const					{ return m_enterMovePosActive; }
	ITF_INLINE Vec3d const &    getEnterMovePos() const					        { return m_enterMove3DPos; }
	ITF_INLINE Vec2d const &    getEnterEjectForce() const					    { return m_enterEjectForce; }
    ITF_INLINE void             setPageActor( const ActorRef & _actor )         { m_pageActor = _actor; }
    ITF_INLINE const ActorRef & getPageActor() const                            { return m_pageActor; }
    ITF_INLINE void             setTargetPageActor( const ActorRef & _actor )   { m_targetActor = _actor; }
    ITF_INLINE const ActorRef & getTargetPageActor() const                      { return m_targetActor; }
    ITF_INLINE void             setFinished()                                   { m_finished = btrue; }
    ITF_INLINE bbool            isFinished() const                              { return m_finished; }
    ITF_INLINE bbool            getHideActor() const                            { return m_hideActor; }
    ITF_INLINE void             setHideActor( bbool _val )                      { m_hideActor = _val; }
    ITF_INLINE bbool            getPlayFx() const                               { return m_playFx; }
    ITF_INLINE void             setPlayFx()                                     { m_playFx = btrue; }

private :
	bbool	    m_airTravelActive;
	bbool	    m_enterMovePosActive;
	Vec3d	    m_enterMove3DPos;
	Vec2d	    m_enterEjectForce;
    ActorRef    m_pageActor;
    ActorRef    m_targetActor;
    bbool       m_finished;
    bbool       m_hideActor;
    bbool       m_playFx;
};

class Ray_EventChangePageQueryPoints : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventChangePageQueryPoints, Event,1998255083);
    DECLARE_SERIALIZE()
public:
    Ray_EventChangePageQueryPoints() : m_finalPointNeedEject(bfalse) {}
    ~Ray_EventChangePageQueryPoints() {}

    ITF_INLINE const Vec3d &	getExitPoint() const						    { return m_exitPoint; }
    ITF_INLINE const Vec3d &	getFinalPoint() const						    { return m_finalPoint; }
    ITF_INLINE bbool	        isFinalPointNeedEject() const				    { return m_finalPointNeedEject; }
    ITF_INLINE void	            setExitPoint( const Vec3d & _point )            { m_exitPoint = _point; }
    ITF_INLINE void	            setFinalPoint( const Vec3d & _point )           { m_finalPoint = _point; }
    ITF_INLINE void	            setFinalPointNeedEject( bbool _eject )          { m_finalPointNeedEject = _eject; }

private :
    Vec3d   m_exitPoint;
    Vec3d   m_finalPoint;
    bbool   m_finalPointNeedEject;
};

class Ray_EventSequenceFinished : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSequenceFinished, Event,2287469510);
    DECLARE_SERIALIZE()

public:

    Ray_EventSequenceFinished() {}
    ~Ray_EventSequenceFinished() {}

private:

};

// Forces an actor to move to a specific point however he can
class Ray_EventMoveToPoint : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventMoveToPoint, Event,1570511527);
    DECLARE_SERIALIZE()

public:

    Ray_EventMoveToPoint() : m_pos(Vec2d::Zero) {}
    Ray_EventMoveToPoint( const Vec2d& _pos ) : m_pos(_pos) {}
    ~Ray_EventMoveToPoint() {}

    const Vec2d&        getPos() const { return m_pos; }

private:

    Vec2d               m_pos;
};

class Ray_EventResetMoveToPoint : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventResetMoveToPoint, Event,918485638);
    DECLARE_SERIALIZE()

public:

    Ray_EventResetMoveToPoint() {}
    ~Ray_EventResetMoveToPoint() {}

private:

};

class Ray_EventQueryDoorData : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryDoorData, Event,2144329540);
    DECLARE_SERIALIZE()

public:

    Ray_EventQueryDoorData();
    Ray_EventQueryDoorData( const Vec2d& _entry, const Vec2d& _exit, bbool _noEntry );
    ~Ray_EventQueryDoorData() {}

    void                setData( const Vec2d& _entry, const Vec2d& _exit, bbool _noEntry );
    
    const Vec2d&        getEntry() const { return m_entry; }
    const Vec2d         getExit() const { return m_exit; }
    const bbool         getIsNoEntry() const { return m_noEntry; }

private:

    Vec2d   m_entry;
    Vec2d   m_exit;
    bbool   m_noEntry;
};

class Ray_EventShowUI : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShowUI, Event,3318916553);
    //DECLARE_SERIALIZE()

public:
    Ray_EventShowUI() {}
    ~Ray_EventShowUI() {}
};

class Ray_EventShowElectoonMedal : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShowElectoonMedal, Event,1324502185);
    //DECLARE_SERIALIZE()

public:
    Ray_EventShowElectoonMedal() 
        : m_forBrokenCage(bfalse)
        , m_playEndSequence(bfalse)
        , m_playVictorySequence(bfalse)
        , m_forLumAndTimeAttacks(bfalse)
        , m_dontSetTimeAttack(bfalse)
        , m_dontSetLumAttack1(bfalse)
        , m_dontSetLumAttack2(bfalse)
        , m_forBeatenBoss(bfalse)
        , m_autoEndLevel(bfalse)
        , m_currentlyEmphasizedChallenge(0)
    {}

    ~Ray_EventShowElectoonMedal() {}

    bbool m_forBrokenCage;
    bbool m_forBeatenBoss;
    bbool m_playEndSequence;
    bbool m_playVictorySequence;
    bbool m_autoEndLevel;
    bbool m_forLumAndTimeAttacks;

    //Following are used only when m_forLumAndTimeAttacks is btrue
    bbool m_dontSetTimeAttack;
    bbool m_dontSetLumAttack1; //If set, lumattack1 conditions are not evaluated and flag is not set in game state. However it is not removed
    bbool m_dontSetLumAttack2;

    // Custom sequence to play
    ActorRef m_customCineRef;

    //
    u32 m_currentlyEmphasizedChallenge; //combination of EChallengeType, or 0. eg, if set CHALLENGE_TIMEATTACK and this challenge is already won, electoon will be forced to arrive

};

class Ray_EventMedalCinematicEnded : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventMedalCinematicEnded, Event,1196816250);
    DECLARE_SERIALIZE()

public:
    Ray_EventMedalCinematicEnded() {}
    ~Ray_EventMedalCinematicEnded() {}    

};

class Ray_EventMedalNonCinematicEnded : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventMedalNonCinematicEnded, Event,694861796);
    DECLARE_SERIALIZE()

public:
    Ray_EventMedalNonCinematicEnded() {}
    ~Ray_EventMedalNonCinematicEnded() {}    

};

class Ray_EventReleaseChild : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventReleaseChild, Event,251461974);
    DECLARE_SERIALIZE()

public:
    Ray_EventReleaseChild() {}
    ~Ray_EventReleaseChild() {}    

};
class Ray_EventUturn : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventUturn, Event,121408696);
    DECLARE_SERIALIZE()

public:
    Ray_EventUturn() {}
    ~Ray_EventUturn() {}    

};
class Ray_EventSetSwimmingPolyline : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSetSwimmingPolyline, Event,4034857887);
    DECLARE_SERIALIZE()
    
public:
    Ray_EventSetSwimmingPolyline() {}
    ~Ray_EventSetSwimmingPolyline() {}    

    ITF_INLINE ObjectRef getPolyline() const { return m_polyline; }
    ITF_INLINE void setPolyline(ObjectRef _val) { m_polyline = _val; }
private:
    ObjectRef m_polyline;
};
class Ray_EventSpawn : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSpawn, Event,3403067276);
    DECLARE_SERIALIZE()
public:
};
//
class Ray_EventAddLum : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventAddLum, Event,4227734836);
    DECLARE_SERIALIZE()

public:
    Ray_EventAddLum() {m_isAccrobatic=bfalse;m_isKingLum=bfalse;m_outputScore=0;m_forceYellow=bfalse;}
    ~Ray_EventAddLum() {}

    bbool m_isAccrobatic;
    bbool m_isKingLum;
    ActorRef m_activator;
    bbool m_forceYellow;

    u32 m_outputScore; //result computed by the scorelum
};
//
class Ray_EventLumReachesScore : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventLumReachesScore, Event,1881707556);
    DECLARE_SERIALIZE()
public:
    Ray_EventLumReachesScore() {m_isAccrobatic=bfalse;m_valueToAdd=0;m_playerIndex=0;}
    ~Ray_EventLumReachesScore() {}

    bbool m_isAccrobatic;
    u32 m_valueToAdd;
    u32 m_playerIndex;
};
//
class Ray_EventQueryHunterNode : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryHunterNode, Event,210353395);
    DECLARE_SERIALIZE()
public:
    Ray_EventQueryHunterNode()
        : m_flip(btrue)
        , m_isHole(btrue)
        , m_numBulletsPerCycle(3)
        , m_numCycles(2)
        , m_index(0)
        , m_finishRadius(3.0f)
    {}
    ~Ray_EventQueryHunterNode() {}

    bbool   m_flip;
    bbool   m_isHole;
    u32     m_numBulletsPerCycle;
    u32     m_numCycles;
    u32     m_index;
    f32     m_finishRadius;

};

class Ray_EventQueryReachedMoveToPoint : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryReachedMoveToPoint, Event,1707458262);
    DECLARE_SERIALIZE()
public:
    Ray_EventQueryReachedMoveToPoint() : m_hasReached(bfalse) {}
    ~Ray_EventQueryReachedMoveToPoint() {}

    bbool           getHasReached() const { return m_hasReached; }
    void            setHasReached( bbool _val ) { m_hasReached = _val; }

private:

    bbool m_hasReached;
};

class Ray_EventQueryLumStage : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryLumStage, Event,4112170611);
    DECLARE_SERIALIZE()
public:
    Ray_EventQueryLumStage() : m_lumStage(0), m_comboState(0) {}
    ~Ray_EventQueryLumStage() {}

    ITF_INLINE u32 getLumStage() const { return m_lumStage; }
    ITF_INLINE void setLumStage(u32 _val) { m_lumStage = _val; }
    ITF_INLINE u32 getComboState() const { return m_comboState; }
    ITF_INLINE void setComboState(u32 _val) { m_comboState = _val; }

private:

    u32 m_lumStage;
    u32 m_comboState;
};

class Ray_EventBounceToLayer : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventBounceToLayer, Event,3529219527);
    DECLARE_SERIALIZE()
public:
    Ray_EventBounceToLayer();
    Ray_EventBounceToLayer( f32 _bounceHeight, f32 _bounceHeight2, f32 _bounceTime, ObjectRef _targetActor, const Vec2d& _offset, u32 _hurt );
    ~Ray_EventBounceToLayer() {}

    f32             getHeight() const { return m_bounceHeight; }
    f32             getHeight2() const { return m_bounceHeight2; }
    f32             getSpeed() const { return m_bounceSpeed; }
    ObjectRef       getTarget() const { return m_targetActor; }
    const Vec2d&    getOffset() const { return m_targetOffset; }
    u32             getHurt() const { return m_hurt; }

    void            setSkipped( bbool _val ) { m_skipped = _val; }
    bbool           getSkipped() const { return m_skipped; }

private:

    f32             m_bounceHeight;
    f32             m_bounceHeight2;
    f32             m_bounceSpeed;
    ObjectRef       m_targetActor;
    Vec2d           m_targetOffset;
    u32             m_hurt;
    bbool           m_skipped;
};

class Ray_EventTriggerBounce : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventTriggerBounce, Event,1890727905);

public:
    Ray_EventTriggerBounce() : Super(), m_isCrushAttack(bfalse) {}

    ITF_INLINE bbool isCrushAttack() const { return m_isCrushAttack; }
    ITF_INLINE void setCrushAttack(bbool _val) { m_isCrushAttack = _val; }

private:
    bbool m_isCrushAttack;
};

class Ray_EventJumpToPos : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventJumpToPos, Event,981487617);
    DECLARE_SERIALIZE()
public:
    Ray_EventJumpToPos();
    Ray_EventJumpToPos( const Vec2d& _pos );
    Ray_EventJumpToPos( ObjectRef _objRef );
    Ray_EventJumpToPos( ObjectRef _polyRef, u32 _edgeIndex, f32 _edgeT );
    ~Ray_EventJumpToPos() {}

    const Vec2d&            getPos() const { return m_pos; }
    ObjectRef               getObjRef() const { return m_objRef; }
    ObjectRef               getPolyRef() const { return m_polyRef; }
    u32                     getEdgeIndex() const { return m_edgeIndex; }
    f32                     getEdgeT() const { return m_edgeT; }
    
    void                    setStop( bbool _val ) { m_stop = _val; }
    bbool                   getStop() const { return m_stop; }

private:

    Vec2d                   m_pos;
    ObjectRef               m_objRef;
    ObjectRef               m_polyRef;
    u32                     m_edgeIndex;
    f32                     m_edgeT;
    bbool                   m_stop;
};

class Ray_EventShooterVacuumed : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShooterVacuumed, Event,1513162250);
    DECLARE_SERIALIZE()
public:
	Ray_EventShooterVacuumed() : m_vaccumedEffective( bfalse ){}
	ITF_INLINE void				setVaccumEffective( bbool _effective = btrue )	{ m_vaccumedEffective = _effective; }
	ITF_INLINE bbool			isVaccumEffective()	const		                { return m_vaccumedEffective; }
private :
	bbool m_vaccumedEffective;
};
class Ray_EventShooterVacuumReleased : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShooterVacuumReleased, Event,1672774138);
};
class Ray_EventShooterSwallow : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShooterSwallow, Event,48834168);
    DECLARE_SERIALIZE()
};
class Ray_EventShooterEjectActor : public Event
{
	DECLARE_OBJECT_CHILD_RTTI(Ray_EventShooterEjectActor, Event,937196133);
    DECLARE_SERIALIZE()
public:
	Ray_EventShooterEjectActor() {};
	Ray_EventShooterEjectActor( const Vec2d & _dir, const Vec2d & _force ) : m_ejectDir( _dir ), m_ejectForce( _force ){}

	ITF_INLINE const Vec2d & getEjectDir( )		const	{ return m_ejectDir; }
	ITF_INLINE const Vec2d & getEjectForce( )	const	{ return m_ejectForce; }
private:
	Vec2d m_ejectDir;
	Vec2d m_ejectForce;
};

class Ray_EventShooterCamera_TransitionNotify : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShooterCamera_TransitionNotify, Event,501414184);
public:
    Ray_EventShooterCamera_TransitionNotify() 
        : m_transitionDuration( 0.f )
        , m_destDepth( 0.f ) 
        , m_transitionType( 0 ) 
    {}

    ITF_INLINE void setTransitionDuration( const f32 & _duration )          { m_transitionDuration = _duration; }
    ITF_INLINE void setTransitionType( const u32 & _type )                  { m_transitionType = _type; }
    ITF_INLINE void setDestDepth( const f32 & _depth )                      { m_destDepth = _depth; }
    
    ITF_INLINE f32  getTransitionDuration()                 const           { return m_transitionDuration; }
    ITF_INLINE u32  getTransitionType()                     const           { return m_transitionType; }
    ITF_INLINE f32  getDestDepth()                          const           { return m_destDepth; }
private:
    f32 m_transitionDuration;
    u32 m_transitionType;
    f32 m_destDepth;
};

class Ray_EventShooterLandingRequested : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShooterLandingRequested, Event,492783699);
public:
    Ray_EventShooterLandingRequested() : m_landingPos( Vec2d::Zero )    { }
    ITF_INLINE void				setLandingPos( Vec2d & _landingPos )	{ m_landingPos = _landingPos; }
    ITF_INLINE const Vec2d &    getLandingPos()	const		            { return m_landingPos; }
private:
    Vec2d m_landingPos;
};

class Ray_EventCanBeVacuum : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventCanBeVacuum, Event,774659105);
    DECLARE_SERIALIZE()
public:
    Ray_EventCanBeVacuum() : m_canBeVaccumed( btrue )                                   { }
    ITF_INLINE void				setCanBeVaccum( bbool _canBeVaccumed = btrue )	{ m_canBeVaccumed = _canBeVaccumed; }
    ITF_INLINE bbool			getCanBeVaccum()	const		                { return m_canBeVaccumed; }
private :
    bbool m_canBeVaccumed;
};

class Ray_EventQueryHasBeenVacuumed : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryHasBeenVacuumed, Event,2297394390);
    DECLARE_SERIALIZE()
public:
    Ray_EventQueryHasBeenVacuumed() : m_hasBeenVacuumed( bfalse )                       { m_vacuumedOwner.invalidate(); }
    ITF_INLINE void				setHasBeenVacuumed( bbool _hasBeenVacuumed = btrue )	{ m_hasBeenVacuumed = _hasBeenVacuumed; }
    ITF_INLINE bbool			getHasBeenVacuumed()	const		                    { return m_hasBeenVacuumed; }
    ITF_INLINE void				setVacuumedOwner( const ActorRef & _owner )	            { m_vacuumedOwner = _owner; }
    ITF_INLINE ActorRef         getVacuumedOwner()	const		                        { return m_vacuumedOwner; }
private :
    bbool       m_hasBeenVacuumed;
    ActorRef    m_vacuumedOwner;
};

class Ray_EventIsInfluencedByVacuum : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventIsInfluencedByVacuum, Event,1958637106);
    DECLARE_SERIALIZE()
public:
    Ray_EventIsInfluencedByVacuum() : m_influenced( btrue )                                   { }
    ITF_INLINE void				setInfluenced( bbool _influenced = btrue )	{ m_influenced = _influenced; }
    ITF_INLINE bbool			getInfluenced()	const		                { return m_influenced; }
private :
    bbool m_influenced;
};
class Ray_EventHasBeenVacuumed  : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventHasBeenVacuumed, Event,1042014523);
    DECLARE_SERIALIZE()
};

class Ray_EventShooterVacuumBhvChangeState : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShooterVacuumBhvChangeState, Event,2152722158);
    DECLARE_SERIALIZE()
public:
    Ray_EventShooterVacuumBhvChangeState() : Super(), m_state( 0 ) {}

    ITF_INLINE const u32    getState( )		        const	{ return m_state; }
    ITF_INLINE const void   setState( u32 _state )	        { m_state = _state; }
    
private:
    u32 m_state;
};

class Ray_EventShooterInvulnerableActor : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShooterInvulnerableActor, Event,1840838639);
    DECLARE_SERIALIZE()
public:
    Ray_EventShooterInvulnerableActor() : Super(), m_invulnerable( btrue ) {}

    ITF_INLINE const bbool  IsInvulnerable( )		                const	{ return m_invulnerable; }
    ITF_INLINE const void   setInvulnerable( bbool _invulnerable )	        { m_invulnerable = _invulnerable; }

private:
    bbool m_invulnerable;
};

class Ray_EventShooterSpawnerDataSet : public EventTimedSpawnerDataSet
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShooterSpawnerDataSet, EventTimedSpawnerDataSet,538828432);
    DECLARE_SERIALIZE()
public:
    Ray_EventShooterSpawnerDataSet() : Super(), m_tweenID( StringID::InvalidId ) {}

    ITF_INLINE const void                           setTweenID( StringID::StringIdValueType _tweewId )  { m_tweenID = _tweewId; }
    ITF_INLINE const StringID::StringIdValueType    getTweenID()	                              const { return m_tweenID; }

private:
    StringID::StringIdValueType m_tweenID;
};

class Ray_EventShooterBossDataSet : public Event
{
    DECLARE_OBJECT_CHILD_RTTI( Ray_EventShooterBossDataSet, Event, 3071700792 );
    DECLARE_SERIALIZE()
public:
    Ray_EventShooterBossDataSet() : Super(), m_dataInt( 0 ), m_dataFloat( 0.0f ) {}
    ITF_INLINE void         setData( i32 _dataInt, f32 _dataFloat )     { m_dataInt = _dataInt; m_dataFloat = _dataFloat; }
    ITF_INLINE const i32    getDataInt()const                           { return m_dataInt; }
    ITF_INLINE const f32    getDataFloat()const                         { return m_dataFloat; }
private:
    i32 m_dataInt;
    f32 m_dataFloat;
};


class Ray_EventQueryChildLaunch : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryChildLaunch, Event,3130623924);
    DECLARE_SERIALIZE()

public:
    Ray_EventQueryChildLaunch() : m_canLaunch(bfalse) {}
    ~Ray_EventQueryChildLaunch() {}

    ITF_INLINE void touch() { m_canLaunch = btrue; }
    ITF_INLINE bbool canLaunch() const { return m_canLaunch; }

private:
    bbool m_canLaunch;
};

class Ray_EventChildLaunch : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventChildLaunch, Event,2714500499);

public:
    Ray_EventChildLaunch(const Vec3d& _launchPos) : Super(), m_launchPos(_launchPos) {}
    ~Ray_EventChildLaunch() {}
    ITF_INLINE const Vec3d& getLaunchPos() const { return m_launchPos; }

private:
    Ray_EventChildLaunch() {}
    Vec3d m_launchPos;
};

class Ray_EventChildLaunchAll : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventChildLaunchAll, Event,1581939056);
    DECLARE_SERIALIZE()

public:
    Ray_EventChildLaunchAll() {}
    ~Ray_EventChildLaunchAll() {}
};

class Ray_EventBulletLaunch : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventBulletLaunch, Event,1261820746);
    DECLARE_SERIALIZE()

public:
    Ray_EventBulletLaunch() {}
    ~Ray_EventBulletLaunch() {}
};

class Ray_EventSetBubblePrizeRewardNumber : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSetBubblePrizeRewardNumber, Event,2143348728);
    DECLARE_SERIALIZE()

public:
    Ray_EventSetBubblePrizeRewardNumber() : m_rewardNumber( 1 ) {}
    Ray_EventSetBubblePrizeRewardNumber( const u32 & _rewardNumber ) : m_rewardNumber(_rewardNumber) {}
    ~Ray_EventSetBubblePrizeRewardNumber() {}

    ITF_INLINE const u32 & getRewardNumber() const { return m_rewardNumber; }

private:
    u32 m_rewardNumber;
};


class Ray_EventHunterBirdBind : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventHunterBirdBind, Event,135728216);
    DECLARE_SERIALIZE()

public:
    Ray_EventHunterBirdBind() {}
    ~Ray_EventHunterBirdBind() {}
};


class Ray_EventHunterBirdUnbind : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventHunterBirdUnbind, Event,1029386227);
    DECLARE_SERIALIZE()

public:
    Ray_EventHunterBirdUnbind() {}
    ~Ray_EventHunterBirdUnbind() {}
};

class Ray_EventQueryAIData : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryAIData, Event,2473202000);
    DECLARE_SERIALIZE()

public:
    Ray_EventQueryAIData()
        : m_softCollisionRadius(0.0f)
        , m_softCollisionForceMultiplier(1.f)
        , m_isReducedSizd(bfalse)
    {}
    ~Ray_EventQueryAIData() {}

    f32 getSoftCollisionRadius() const { return m_softCollisionRadius;}
    void setSoftCollisionRadius(f32 _radius) { m_softCollisionRadius = _radius; }

    ITF_INLINE f32 getSoftCollisionForceMultiplier() const { return m_softCollisionForceMultiplier; }
    ITF_INLINE void setSoftCollisionForceMultiplier( f32 _value ) { m_softCollisionForceMultiplier = _value; }
    ITF_INLINE ITF::bbool getReducedSize() const { return m_isReducedSizd; }
    ITF_INLINE void setReducedSizd(ITF::bbool val) { m_isReducedSizd = val; }
    ITF_INLINE ObjectRef getSwimmingPolyline() const { return m_swimmingPolyline; }
    ITF_INLINE void setSwimmingPolyline( const ObjectRef _value ) { m_swimmingPolyline = _value; }

private:
    f32 m_softCollisionRadius;
    f32 m_softCollisionForceMultiplier;
    bbool m_isReducedSizd;
    ObjectRef m_swimmingPolyline;
};

class Ray_EventSuperPunchLaunch : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSuperPunchLaunch, Event,1975677917);
    DECLARE_SERIALIZE()

public:
    Ray_EventSuperPunchLaunch()
        : Super()
        , m_direction(Vec2d::Zero)
        , m_reducedHit(bfalse)
        {}
    Ray_EventSuperPunchLaunch(const Vec2d& _direction)
        : Super()
        , m_direction(_direction)
        , m_reducedHit(bfalse)
        {}
    ~Ray_EventSuperPunchLaunch() {}

    ITF_INLINE const Vec2d& getDirection() const { return m_direction; }

    ITF_INLINE bbool getReducedHit() const { return m_reducedHit; }
    ITF_INLINE void setReducedHit(bbool _val) { m_reducedHit = _val; }

private:
    Vec2d m_direction;
    bbool m_reducedHit;
};


// Sent to a friendly AI to give it an order
class Ray_EventAIOrder : public EventGeneric
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventAIOrder, EventGeneric,1055366852);
    DECLARE_SERIALIZE()

public:

    enum Type
    {
        ORDERTYPE_Unknown,
        ORDERTYPE_WaitForPlayer,
        ORDERTYPE_WaitForTrigger,
    };

    #define SERIALIZE_ORDERTYPE(name,val) \
    SERIALIZE_ENUM_BEGIN(name,val); \
        SERIALIZE_ENUM_VAR(ORDERTYPE_Unknown); \
        SERIALIZE_ENUM_VAR(ORDERTYPE_WaitForPlayer); \
        SERIALIZE_ENUM_VAR(ORDERTYPE_WaitForTrigger); \
    SERIALIZE_ENUM_END();


    Ray_EventAIOrder()
        : m_targetReachedRadius(0.3f)
        , m_playerDetectRadius(3.f)
        , m_orderType(ORDERTYPE_Unknown)
    {}
    virtual ~Ray_EventAIOrder() {}


    ITF_INLINE ActorRef getTarget() const { return m_target; }
    ITF_INLINE void setTarget( ActorRef _value ) { m_target = _value; }

    ITF_INLINE f32 getTargetReachedRadius() const { return m_targetReachedRadius; }
    ITF_INLINE void setTargetReachedRadius( f32 _value ) { m_targetReachedRadius = _value; }

    ITF_INLINE f32 getPlayerDetectRadius() const { return m_playerDetectRadius; }
    ITF_INLINE void setPlayerDetectRadius( f32 _value ) { m_playerDetectRadius = _value; }

    ITF_INLINE Type getOrderType() const { return m_orderType; }
    ITF_INLINE void setOrderType( Type _value ) { m_orderType = _value; }


private:
    ActorRef    m_target;
    f32         m_targetReachedRadius;
    f32         m_playerDetectRadius;
    Type        m_orderType;
};


// Sent by a friendly AI to ping back its order once it's processed
class Ray_EventAIOrderProcessed : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventAIOrderProcessed, Event,733417094);
    DECLARE_SERIALIZE()

public:

    Ray_EventAIOrderProcessed() {}
    virtual ~Ray_EventAIOrderProcessed() {}

private:
};


// Sent by a friendly AI to ping back its order once it's processed
class Ray_EventAIQueryNextOrder : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventAIQueryNextOrder, Event,2468917623);
    DECLARE_SERIALIZE()

public:

    Ray_EventAIQueryNextOrder()
        : m_hasNextOrder(bfalse)
    {}
    virtual ~Ray_EventAIQueryNextOrder() {}

    ITF_INLINE bbool getHasNextOrder() const { return m_hasNextOrder; }
    ITF_INLINE void setHasNextOrder( bbool _value ) { m_hasNextOrder = _value; }

    ITF_INLINE const Ray_EventAIOrder& getNextOrder() const { return m_nextOrder; }
    ITF_INLINE void setNextOrder( const Ray_EventAIOrder& _value ) { m_nextOrder = _value; }

private:

    bbool               m_hasNextOrder;
    Ray_EventAIOrder    m_nextOrder;
};

class Ray_EventInstantKill : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventInstantKill, Event,2400145155);
    DECLARE_SERIALIZE()

public:
    Ray_EventInstantKill() {}
    ~Ray_EventInstantKill() {}
};

class Ray_EventHandsCaught : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventHandsCaught, Event,3210089466);
    DECLARE_SERIALIZE()

public:
    Ray_EventHandsCaught() {}
    ~Ray_EventHandsCaught() {}
};

class Ray_EventHandsRelease : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventHandsRelease, Event,579391720);
    DECLARE_SERIALIZE()

public:
    Ray_EventHandsRelease() {}
    ~Ray_EventHandsRelease() {}
};

class Ray_EventSnapDance : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSnapDance, Event,806811309);
    DECLARE_SERIALIZE()

public:
    Ray_EventSnapDance() : m_isSnap(btrue) {}
    ~Ray_EventSnapDance() {}

    ITF_INLINE bbool getSnap() const { return m_isSnap; }
    ITF_INLINE void setSnap( bbool _value ) { m_isSnap = _value; }

    bbool m_isSnap;
};

class Ray_EventCaughtInPipe : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventCaughtInPipe, Event,1030315255);
    DECLARE_SERIALIZE()

public:
    Ray_EventCaughtInPipe()
        : m_catch(bfalse)
        , m_acceptCatch(bfalse)
    {}
    ~Ray_EventCaughtInPipe() {}

    ITF_INLINE bbool getCatch() const { return m_catch; }
    ITF_INLINE void setCatch( bbool _value ) { m_catch = _value; }

    ITF_INLINE bbool getAcceptCatch() const { return m_acceptCatch; }
    ITF_INLINE void setAcceptCatch( bbool _value ) { m_acceptCatch = _value; }

private:

    bbool   m_catch; // or release
    bbool   m_acceptCatch;
};

class Ray_EventTeleportUsed : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventTeleportUsed, Event,75030505);
    DECLARE_SERIALIZE()

public:
    Ray_EventTeleportUsed() : m_teleportTarget(ITF_INVALID_OBJREF), m_position(Vec2d::Zero) {}
    Ray_EventTeleportUsed( ObjectRef _sender, ObjectRef _teleportTarget, const Vec2d& _position );
    ~Ray_EventTeleportUsed() {}

    ObjectRef           getTeleportTarget() const { return m_teleportTarget; }
    const Vec2d&        getPosition() const { return m_position; }

private:

    ObjectRef           m_teleportTarget;
    Vec2d               m_position;
};

class Ray_EventTriggerChangePage : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventTriggerChangePage, Event,3503198481);
    DECLARE_SERIALIZE()

public:
    Ray_EventTriggerChangePage();
    Ray_EventTriggerChangePage( ObjectRef _sender, const Vec3d& _enterPoint, const Vec3d& _exitPoint, const Vec3d& _finalPoint,
                                bbool _verticalEject, ActorRef _destinationPage, ActorRef _activateCheckpoint,
                                f32 _waitPlayerDuration, bbool _useFade, bbool _isCageDoor );
    ~Ray_EventTriggerChangePage() {}

    const Vec3d&            getEnterPoint() const { return m_enterPoint; }
    const Vec3d&            getExitPoint() const { return m_exitPoint; }
    const Vec3d&            getFinalPoint() const { return m_finalPoint; }
    bbool                   getIsVerticalEject() const { return m_verticalEject; }
    ActorRef                getDestinationPage() const { return m_destinationPage; }
    ActorRef                getActivateCheckpoint() const { return m_activateCheckpoint; }
    f32                     getWaitPlayerDuration() const { return m_waitPlayerDuration; }
    bbool                   getUseFade() const { return m_useFade; }
    bbool                   getIsCageDoor() const {return m_isCageDoor;}

private:

    Vec3d       m_enterPoint;
    Vec3d       m_exitPoint;
    Vec3d       m_finalPoint;
    bbool       m_verticalEject;
    ActorRef    m_destinationPage;
    ActorRef    m_activateCheckpoint;
    f32         m_waitPlayerDuration;
    bbool       m_useFade;
    bbool       m_isCageDoor;
};

class Ray_EventTriggerEnterDoor : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventTriggerEnterDoor, Event,849805494);
    DECLARE_SERIALIZE()

public:
    Ray_EventTriggerEnterDoor() : m_destination(ITF_INVALID_OBJREF) {}
    Ray_EventTriggerEnterDoor( ObjectRef _sender, ObjectRef _destination ) : Super(_sender), m_destination(_destination) {}
    ~Ray_EventTriggerEnterDoor() {}

    ObjectRef           getDestination() const { return m_destination; }

private:

    ObjectRef           m_destination;
};

class Ray_EventReinitializeNPC : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventReinitializeNPC, Event,1176517542);
    DECLARE_SERIALIZE()
public:
    Ray_EventReinitializeNPC() {}
};

class Ray_EventTriggerStargate : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventTriggerStargate, Event,3125962121);
    DECLARE_SERIALIZE()

public:
    Ray_EventTriggerStargate() : m_start(Vec3d::Zero), m_speed(0.f) {}
    Ray_EventTriggerStargate( ObjectRef _sender, const Vec3d& _start, f32 _speed );
    ~Ray_EventTriggerStargate() {}

    const Vec3d&        getStart() const { return m_start; }
    f32                 getSpeed() const { return m_speed; }

private:

    Vec3d               m_start;
    f32                 m_speed;
};

class Ray_EventCanDanceOnStone : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventCanDanceOnStone, Event,1732983345);
    DECLARE_SERIALIZE()

public:
    Ray_EventCanDanceOnStone() : m_occupied(bfalse), m_opened(bfalse), m_canDance(bfalse), m_mustWaitForSartDance(bfalse) {}
    Ray_EventCanDanceOnStone( ObjectRef _sender ) : Super(_sender), m_occupied(bfalse), m_opened(bfalse), m_canDance(bfalse), m_mustWaitForSartDance(bfalse) {}
    ~Ray_EventCanDanceOnStone() {}

    bbool           isOccupied() const { return m_occupied; }
    bbool           isOpen() const { return m_opened; }
    bbool           getCanDance() const { return m_canDance; }
    void            setOccupied( bbool _val ) { m_occupied = _val; }
    void            setOpened( bbool _val ) { m_opened = _val; }
    void            setCanDance( bbool _val ) { m_canDance = _val; }

    void            setMustWaitForSartDance( bbool _val ) { m_mustWaitForSartDance = _val; }
    bbool           getMustWaitForSartDance() const { return m_mustWaitForSartDance; }

private:

    bbool           m_occupied;
    bbool           m_opened;
    bbool           m_mustWaitForSartDance;
    bbool           m_canDance;
};

class Ray_EventQueryEmptyStone : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryEmptyStone, Event,343141899);
    DECLARE_SERIALIZE()

public:
    Ray_EventQueryEmptyStone() : m_emptyStone(ITF_INVALID_OBJREF), m_waitZone(Vec3d::Zero) {}
    ~Ray_EventQueryEmptyStone() {}

    ObjectRef               getEmptyStone() const { return m_emptyStone; }
    void                    setEmptyStone( ObjectRef _ref ) { m_emptyStone = _ref; }

    Vec3d                   getWaitZone() const { return m_waitZone; }
    void                    setWaitZone( Vec3d _vec ) { m_waitZone = _vec; }

    ObjectRef               getDoor() const { return m_door; }
    void                    setDoor( ObjectRef _ref ) { m_door = _ref; }

private:

    ObjectRef               m_emptyStone;
    Vec3d                   m_waitZone;
    ObjectRef               m_door;
};

class Ray_EventStoneActivationFinished : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventStoneActivationFinished, Event,1905511856);
    DECLARE_SERIALIZE()

public:
    Ray_EventStoneActivationFinished() : m_stone(ITF_INVALID_OBJREF) {}
    Ray_EventStoneActivationFinished( ObjectRef _sender, ObjectRef _stone ) : Super(_sender), m_stone(_stone) {}
    ~Ray_EventStoneActivationFinished() {}

    ObjectRef               getStone() const { return m_stone; }

private:

    ObjectRef               m_stone;
};

class Ray_EventQueryIsInPedestal : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryIsInPedestal, Event,2146089474);
    DECLARE_SERIALIZE()

public:
    Ray_EventQueryIsInPedestal() : m_inPedestal(bfalse) {}
    ~Ray_EventQueryIsInPedestal() {}

    bbool                   getIsInPedestal() const { return m_inPedestal; }
    void                    setIsInPedestal( bbool _val ) { m_inPedestal = _val; }

private:

    bbool                   m_inPedestal;
};

class Ray_MusicScoreOpenEvent : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_MusicScoreOpenEvent, Event,2109898583);
    DECLARE_SERIALIZE()
 
public:
    Ray_MusicScoreOpenEvent()
        : Super()
        , m_maxNote(0)
        , m_snapNote(0)
        , m_snapDistance(0.0f)
        , m_yInterval(0.0f)
        , m_forced(bfalse)
        {}
    Ray_MusicScoreOpenEvent(
        u32 _maxNote,
        u32 _snapNote,
        f32 _snapDistance,
        f32 _yInterval,
        bbool _forced
        )
        : Super()
        , m_maxNote(_maxNote)
        , m_snapNote(_snapNote)
        , m_snapDistance(_snapDistance)
        , m_yInterval(_yInterval)
        , m_forced(_forced)
        {}
    ~Ray_MusicScoreOpenEvent() {}
    
    ITF_INLINE u32 getMaxNote() const { return m_maxNote; }
    ITF_INLINE u32 getSnapNote() const { return m_snapNote; }
    ITF_INLINE f32 getSnapDistance() const { return m_snapDistance; }
    ITF_INLINE f32 getYInterval() const { return m_yInterval; }
    ITF_INLINE bbool isForced() const { return m_forced; }

private:
    u32 m_maxNote;
    u32 m_snapNote;
    f32 m_snapDistance;
    f32 m_yInterval;
    bbool m_forced;
};

class Ray_MusicScoreCloseEvent : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_MusicScoreCloseEvent, Event,2878829646);
    DECLARE_SERIALIZE()

public:
    Ray_MusicScoreCloseEvent() : Super() {}
    ~Ray_MusicScoreCloseEvent() {}
};

class Ray_EventRestoreDeath : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventRestoreDeath, Event,3768337963);
    DECLARE_SERIALIZE()

public:
    Ray_EventRestoreDeath() {}
    ~Ray_EventRestoreDeath() {}

private:

};

class Ray_EventQueryCanJumpFromCurrent : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryCanJumpFromCurrent, Event,1690201300);
    DECLARE_SERIALIZE()

public:
    Ray_EventQueryCanJumpFromCurrent() : m_canJump(bfalse) {}
    Ray_EventQueryCanJumpFromCurrent( const Vec2d& _pos ) : m_pos(_pos), m_canJump(bfalse) {}
    ~Ray_EventQueryCanJumpFromCurrent() {}

    const Vec2d&        getPos() const { return m_pos; }

    void                setCanJump( bbool _val ) { m_canJump = _val; }
    bbool               getCanJump() const { return m_canJump; }

private:

    Vec2d               m_pos;
    bbool               m_canJump;
};

class Ray_EventActivateStone : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventActivateStone, Event,2310010701);
    DECLARE_SERIALIZE()

public:
    Ray_EventActivateStone() : m_received(bfalse) {}
    Ray_EventActivateStone( ObjectRef _sender, ObjectRef _stoneRef ) : Super(_sender), m_stone(_stoneRef), m_received(bfalse) {}
    ~Ray_EventActivateStone() {}

	ObjectRef					getStone() const { return m_stone; }

    bbool                       isReceived() const { return m_received; }
    void                        setReceived( bbool _val ) { m_received = _val; }

private:

	ObjectRef					m_stone;
    bbool                       m_received;
};

class Ray_EventQueryCanActivateStone : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryCanActivateStone, Event,1176450728);
    DECLARE_SERIALIZE()

public:
    Ray_EventQueryCanActivateStone() : m_canActivate(bfalse), m_isOccupied(bfalse) {}
    Ray_EventQueryCanActivateStone( ObjectRef _sender ) : Super(_sender), m_canActivate(btrue), m_isOccupied(bfalse) {}
    ~Ray_EventQueryCanActivateStone() {}

    bbool                   getCanActivate() const { return m_canActivate; }
    void                    setCanActivate( bbool _val ) { m_canActivate = _val; }

    bbool                   getIsOccupied() const { return m_isOccupied; }
    void                    setIsOccupied( bbool _val ) { m_isOccupied = _val; }

private:

    bbool                   m_canActivate;
    bbool                   m_isOccupied;
};

class Ray_EventStargateUse : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventStargateUse, Event,3921841867);
    DECLARE_SERIALIZE()

public:

    Ray_EventStargateUse() : m_stage(STARGATESTAGE_SUCKIN), m_suckPoint(Vec3d::Zero), m_stargateStart(Vec3d::Zero), m_lastTrip(bfalse) {}
    ~Ray_EventStargateUse() {}

    EStargateStage              getStage() const { return m_stage; }
    void                        setStage( EStargateStage _stage ) { m_stage = _stage; }

    const Vec3d&                getSuckPoint() const { return m_suckPoint; }
    void                        setSuckPoint( const Vec3d& _pos ) { m_suckPoint = _pos; }

    const Spline&               getSpline() const { return m_spline; }
    Spline&                     getSpline() { return m_spline; }

	const Vec3d&				getStargateStart() const { return m_stargateStart; }
	void						setStargateStart( const Vec3d& _start ) { m_stargateStart = _start; }

    bbool                       getIsLastTrip() const { return m_lastTrip; }
    void                        setIsLastTrip( bbool _val ) { m_lastTrip = _val; }

private:

    Spline                      m_spline;
    EStargateStage              m_stage;
    Vec3d                       m_suckPoint;
	Vec3d						m_stargateStart;
    bbool                       m_lastTrip;
};

class Ray_EventStargateStageFinished : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventStargateStageFinished, Event,211966772);
    DECLARE_SERIALIZE()

public:

    Ray_EventStargateStageFinished() : m_stage(STARGATESTAGE_SUCKIN),  m_abort(bfalse) {}
    Ray_EventStargateStageFinished( ObjectRef _sender, EStargateStage _stage, bbool _abort = bfalse ) : Super(_sender), m_stage(_stage), m_abort(_abort) {}
    ~Ray_EventStargateStageFinished() {}

    EStargateStage              getStage() const { return m_stage; }
    void                        setStage( EStargateStage _stage ) { m_stage = _stage; }

    bbool                       getAbort() const { return m_abort; }

private:

    EStargateStage              m_stage;
    bbool                       m_abort;
};

class Ray_EventBouncePropagate : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventBouncePropagate, Event,889182957);
    DECLARE_SERIALIZE()

public:
    Ray_EventBouncePropagate() {}
    Ray_EventBouncePropagate( ObjectRef _sender ) : Super(_sender) {}
    ~Ray_EventBouncePropagate() {}

private:

};

//
class Ray_EventSpawnWave : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSpawnWave, Event,588272106);
    DECLARE_SERIALIZE()
public:
    Ray_EventSpawnWave() : m_waveType(0), m_speed(0), m_delayBeforeMoving(0), m_mustOffsetByRadius(bfalse) {}

    void setup(u32 _waveType, f32 _speed) {m_waveType = _waveType; m_speed = _speed;}
    u32 getWaveType() const {return m_waveType;}
    f32 getSpeed() const {return m_speed;}
    f32 getDelayBeforeMoving() const {return m_delayBeforeMoving;}
    bbool getMustOffsetByRadius() const {return m_mustOffsetByRadius;}

private:
    u32 m_waveType;
    f32 m_speed;
    f32 m_delayBeforeMoving;
    bbool m_mustOffsetByRadius;
};

//
class Ray_EventWaveGeneratorNotification : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventWaveGeneratorNotification,Event,1312585498);
public:
    Ray_EventWaveGeneratorNotification() : m_waveType(0), m_scale(1), m_isColored(bfalse), m_isPreparing(bfalse) {}
    void setup(u32 _waveType, f32 _scale, bbool _isColored, bbool _isPreparing) {m_waveType = _waveType; m_scale = _scale; m_isColored = _isColored; m_isPreparing = _isPreparing;}
    u32 getWaveType() const {return m_waveType;}
    f32 getScale() const {return m_scale;}
    bbool getIsColored() const {return m_isColored;}
    bbool getIsPreparing() const {return m_isPreparing;}

private:
    u32 m_waveType;
    f32 m_scale;
    bbool m_isColored;
    bbool m_isPreparing;
};

class Ray_EventDynamicFogParamSet : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventDynamicFogParamSet,Event,2084909099);
    DECLARE_SERIALIZE()
public:
    Ray_EventDynamicFogParamSet() : m_fogColor(0), m_fogMaxDepth( 30.0f ) {}
    Ray_EventDynamicFogParamSet( u32 _color, f32 _maxDepth ){ m_fogColor =_color; m_fogMaxDepth = _maxDepth;  }

    ITF_INLINE u32             getFogColor() const              { return m_fogColor; }
    ITF_INLINE f32             getFogMaxDepth() const           { return m_fogMaxDepth; }
    ITF_INLINE void            setFogColor( u32 _fogColor )     { m_fogColor = _fogColor; }
    ITF_INLINE void            setFogMaxDepth( f32 _maxDepth )  { m_fogMaxDepth = _maxDepth; }
private :
    u32     m_fogColor;
    f32     m_fogMaxDepth;
};

class Ray_EventDynamicFogParamQuery : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventDynamicFogParamQuery,Event,2314851366);
public:
    Ray_EventDynamicFogParamQuery() : m_hasData(bfalse), m_fogColor(0), m_fogMaxDepth( 30.0f ) {}
    ITF_INLINE bbool           getFogParams( u32 & _res_Color, f32 & _res_MaxDepth ) const  { _res_Color = m_fogColor; _res_MaxDepth = m_fogMaxDepth; return m_hasData; }
    ITF_INLINE void            setFogParams( u32 _Color, f32 _MaxDepth )                    { m_fogColor = _Color; m_fogMaxDepth = _MaxDepth; m_hasData = btrue; }
private :
    u32     m_fogColor;
    f32     m_fogMaxDepth;
    bbool   m_hasData;
};

//
class Ray_EventQueryWindTunnelInfo : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryWindTunnelInfo,Event,3603812346);
    DECLARE_SERIALIZE()
public:

    Ray_EventQueryWindTunnelInfo() : m_startPoint(Vec2d::Zero), m_exitPoint(Vec2d::Zero), m_level(U32_INVALID) {}
    ~Ray_EventQueryWindTunnelInfo() {}

    void                    setStartPoint( const Vec2d& _start ) { m_startPoint = _start; }
    const Vec2d&            getStartPoint() const { return m_startPoint; }

    void                    setExitPoint( const Vec2d& _exit ) { m_exitPoint = _exit; }
    const Vec2d&            getExitPoint() const { return m_exitPoint; }

    void                    setLevel( u32 _level ) { m_level = _level; }
    u32                     getLevel() const { return m_level; }

private:

    Vec2d                   m_startPoint;
    Vec2d                   m_exitPoint;
    u32                     m_level;
};

class Ray_EventQueryGeyserBehavior : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryGeyserBehavior,Event,2840355382);
    DECLARE_SERIALIZE()        
public:

    Ray_EventQueryGeyserBehavior() : m_geyser(bfalse)  {}

    bbool                       getHasGeyserBehavior() const { return m_geyser; }
    void                        setHasGeyserBehavior( bbool _val ) { m_geyser = _val; } 

private:

    bbool                       m_geyser;
};

class Ray_EventRescueFriendly : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventRescueFriendly,Event,777320120);
    DECLARE_SERIALIZE()        
public:

    Ray_EventRescueFriendly() {}

private:

};

class Ray_EventQueryIsRescued : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryIsRescued,Event,3718040811);
    DECLARE_SERIALIZE()        
public:

    Ray_EventQueryIsRescued() : m_isRescued(bfalse) {}
    ~Ray_EventQueryIsRescued() {}

    bbool                   getIsRescued() const { return m_isRescued; }
    void                    setIsRescued( bbool _val ) { m_isRescued = _val; }

private:

    bbool                   m_isRescued;
};

class Ray_EventBreakableBreak : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventBreakableBreak,Event,4257664379);
    DECLARE_SERIALIZE()        
public:

    Ray_EventBreakableBreak() : m_break(bfalse) {}
    Ray_EventBreakableBreak( ObjectRef _sender, bbool _break ) : Super(_sender), m_break(_break) {}

    bbool                   getIsBreak() const { return m_break; }

private:

    bbool                   m_break;
};

class Ray_EventBreakableQuery : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventBreakableQuery,Event,4136568804);
    DECLARE_SERIALIZE()        
public:

    Ray_EventBreakableQuery() : m_broken(bfalse), m_isBreakable(bfalse), m_poly(ITF_INVALID_OBJREF) {}
    Ray_EventBreakableQuery( ObjectRef _sender ) : Super(_sender), m_broken(bfalse), m_isBreakable(bfalse), m_poly(ITF_INVALID_OBJREF) {}

    bbool                   getIsBroken() const { return m_broken; }
    void                    setIsBroken( bbool _val ) { m_broken = _val; }

    bbool                   getIsBreakable() const { return m_isBreakable; }
    void                    setIsBreakable( bbool _val ) { m_isBreakable = _val; }

    ObjectRef               getPolyline() const { return m_poly; }
    void                    setPolyline( ObjectRef _poly ) { m_poly = _poly; }

private:

    bbool                   m_broken;
    bbool                   m_isBreakable;
    ObjectRef               m_poly;
};

class Ray_EventHUDDisplayElem : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventHUDDisplayElem,Event,2571036057);
    DECLARE_SERIALIZE()        
public:

    Ray_EventHUDDisplayElem()
        : m_index(U32_INVALID)
        , m_bShow(bfalse)
    {}

    Ray_EventHUDDisplayElem( u32 _index, bbool _show ) { m_index = _index ; m_bShow = _show; }

    u32             getIndex() { return m_index; }
    u32             getShow() { return m_bShow; }

private:
    u32             m_index;
    bbool           m_bShow;
};

class Ray_EventHUDSetText : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventHUDSetText,Event,1340493890);
    DECLARE_SERIALIZE()        
public:

    Ray_EventHUDSetText() {}

    Ray_EventHUDSetText( String8 _friendlyName, String8 _text ) { m_friendlyName = _friendlyName ; m_text = _text; }

    const String8&             getFriendlyName() { return m_friendlyName; }
    const String8&             getText() { return m_text; }

private:
    String8         m_friendlyName;
    String8         m_text;
};

class Ray_EventSetSpikyFlower : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventSetSpikyFlower, Event,4136114510);
    DECLARE_SERIALIZE()
public:
    Ray_EventSetSpikyFlower() : m_circleIndex(0) {}

    u32 getCircleIndex() const {return m_circleIndex;}

private:
    u32 m_circleIndex;
};
class Ray_EventQueryOpen : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryOpen, Event,251146153);
    DECLARE_SERIALIZE()
public:
    Ray_EventQueryOpen() : m_open(bfalse) {}

    bbool isOpen() const {return m_open;}
    void setOpen(bbool _open) { m_open = _open; }
private:
    bbool m_open;
};

class Ray_EventAddSoftCollForce : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventAddSoftCollForce, Event,2624804693);
    DECLARE_SERIALIZE()
public:
    Ray_EventAddSoftCollForce() {}
    ITF_INLINE ITF::Vec2d getForce() const { return m_force; }
    ITF_INLINE void setForce(ITF::Vec2d val) { m_force = val; }

private:
    Vec2d m_force;
};

class Ray_EventScaleDoorTrigger : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventScaleDoorTrigger, Event,3012042865);
    DECLARE_SERIALIZE()
public:
    Ray_EventScaleDoorTrigger() : m_targetPos(Vec2d::Zero), m_scale(1.f), m_applyAll(bfalse) {}
    Ray_EventScaleDoorTrigger( ObjectRef _sender, ActorRef _targetActor, const Vec2d& _targetPos, f32 _scale, bbool _applyAll );
    ~Ray_EventScaleDoorTrigger() {}

    ActorRef                    getTargetActor() const { return m_targetActor; }
    f32                         getScale() const { return m_scale; }
    bbool                       getApplyAll() const { return m_applyAll; }
    Vec2d                       getTargetPos() const { return m_targetPos; }

private :

    ActorRef                    m_targetActor;
    Vec2d                       m_targetPos;
    f32                         m_scale;
    bbool                       m_applyAll;
};

class Ray_EventScaleFunnelTrigger : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventScaleFunnelTrigger, Event,2532379169);
    DECLARE_SERIALIZE()
public:
    Ray_EventScaleFunnelTrigger() : m_enter(bfalse), m_scale(0.f), m_exitPoint(Vec2d::Zero) {}
    Ray_EventScaleFunnelTrigger( ObjectRef _sender, bbool _enter, f32 _scale, const Vec2d& _exitPoint ) : Super(_sender), m_enter(_enter), m_scale(_scale), m_exitPoint(_exitPoint) {}
    ~Ray_EventScaleFunnelTrigger() {}

    bbool                       getEnter() const { return m_enter; }
    f32                         getScale() const { return m_scale; }
    const Vec2d&                getExitPoint() const { return m_exitPoint; }

private:

    bbool                       m_enter;
    f32                         m_scale;
    Vec2d                       m_exitPoint;
};

class Ray_EventScaleTunnelTrigger : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventScaleTunnelTrigger, Event,2369480741);
    DECLARE_SERIALIZE()
public:
    Ray_EventScaleTunnelTrigger() : m_scale(1.f), m_t(0.f) {}
    Ray_EventScaleTunnelTrigger( ObjectRef _sender, f32 _scale, f32 _t ) : Super(_sender), m_scale(_scale), m_t(_t) {}
    ~Ray_EventScaleTunnelTrigger() {}

    f32                         getScale() const { return m_scale; }
    f32                         getT() const { return m_t; }

private :

    f32                         m_scale;
    f32                         m_t;
};

class Ray_EventQueryPaintTrail : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryPaintTrail, Event,3721333601);
    DECLARE_SERIALIZE()
public:
    Ray_EventQueryPaintTrail() : m_paint(bfalse) {}
    ~Ray_EventQueryPaintTrail() {}

    bbool                       getPaint() const { return m_paint; }
    void                        setPaint(bbool _paint) { m_paint = _paint; }
private :
    bbool                       m_paint;
};

class Ray_EventCheckPedestalSupport : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventCheckPedestalSupport, Event,1578355683);
    DECLARE_SERIALIZE()
public:
    Ray_EventCheckPedestalSupport() : m_pedestalPos(Vec2d::Zero), m_triggerPedestal(bfalse) {}
    Ray_EventCheckPedestalSupport( ObjectRef _sender, const Vec2d& _pedestalPos ) : Super(_sender), m_pedestalPos(_pedestalPos), m_triggerPedestal(bfalse) {}
    ~Ray_EventCheckPedestalSupport() {}

    void                                setTriggerPedestal() { m_triggerPedestal = btrue; }
    bbool                               getTriggerPedestal() const { return m_triggerPedestal; }

    const Vec2d&                        getPedestalPos() const { return m_pedestalPos; }

private:

    Vec2d                               m_pedestalPos;
    bbool                               m_triggerPedestal;
};

class Ray_EventQueryPedestalInfo : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryPedestalInfo, Event,2107418028);
    DECLARE_SERIALIZE()
public:
    Ray_EventQueryPedestalInfo() : m_pedestalPos(Vec2d::Zero), m_valid(bfalse), m_allowFallFromMoveDown(btrue) {}
    Ray_EventQueryPedestalInfo( ObjectRef _sender ) : Super(_sender), m_pedestalPos(Vec2d::Zero), m_valid(bfalse), m_allowFallFromMoveDown(btrue) {}
    ~Ray_EventQueryPedestalInfo() {}

    void                                setPedestalPos( const Vec2d& _pos ) { m_pedestalPos = _pos; }
    const Vec2d&                        getPedestalPos() const { return m_pedestalPos; }

    void                                setValid( bbool _val ) { m_valid = _val; }
    bbool                               getValid() const { return m_valid; }

    void                                setAllowFallFromMoveDown( bbool _val ) { m_allowFallFromMoveDown = _val; }
    bbool                               getAllowFallFromMoveDown() const { return m_allowFallFromMoveDown; }

    void                                addObjectOnPedestal( ObjectRef _ref ) { if ( !m_objectsOnPedestal.full() ) { m_objectsOnPedestal.push_back(_ref); } }
    const FixedArray <ObjectRef,10>&    getObjectsOnPedestal() const { return m_objectsOnPedestal; }

private:

    FixedArray <ObjectRef,10>           m_objectsOnPedestal;
    Vec2d                               m_pedestalPos;
    bbool                               m_valid;
    bbool                               m_allowFallFromMoveDown;
};

class Ray_EventReleasePedestal : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventReleasePedestal, Event,4284763324);
    DECLARE_SERIALIZE()
public:
    Ray_EventReleasePedestal() {}
    Ray_EventReleasePedestal( ObjectRef _sender ) : Super(_sender) {}
    ~Ray_EventReleasePedestal() {}

private:

};

class Ray_EventQueryLumCount : public Event //queries how many lums are contained in an object - typically broadcasted to many objects, which increment counters depending on what they contain
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryLumCount, Event,3794901482);
public:
    Ray_EventQueryLumCount() : m_normalLumCount(0), m_lumKingCount(0) {}
    ~Ray_EventQueryLumCount() {}

    u32 getNormalLumCount() const {return m_normalLumCount;}
    u32 getLumKingCount() const {return m_lumKingCount;}

    void addNormalLums(u32 _increment) {m_normalLumCount += _increment;}
    void addLumKings(u32 _increment) {m_lumKingCount += _increment;}

private:
    u32 m_normalLumCount, m_lumKingCount;

};

class Ray_EventRegisterBubblePrizeHub : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventRegisterBubblePrizeHub,Event,3108685932);
};

class Ray_EventNotifyBubblePrizeHub : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventNotifyBubblePrizeHub,Event,3264814460);

public:
    Ray_EventNotifyBubblePrizeHub() : Super(), m_isCrushAttack(bfalse) {}

    ITF_INLINE bbool isCrushAttack() const { return m_isCrushAttack; }
    ITF_INLINE void setCrushAttack(bbool _val) { m_isCrushAttack = _val; }

private:
    bbool m_isCrushAttack;
};

class Ray_EventQueryHitType : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryHitType, Event,4146626226);

public:
    Ray_EventQueryHitType() : Super(), m_hitType(EPunchType(U32_INVALID)) {}

    ITF_INLINE const EPunchType& getHitType() const { return m_hitType; }
    ITF_INLINE void setHitType(const EPunchType& _val) { m_hitType = _val; }

private:
    EPunchType m_hitType;
};

class Ray_EventScoreGuageAddLum : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventScoreGuageAddLum, Event,2833738877);

public:
    Ray_EventScoreGuageAddLum() : m_numLums(0) {}

    ITF_INLINE u32 getNumLums() const { return m_numLums; }
    ITF_INLINE void setNumLums(u32 _val) { m_numLums = _val; }
private:
    u32 m_numLums;
};

class Ray_EventScoreSetup : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventScoreSetup, Event,427760862);

public:
    Ray_EventScoreSetup() : m_index(0) {}

    ITF_INLINE u32 getIndex() const { return m_index; }
    ITF_INLINE void setIndex(u32 _val) { m_index = _val; }

private:
    u32 m_index;
};

class Ray_EventShowElectoonToothScore : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventShowElectoonToothScore, Event,116023213);

public:
    Ray_EventShowElectoonToothScore() : m_show(bfalse), m_received(bfalse) {}

    ITF_INLINE bbool getShow() const {return m_show;}
    ITF_INLINE void setShow(bbool _show) {m_show = _show;}
    ITF_INLINE const Vec3d &getSourcePos() const {return m_sourcePos;}
    ITF_INLINE void setSourcePos(const Vec3d &_pos) {m_sourcePos = _pos;}
    ITF_INLINE bbool getReceived() const { return m_received; }
    ITF_INLINE void setReceived(bbool _val) { m_received = _val; }

private:
    bbool m_show;
    Vec3d m_sourcePos;
    bbool m_received;
};

class Ray_EventFetchElectoonToothScoreInfos : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventFetchElectoonToothScoreInfos, Event,3242513602);

public:
    Ray_EventFetchElectoonToothScoreInfos() : m_hasIconsInFlow(bfalse) {}

    bbool getHasIconsInFlow() const {return m_hasIconsInFlow;}
    void setHasIconsInFlow(bbool _hasIcons) {m_hasIconsInFlow = _hasIcons;}

private:
    bbool m_hasIconsInFlow;
};

class Ray_EventGoToWorldMap : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventGoToWorldMap, Event,3334813547);
    DECLARE_SERIALIZE()

public:
    Ray_EventGoToWorldMap()
        : m_saveGameState(btrue)
        , m_markCurrentLevelAsCompleted(bfalse)
    {}

    ITF_INLINE const bbool      getSaveGameState() const { return m_saveGameState; }
    ITF_INLINE const bbool      getMarkCurrentLevelAsCompleted() const { return m_markCurrentLevelAsCompleted; }

    ITF_INLINE void             setSaveGameState( bbool _save ) { m_saveGameState = _save; }
    ITF_INLINE void             setMarkCurrentLevelAsCompleted( bbool _complete ) { m_markCurrentLevelAsCompleted = _complete; }

private:
    
    bbool       m_saveGameState;
    bbool       m_markCurrentLevelAsCompleted;
};

class Ray_EventStoneManLaunch : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventStoneManLaunch, Event,3722106216);
    DECLARE_SERIALIZE()

public:
    Ray_EventStoneManLaunch() 
    : m_actionIndex(0) 
    {}

    ITF_INLINE u32 getActionIndex() const
    { 
        return m_actionIndex; 
    }
    ITF_INLINE void setActionIndex(const u32 _val) 
    { 
        m_actionIndex = _val; 
    }

private:
    u32 m_actionIndex;
};

class Ray_EventTakeTooth : public Event //The sender will be registered as the tooth
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventTakeTooth, Event,2760419748);
    DECLARE_SERIALIZE()
public:
    Ray_EventTakeTooth() {}
};

class Ray_EventQueryDisableAirControlInfo : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventQueryDisableAirControlInfo, Event,1557318448);
    DECLARE_SERIALIZE()

public:

    Ray_EventQueryDisableAirControlInfo()
        : m_disable(bfalse)
        , m_blend(bfalse)
        , m_amount(0.f)
        , m_duration(0.f)
        , m_allowHelico(btrue)
    {}
    ~Ray_EventQueryDisableAirControlInfo() {}

    bbool               getDisable() const { return m_disable; }
    void                setDisable( bbool _val ) { m_disable = _val; }

    bbool               getIsBlend() const { return m_blend; }
    void                setIsBlend( bbool _val ) { m_blend = _val; }

    f32                 getAmount() const { return m_amount; }
    void                setAmount( const f32 _value ) { m_amount = _value; }

    f32                 getDuration() const { return m_duration; }
    void                setDuration( f32 _val ) { m_duration = _val; }

    bbool               getAllowHelico() const { return m_allowHelico; }
    void                setAllowHelico( bbool _val ) { m_allowHelico = _val; }

private:

    bbool               m_disable;
    bbool               m_blend;
    bbool               m_allowHelico;
    f32                 m_amount;   // 0: no air control, 1: full air control
    f32                 m_duration; // disable air control for n seconds / blend from m_amount to 100% in n seconds
};

class Ray_EventReturnTeeth : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventReturnTeeth, Event,810560716);
    DECLARE_SERIALIZE()

public:

    enum Stage
    {
        STAGE_SURPRISE,
        STAGE_THROW,
        STAGE_CHEER,
        STAGE_FINISHED,
        STAGE_STOP,
    };

    Ray_EventReturnTeeth() : m_numTeeth(0), m_stage(STAGE_SURPRISE) {}
    Ray_EventReturnTeeth( u32 _numTeeth, Stage _stage ) : m_numTeeth(_numTeeth), m_stage(_stage) {}
    ~Ray_EventReturnTeeth() {}

    u32                 getNumTeeth() const { return m_numTeeth; }
    Stage               getStage() const { return m_stage; }

private:

    u32                 m_numTeeth;
    Stage               m_stage;
};

class Ray_EventHunterBulletLaunched : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventHunterBulletLaunched, Event,1767723773);
    DECLARE_SERIALIZE()

public:

    Ray_EventHunterBulletLaunched(){}
    ~Ray_EventHunterBulletLaunched() {}

    ITF_INLINE ITF::ActorRef getBuller() const { return m_buller; }
    ITF_INLINE void setBuller(ITF::ActorRef _val) { m_buller = _val; }

private:
    ActorRef m_buller;
};

class Ray_EventPlayAnimState : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventPlayAnimState, Event,4089549703);
    DECLARE_SERIALIZE()

public:

    Ray_EventPlayAnimState() {}
    Ray_EventPlayAnimState( const StringID& _anim ) : m_anim(_anim) {}
    ~Ray_EventPlayAnimState() {}

    const StringID&     getAnim() const { return m_anim; }

private:

    StringID            m_anim;
};

class Ray_EventDyingStoneMan : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventDyingStoneMan, Event,1834526668);

public:
    Ray_EventDyingStoneMan() {};
    Ray_EventDyingStoneMan(ObjectRef _sender) : Super(_sender) {}
    ~Ray_EventDyingStoneMan() {};

};

class Ray_EventWorldMapUnlock : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventWorldMapUnlock, Event,1931876316);
    DECLARE_SERIALIZE()

public:
    Ray_EventWorldMapUnlock()
        : m_changeCurrentLevelName(bfalse)
        , m_levelName(StringID::Invalid)
        , m_saveGameState(bfalse)
    {};
    Ray_EventWorldMapUnlock(ObjectRef _sender) : Super(_sender) {}
    ~Ray_EventWorldMapUnlock() {};

    const ITF_VECTOR<StringID>&     getUnlocks() { return m_unlocks; }
    void addUnlock(const StringID& _newId) { if (_newId.isValid()) m_unlocks.push_back(_newId); }

    ITF_INLINE const bbool          getChangeCurrentLevelName() const { return m_changeCurrentLevelName; }
    ITF_INLINE void                 setChangeCurrentLevelName(bbool _aBool) { m_changeCurrentLevelName = _aBool; }

    ITF_INLINE const StringID&      getLevelName() const { return m_levelName; }
    ITF_INLINE void                 setLevelName(const StringID& _name) { m_levelName = _name; }

    ITF_INLINE bbool                getSaveGameState() const { return m_saveGameState; }
    ITF_INLINE void                 setSaveGameState(bbool _save) { m_saveGameState = _save; }

private:
    ITF_VECTOR<StringID>        m_unlocks;
    bbool                       m_changeCurrentLevelName;
    StringID                    m_levelName;
    bbool                       m_saveGameState;
};

class Ray_EventEndSequence : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventEndSequence, Event,3991450529);
    DECLARE_SERIALIZE()

public:
    Ray_EventEndSequence(): m_playMovie(btrue) {};
    Ray_EventEndSequence(ObjectRef _sender) : Super(_sender), m_playMovie(btrue) {}
    ~Ray_EventEndSequence() {};

    ITF_INLINE bbool getPlayMovie() const { return m_playMovie; }
    ITF_INLINE void setPlayMovie(ITF::bbool _val) { m_playMovie = _val; }
private:
    bbool m_playMovie;
};

class Ray_EventFxScreen : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventFxScreen, Event, ITF_GET_STRINGID_CRC(Ray_EventFxScreen,362679351) )
    DECLARE_SERIALIZE()

public:
    Ray_EventFxScreen()
	: m_isStart(bfalse)
    {}

    ~Ray_EventFxScreen() {};

    ITF_INLINE bbool getIsStart() const { return m_isStart; }
    ITF_INLINE void  setIsStart(bbool _value) { m_isStart = _value; }
    ITF_INLINE void  setFadeType(const StringID& _type) {m_fadeType = _type;}
    ITF_INLINE const StringID &getFadeType() const {return m_fadeType;}

private:
	u32 m_isStart;
    StringID m_fadeType; //empty type is default
};

class Ray_EventWMDisplayWorldRecap : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventWMDisplayWorldRecap, Event, 554350578 )

public:
    Ray_EventWMDisplayWorldRecap()
        : m_worldTag(StringID::Invalid)
        , m_display(bfalse)
    {};

    ~Ray_EventWMDisplayWorldRecap() {};

    ITF_INLINE const bbool      getDisplay() const { return m_display; }
    ITF_INLINE void             setDisplay(bbool _display){ m_display = _display; }

    ITF_INLINE const StringID&  getWorldTag() const { return m_worldTag; }
    ITF_INLINE void             setWorldTag(const StringID& _worldTag) { m_worldTag = _worldTag; }

private:

    StringID        m_worldTag;
    bbool           m_display;
};

class Ray_EventWMOpenPaths : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventWMOpenPaths, Event,2918906348);
};

class Ray_MoveOnPolylineFromNautilus : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_MoveOnPolylineFromNautilus, Event,1238635980)

public:

    Ray_MoveOnPolylineFromNautilus() : m_distance(0.f) {}
    Ray_MoveOnPolylineFromNautilus( f32 _dist ) : m_distance(_dist) {}

    f32             getDistance() const { return m_distance; }

private:

    f32             m_distance;
};

class Ray_EventFetchWMOpenPaths : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventFetchWMOpenPaths, Event,2134285703);

public:
    Ray_EventFetchWMOpenPaths() : Super(), m_done(bfalse) {}
    virtual ~Ray_EventFetchWMOpenPaths() {}

    ITF_INLINE bbool isDone() const { return m_done; }
    ITF_INLINE void setDone() { m_done = btrue; }

private:
    bbool m_done;
};

class Ray_EventWMShowNewNotification : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventWMShowNewNotification, Event, 1272081763);

public:
    Ray_EventWMShowNewNotification() : Super(), m_show(bfalse) {}
    virtual ~Ray_EventWMShowNewNotification() {}

    ITF_INLINE bbool    show() const { return m_show; }
    ITF_INLINE void     setShow(bbool _show) { m_show = _show; }

private:
    bbool       m_show;
};

class Ray_EventWMShow : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventWMShow, Event, 2102286749);

public:
    Ray_EventWMShow() : Super(), m_show(bfalse) {}
    virtual ~Ray_EventWMShow() {}

    ITF_INLINE bbool    show() const { return m_show; }
    ITF_INLINE void     setShow(bbool _show) { m_show = _show; }

private:
    bbool       m_show;
};

class Ray_EventTriggerStartTimeAttack : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_EventTriggerStartTimeAttack, Event,3727680095)
    DECLARE_SERIALIZE()

public:
    Ray_EventTriggerStartTimeAttack() : m_start(bfalse) {}
    Ray_EventTriggerStartTimeAttack( bbool _start ) : m_start(_start) {}
    ~Ray_EventTriggerStartTimeAttack() {}

    bbool               getStart() const { return m_start; }

private:

    bbool               m_start;
};

// CASA::CTR::SFS : Add For Arena   
class EventHurtNotification : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(EventHurtNotification,Event,758119956);
    DECLARE_SERIALIZE()        
public:

    EventHurtNotification() : m_Attacker(ITF_INVALID_OBJREF),m_Victime(ITF_INVALID_OBJREF), m_Level(0)  {}
    EventHurtNotification(ObjectRef _Attacker, ObjectRef _Victime, u32 level) : m_Attacker(_Attacker), m_Victime(_Victime), m_Level(level) {}
    ObjectRef getAttacker() const {  return m_Attacker; }
    ObjectRef getVictime() const {  return m_Victime; }
    u32 getLevel() const { return m_Level; }
private:
    ObjectRef m_Attacker;
    ObjectRef m_Victime;
    u32       m_Level;
};    

class EventPickRewardNotification : public Event
{
    DECLARE_OBJECT_CHILD_RTTI(EventPickRewardNotification,Event,1782638909);
    DECLARE_SERIALIZE()        
public:

    EventPickRewardNotification() : m_Picker(ITF_INVALID_OBJREF)  {}
    EventPickRewardNotification(ObjectRef _Picker) : m_Picker(_Picker){}
    ObjectRef getPicker() const {  return m_Picker; }        
private:
    ObjectRef m_Picker;       
};
// CASA>

} // namespace ITF

#endif // _ITF_RAY_GAMEPLAYEVENTS_H_
