
#ifndef _ITF_PLAYTRAJECTORY_EVT_H_
#define _ITF_PLAYTRAJECTORY_EVT_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_TRAJECTORY_H_
#include "engine/animation/trajectory.h"
#endif //_ITF_TRAJECTORY_H_

#ifndef _ITF_ICAMERACONTROLLER_H_
#include "engine/display/ICameraController.h"
#endif //_ITF_ICAMERACONTROLLER_H_

namespace ITF
{

class Blob;
class Trajectory;
class PlayTrajectory_evtTemplate;
class View;

class PlayTrajectory_evt : public SequenceEventWithActor, public ICameraController
{
    DECLARE_OBJECT_CHILD_RTTI(PlayTrajectory_evt,SequenceEventWithActor,1403481053);

public:
    PlayTrajectory_evt              (                       );
    PlayTrajectory_evt              (  const Vec3d& _anchor );
    virtual ~PlayTrajectory_evt     (                       );

    static SequenceEvent *  create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template);

    void                    init                    (  const Vec3d& _pos    );
    void                    forceCurrentFrame       (   i32 _frame          );

    Trajectory*             getTrajectory() {return &m_trajectory;}
    virtual bbool           isAlwaysActive() { return btrue; }


    bbool                   isBindedActorOk (    );

    virtual void            onBecomeActive();
    virtual void            onBecomeInactive();
    virtual Vec3d           getCameraPosition() const ;
    virtual f32             getDepth() const { return 0.0f; }
    virtual void            prefetchResources(i32 _pos);

    PlayTrajectory_evtTemplate *getTemplate() { return (PlayTrajectory_evtTemplate *)m_template; }

private:
    void        applyPosOnCam     (const Vec3d& _pos);
    void        applyPosOnActor   (const Vec3d& _pos);
    void        applyRotOnCam     (const Vec3d& _rot);
    void        applyRotOnActor   (const Vec3d& _rot);
    void        applyScaleOnCam   (const Vec3d& _scale);
    void        applyScaleOnActor (const Vec3d& _scale);

    Trajectory  m_trajectory;

    // runtime only members
    Vec3d       m_basePos;
    Vec3d       m_targetInitialPos; 
    Vec3d       m_cameraPosition;

    View       *m_prefetchView;
};

class PlayTrajectory_evtTemplate : public SequenceEventWithActor_Template
{
    DECLARE_SERIALIZE()
    DECLARE_OBJECT_CHILD_RTTI(PlayTrajectory_evtTemplate,SequenceEventWithActor_Template,2715452934);

public:
    PlayTrajectory_evtTemplate              (                       );
    virtual ~PlayTrajectory_evtTemplate     (                       ) {}
    EvtType getType                         (                       )  const { return event_PlayTrajectory;}

    void                            postLoad            (           );

#ifdef ITF_SUPPORT_BLOBEVENT
    static  SequenceEvent_Template* createFromBlob  (Blob* _blob, SequencePlayerComponent_Template* _owner );
    virtual     void                serializeToBlob     (Blob* _blob);
    virtual     void                fillFromBlob        (Blob* _blob);
    
#endif //ITF_SUPPORT_BLOBEVENT
    virtual     bbool               fillFromActor       (Actor * _actor, const Vec3d &_pos);

    Trajectory_Template*            getTrajectoryTemplate   () {return &m_trajectory;}
    bbool                           isBindedToCamera        () const {return m_useCamera;}
    bbool                           isRestoreInitialPos     () const { return m_bRestoreInitialPos; }

    float                           getCoeffOneCamera       () const { return m_coeffOneCamera; }
    float                           getCoeffTwoCamera       () const { return m_coeffTwoCamera; }
    float                           getCoeffOneCameraOut    () const { return m_coeffOneCameraOut; }
    float                           getCoeffTwoCameraOut    () const { return m_coeffTwoCameraOut; }
    int                             getStartFrameCameraBlend() const { return m_startFrameCameraBlend; }

    // split return second trajectory obtained
    SequenceEvent_Template *        split( i32 _frame );
    bbool                           concat( SequenceEvent_Template * _evt);
    void                            globalPosMove( const Vec3d & _delta );

private:
    bbool                m_useCamera;
    Trajectory_Template  m_trajectory;
    bbool                m_bRestoreInitialPos;

    // for blending camera
    float       m_coeffOneCamera;
    float       m_coeffTwoCamera;
    float       m_coeffOneCameraOut;
    float       m_coeffTwoCameraOut;
    int         m_startFrameCameraBlend;
};


} // namespace ITF

#endif // _ITF_PLAYTRAJECTORY_EVT_H_

