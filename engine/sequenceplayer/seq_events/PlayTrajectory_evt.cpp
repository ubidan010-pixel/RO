#include "precompiled_engine.h"

#ifndef _ITF_PLAYTRAJECTORY_EVT_H_
#include "engine/sequenceplayer/seq_events/PlayTrajectory_evt.h"
#endif //_ITF_PLAYTRAJECTORY_EVT_H_

#ifndef _ITF_LUAHANDLER_H_
#include "core/script/LUAHandler.h"
#endif //_ITF_LUAHANDLER_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_PHYSCOMPONENT_H_
#include "engine/actors/components/physcomponent.h"
#endif //_ITF_PHYSCOMPONENT_H_

#ifndef _ITF_PARTICLEGENERATORCOMPONENT_H_
#include "engine/actors/components/ParticleGeneratorComponent.h"
#endif //_ITF_PARTICLEGENERATORCOMPONENT_H_

#ifndef _ITF_TRAJECTORY_H_
#include "engine/animation/trajectory.h"
#endif //_ITF_TRAJECTORY_H_

#ifndef _ITF_SEQUENCEPLAYERCOMPONENT_H_
#include "engine/sequenceplayer/SequencePlayerComponent.h"
#endif //_ITF_SEQUENCEPLAYERCOMPONENT_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_SCENE_MANAGER_H_
#include "engine/scene/sceneManager.h"
#endif //_ITF_SCENE_MANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_CAMERACONTROLLERMANAGER_H_
#include "engine/actors/components/Camera/CameraControllerManager.h"
#endif // _ITF_CAMERACONTROLLERMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_


namespace ITF
{
IMPLEMENT_OBJECT_RTTI(PlayTrajectory_evt)

PlayTrajectory_evt::PlayTrajectory_evt(const Vec3d& _anchor)
{
    m_trajectory.setPos(_anchor);
    m_trajectory.setBindedEvent(this);
    m_prefetchView = NULL;
}

PlayTrajectory_evt::PlayTrajectory_evt()
{
    m_trajectory.setBindedEvent(this);
    m_prefetchView = NULL;
}

SequenceEvent * PlayTrajectory_evt::create(SequencePlayerComponent* _owner, SequenceEvent_Template *_template)
{
    PlayTrajectory_evt *pNew  = newAlloc(mId_Gameplay, PlayTrajectory_evt());
    pNew->m_ownerSequence       = _owner;
    pNew->m_template            = _template;
    pNew->m_trajectory.init(((PlayTrajectory_evtTemplate *)_template)->getTrajectoryTemplate());

    return pNew;
}


PlayTrajectory_evt::~PlayTrajectory_evt()
{    
    if(!m_ownerSequence)
        return;
    
    if  (getActive())
    {
        onBecomeInactive();
    }

    if (m_prefetchView)
    {
        View::deleteView(m_prefetchView);
        m_prefetchView = NULL;
    }
}


void    PlayTrajectory_evt::init(const Vec3d& _pos)
{
    m_basePos = _pos;
    m_trajectory.setAllowedCell(bfalse);
    m_trajectory.setPos(_pos);
    if (!getTemplate()->isBindedToCamera())
    {
        Actor* actor = getBindedActor();
        if (actor)
            m_targetInitialPos = actor->getPos();      
    }
    // event bind camera
    else
    {
        m_targetInitialPos = CAMERA->getPos();
    }
}


void    PlayTrajectory_evt::forceCurrentFrame(   i32 _frame  )
{
    if(!m_active) 
        onBecomeActive();

    PlayTrajectory_evtTemplate * evtTemplate = getTemplate();
    const Trajectory_Template *  trajTemplate = evtTemplate->getTrajectoryTemplate();

    bbool needRecomputeLocalPos = bfalse;
    if (!evtTemplate->isBindedToCamera())
    {
        Actor * act = getBindedActor();
        if (act)
        {
            Actor * parent =  act->getParent().getActor();
            if (parent)
            {
                m_trajectory.setPos(parent->getPos());
                needRecomputeLocalPos = btrue;
            }
        }
    }

    // update time
    f32 localFrame = static_cast<f32>(_frame - evtTemplate->getStartFrame());
    {
        // get trajectory pos for this time
        Vec3d pos;
        if (trajTemplate->computeGlobalPosAtTime(localFrame, pos, m_trajectory.getPos()))
        {
            if ( evtTemplate->isBindedToCamera()) 
            {
                if (m_ownerSequence->isCameraControlAllowed())
                    applyPosOnCam(pos);
            }
            else
                applyPosOnActor(pos);
        }
    }

    {
        // get trajectory rot for this time
        Vec3d rot;
        if (trajTemplate->computeRotAtTime(localFrame, rot))
        {
            if (evtTemplate->isBindedToCamera())
            {
                if (m_ownerSequence->isCameraControlAllowed())
                   applyRotOnCam(rot);
            }
            else
                applyRotOnActor(rot);
        }
    }

    {
        // get trajectory scale for this time
        Vec3d scale;
        if (trajTemplate->computeScaleAtTime(localFrame, scale))
        {
            if (evtTemplate->isBindedToCamera())
            {
                if (m_ownerSequence->isCameraControlAllowed())
                    applyScaleOnCam(scale);
            }
            else
                applyScaleOnActor(scale);
        }
    }

    if (needRecomputeLocalPos)
    {
        Actor * act = getBindedActor();
        if (act)
        {
            act->setBoundWorldPos(act->getPos());
            act->setBoundWorldAngle(act->getAngle());
        }
    }

    if (m_actorPlayerIndex != U32_INVALID)
    {
        // update actor data ...
        Actor * act = getBindedActor();
        if (act && m_actorPlayerIndex < CINEMATICMANAGER->getPlayersInfoCount())
        {
            SequencePlayers & seqPlayer = CINEMATICMANAGER->getPlayersInfoAtIndex(m_actorPlayerIndex);
            // conserve z pos ... for the moment ...
            seqPlayer.m_pos      = act->get2DPos().to3d(seqPlayer.m_pos.m_z);
            seqPlayer.m_angle    = act->getAngle();
        }
    }

#ifndef ITF_FINAL
    if (evtTemplate->isBindedToCamera() && CAMERACONTROLLERMANAGER->isPauseMode() && CAMERA->getMode() == Camera::Camera_InGame)
    {
        GFX_ADAPTER->setCamera(m_cameraPosition.m_x, m_cameraPosition.m_y, m_cameraPosition.m_z, CAMERA);
    }
#endif
}


void PlayTrajectory_evt::applyPosOnCam   (const Vec3d& _pos)
{
    m_cameraPosition = _pos;
}


void PlayTrajectory_evt::applyPosOnActor (const Vec3d& _pos)
{
    if (Actor* actor = getBindedActor())
        actor->setPos(_pos);
}

void PlayTrajectory_evt::applyRotOnCam   (const Vec3d& _rot)
{
    CAMERA->setRotateZAngle(_rot.m_z);
}

void PlayTrajectory_evt::applyRotOnActor (const Vec3d& _rot)
{
    if (Actor* actor = getBindedActor())
        actor->setAngle(_rot.m_z);
}

void PlayTrajectory_evt::applyScaleOnCam   (const Vec3d& _scale) 
{
    //NOT SET YET
}

void PlayTrajectory_evt::applyScaleOnActor (const Vec3d& _scale) 
{
    Actor* actor = getBindedActor();
    if (actor)
    {
        actor->setScale(_scale.truncateTo2D());
    }
}

Vec3d PlayTrajectory_evt::getCameraPosition() const
{
    return m_cameraPosition;
}

void    PlayTrajectory_evt::onBecomeActive()
{
    Super::onBecomeActive();
    PlayTrajectory_evtTemplate *evtTemplate = getTemplate();

    if (evtTemplate->isBindedToCamera())
    {
        CAMERACONTROLLERMANAGER->registerCameraController(this);
        if (CAMERACONTROLLERMANAGER->isPauseMode())
        {
            // isPauseMode is set on propagate in Edition mode
            CAMERACONTROLLERMANAGER->setMainCameraController(this, 1.f, 1.f);
        } else
        {
            CAMERACONTROLLERMANAGER->setMainCameraController(this, evtTemplate->getCoeffOneCamera(), evtTemplate->getCoeffTwoCamera());
        }
        CINEMATICMANAGER->controlCamera();
    }
    else
    {
        Actor* actor = getBindedActor();   

        if (actor)
        {
            m_ownerSequence->controlActor(actor->getRef(), evtTemplate->getDisableComponentsActor());
        }
    }
}


void    PlayTrajectory_evt::onBecomeInactive()
{
    Super::onBecomeInactive();

    PlayTrajectory_evtTemplate *evtTemplate = getTemplate();
    if (evtTemplate->isBindedToCamera())
    {
        if (CAMERACONTROLLERMANAGER->isPauseMode())
        {
            CAMERACONTROLLERMANAGER->removeMainCameraController(this, 1.f, 1.f);
        } else
        {
            CAMERACONTROLLERMANAGER->removeMainCameraController(this, evtTemplate->getCoeffOneCameraOut(), evtTemplate->getCoeffTwoCameraOut());
        }
        CINEMATICMANAGER->releaseCamera(bfalse);
        CAMERACONTROLLERMANAGER->unregisterCameraController(this);
    }
    else
    {
        Actor* actor = getBindedActor();
        if (actor)
        {
            m_ownerSequence->releaseActor(actor->getRef(), evtTemplate->getDisableComponentsActor());
        }
    }

    if (m_prefetchView)
    {
        View::deleteView(m_prefetchView);
        m_prefetchView = NULL;
    }
}

ITF::bbool PlayTrajectory_evt::isBindedActorOk()
{
    return (getBindedActor() != NULL) || getTemplate()->isBindedToCamera();
}

void PlayTrajectory_evt::prefetchResources(i32 _pos)
{
    Super::prefetchResources(_pos);
    
    if (getTemplate()->isBindedToCamera())
    {
        if (!m_prefetchView)
        {
            m_prefetchView = View::createView("PlayTrajectory_evt", bfalse, bfalse);
            m_prefetchView->createCamera();
            CAMERA->copyTo(*m_prefetchView->getCamera());
        }

        m_prefetchView->setActive(btrue);
        Camera * cam = m_prefetchView->getCamera();
        ITF_ASSERT(cam);
        if (cam)
        {
            // We consider that the camera is NOT binded !
            PlayTrajectory_evtTemplate * evtTemplate = getTemplate();
            const Trajectory_Template *  trajTemplate = evtTemplate->getTrajectoryTemplate();

            f32     localFrame = static_cast<f32>(_pos - evtTemplate->getStartFrame());
            Vec3d   pos;
            if (trajTemplate->computeGlobalPosAtTime(localFrame, pos, m_trajectory.getPos()))
            {
                cam->forcePosition(pos);
            }
            cam->startFrame();
        }
    }
}


/////////////////////////////////////////////////////
/// TEMPLATE
/////////////////////////////////////////////////////

IMPLEMENT_OBJECT_RTTI(PlayTrajectory_evtTemplate)

BEGIN_SERIALIZATION_CHILD(PlayTrajectory_evtTemplate)
    SERIALIZE_MEMBER("RestoreInitialPos",m_bRestoreInitialPos);
    SERIALIZE_OBJECT("Keyframes",m_trajectory);
    SERIALIZE_MEMBER("startFrameCameraBlend", m_startFrameCameraBlend);
    SERIALIZE_MEMBER("coeffOneCamera", m_coeffOneCamera);
    SERIALIZE_MEMBER("coeffTwoCamera", m_coeffTwoCamera);
    SERIALIZE_MEMBER("coeffOneCameraOut", m_coeffOneCameraOut);
    SERIALIZE_MEMBER("coeffTwoCameraOut", m_coeffTwoCameraOut);
    SERIALIZE_FUNCTION(postLoad,ESerialize_Data_Load);
END_SERIALIZATION()

PlayTrajectory_evtTemplate::PlayTrajectory_evtTemplate()
: m_startFrameCameraBlend(bfalse)
, m_coeffOneCamera(1.f)
, m_coeffTwoCamera(1.f)
, m_coeffOneCameraOut(0.f)
, m_coeffTwoCameraOut(0.f)
, m_useCamera(bfalse)
, m_bRestoreInitialPos(btrue)
{

}

void PlayTrajectory_evtTemplate::postLoad()
{
    m_useCamera = !strcmp(getObjectPath().getStrId(), SequencePlayerComponent::getCameraKey());
    m_trajectory.setBindedEventTemplate(this);
    if (m_coeffOneCameraOut == 0.f || m_coeffTwoCameraOut == 0.f)
    {
        m_coeffOneCameraOut = m_coeffOneCamera;
        m_coeffTwoCameraOut = m_coeffTwoCamera;
    }
}

#ifdef ITF_SUPPORT_BLOBEVENT
SequenceEvent_Template*   PlayTrajectory_evtTemplate::createFromBlob( Blob* _blob, SequencePlayerComponent_Template* _owner )
{
    PlayTrajectory_evtTemplate* pNew = new PlayTrajectory_evtTemplate(/*_anchor*/);
    pNew->setOwnerSequence(_owner);
    pNew->fillFromBlob(_blob);
    pNew->m_trajectory.setBindedEventTemplate(pNew);

    return pNew;
}

void PlayTrajectory_evtTemplate::serializeToBlob (Blob* _blob)
{
    serializeCommonToBlob(_blob);

    if (m_useCamera)
        m_objectPath.setStrId(SequencePlayerComponent::getCameraKey());
    serializeFriendlyToBlob(_blob);

    // restore initial position
    _blob->pushBool(m_bRestoreInitialPos);          // restore initial position

    // sort
    m_trajectory.sortPosByTime();
    m_trajectory.sortRotByTime();
    m_trajectory.sortScaleByTime();

    // trajectory
    m_trajectory.serializeToBlob (_blob);           //trajectory

    // camera
    _blob->pushInt32(m_startFrameCameraBlend);
    _blob->pushFloat32(m_coeffOneCamera);
    _blob->pushFloat32(m_coeffTwoCamera);
    _blob->pushFloat32(m_coeffOneCameraOut);
    _blob->pushFloat32(m_coeffTwoCameraOut);
}

void PlayTrajectory_evtTemplate::fillFromBlob(Blob* _blob)
{
    fillCommonFromBlob(_blob);
    fillFriendlyFromBlob(_blob);


    m_useCamera = !strcmp(getObjectPath().getStrId(), SequencePlayerComponent::getCameraKey());
    m_bRestoreInitialPos = _blob->extractBool();

    // trajectory
    m_trajectory.fillFromBlob(_blob);

    // sort
    m_trajectory.sortPosByTime();
    m_trajectory.sortRotByTime();
    m_trajectory.sortScaleByTime();

    m_startFrameCameraBlend = _blob->extractInt32();
    m_coeffOneCamera =_blob->extractFloat32();
    m_coeffTwoCamera =_blob->extractFloat32();
    m_coeffOneCameraOut =_blob->extractFloat32();
    m_coeffTwoCameraOut =_blob->extractFloat32();
}

#endif //ITF_SUPPORT_BLOBEVENT

bbool PlayTrajectory_evtTemplate::fillFromActor(Actor * _actor, const Vec3d & _pos)
{
    ITF_ASSERT(m_trajectory.isEmpty());
    if (!m_trajectory.isEmpty())
        return bfalse;

    if (m_useCamera)
    {
        // insert target's current state as first key (time 0)
        m_trajectory.addGlobalPosData(CAMERA->getPos(), _pos, Spline::m_defaultInterpolation, 0.0f);
        m_trajectory.addRotData(Vec3d::Zero, Spline::m_defaultInterpolation, 0.0f);
        m_trajectory.addScaleData(Vec3d(1.f, 1.f, 0.f), Spline::m_defaultInterpolation, 0.0f);
        return btrue;
    }

    if (_actor )
    {
        Vec3d   truePos = _pos;
        Actor * parent  =  _actor->getParent().getActor();
        if (parent)
        {
            truePos = parent->getPos();
        }

        // insert target's current state as first key (time 0)
        m_trajectory.addGlobalPosData(_actor->getPos(), truePos, Spline::m_defaultInterpolation, 0.0f);
        m_trajectory.addRotData(Vec3d(0.f, 0.f, _actor->getAngle()), Spline::m_defaultInterpolation, 0.0f);
        m_trajectory.addScaleData(Vec3d(_actor->getScale().m_x, _actor->getScale().m_y, 0.f), Spline::m_defaultInterpolation, 0.0f);
        return btrue;
    }

    return btrue;
}

SequenceEvent_Template  * PlayTrajectory_evtTemplate::split( i32 _frame )
{
    i32 localFrame = _frame - getStartFrame();
    f32 fLocalFrame = static_cast<f32>(localFrame);
    
    if (fLocalFrame <= 0 || fLocalFrame >= getStopFrame())
        return NULL;

    PlayTrajectory_evtTemplate  * newTrajectoty = newAlloc(mId_System, PlayTrajectory_evtTemplate());
    BinaryClone(this, newTrajectoty);
    
    Trajectory_Template * beginTraj = &m_trajectory;
    Trajectory_Template* endTraj    = &newTrajectoty->m_trajectory;

    bbool somethingDone = bfalse;
    for (u32 trajDataIdx=0; trajDataIdx<3; trajDataIdx++)
    {
        Trajectory_Template::TrajectoryDataType trajType = (Trajectory_Template::TrajectoryDataType)trajDataIdx;

        u32 dataCount    = beginTraj->getCount(trajType);
        u32 dataCut      = 0;
        if (!dataCount)
            continue;

        somethingDone = btrue;

        for (; dataCut < dataCount; dataCut++)
        {
            if (beginTraj->getDataTimeAt(trajType, dataCut) >= fLocalFrame)
                break;
        }
        if (dataCut < dataCount && beginTraj->getDataTimeAt(trajType, dataCut) == fLocalFrame)
        {
            ITF_VERIFY(beginTraj->extractData(trajType, 0, dataCut+1, 0.f));
        } else
        {
            ITF_VERIFY(beginTraj->extractData(trajType, 0, dataCut, 0.f));
        }
        if (dataCut >= dataCount)
        {
            endTraj->extractData(trajType, dataCount-1, dataCount, -beginTraj->getDataTimeAt(trajType, dataCount-1));
        } else
        {
            endTraj->extractData(trajType, dataCut, dataCount, -fLocalFrame);
        }
    }

    if (!somethingDone)
    {
        SF_DEL(newTrajectoty);
        return NULL;
    }

    newTrajectoty->setDuration(getDuration() - localFrame);
    newTrajectoty->setStartFrame(getStartFrame() + localFrame);
    
    setDuration(localFrame);

    return newTrajectoty;
}

bbool PlayTrajectory_evtTemplate::concat( SequenceEvent_Template * _evt )
{
    if (_evt->getType() != getType())
        return bfalse;

    PlayTrajectory_evtTemplate *traj_Event = (PlayTrajectory_evtTemplate *)_evt;
    Trajectory_Template * beginTraj = &m_trajectory;
    Trajectory_Template* endTraj    = &traj_Event->m_trajectory;

    f32 startOffset = (f32)(traj_Event->getStartFrame() - getStartFrame());
    for (u32 trajDataIdx=0; trajDataIdx<3; trajDataIdx++)
    {
        Trajectory_Template::TrajectoryDataType trajType = (Trajectory_Template::TrajectoryDataType)trajDataIdx;

        u32 dataCount    = endTraj->getCount(trajType);
        for (u32 dataIdx = 0; dataIdx < dataCount; ++dataIdx)
        {
            beginTraj->addData(trajType,
                endTraj->getDataAt(trajType, dataIdx, Vec3d::Zero), Vec3d::Zero,
                endTraj->getDataInterpAt(trajType, dataIdx),
                endTraj->getDataTimeAt(trajType, dataIdx) + startOffset);
        }
    }
    setDuration(traj_Event->getStopFrame() - getStartFrame());
    return btrue;
}

void PlayTrajectory_evtTemplate::globalPosMove( const Vec3d & _delta )
{
    u32 count = m_trajectory.getPosCount();
    // find if a key exist at current frame
    for (u32 i = 0; i < count; i++)
    {
        m_trajectory.setLocalPosDataAt(m_trajectory.getLocalPosAt(i) + _delta, m_trajectory.getPosInterpAt(i), m_trajectory.getPosTimeAt(i), i);
    }    
}

} // namespace ITF


