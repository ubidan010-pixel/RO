#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_LIGHTNINGPATCHAICOMPONENT_H_
#include "Rayman/gameplay/Components/AI/Ray_LightningPatchAIComponent.h"
#endif //_ITF_RAY_LIGHTNINGPATCHAICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_LightningPatchAIComponent)

        BEGIN_SERIALIZATION_CHILD(Ray_LightningPatchAIComponent)
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_LightningPatchAIComponent)
        //VALIDATE_COMPONENT_PARAM("", m_curveComponent,  "Ray_LightningPatchAIComponent requires an CurveComponent.");
        END_VALIDATE_COMPONENT()

        //*****************************************************************************

        Ray_LightningPatchAIComponent::Ray_LightningPatchAIComponent()
        : m_animComponent(NULL)
        , m_boneIndex(U32_INVALID)
        , m_curDist(0.0f)
        , m_drawDist(0.0f)
        , m_timer(0.0f)
        , m_headIndex(0)
        , m_tailIndex(0)
        , m_bodyIndex(0)
        , m_headState(State_None)
        , m_tailState(State_None)
        , m_bodyState(State_None)
        , m_start(0.0f,0.0f,0.0f)
        , m_stop(0.0f,0.0f,0.0f)
        , m_effectBoneIndex(U32_INVALID)
        , m_effectStart(0.0f,0.0f,0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_LightningPatchAIComponent::~Ray_LightningPatchAIComponent()
    {
        // None
    }

    //*****************************************************************************

    void Ray_LightningPatchAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHitSuccessful,818883013),this);
        m_bezierCurve.addPoint(Vec3d(0.0f,0.0f,0.0f),Vec3d(10.0f,0.0f,0.0f),Vec3d(10.0f,0.0f,0.0f));
        m_bezierCurve.addPoint(Vec3d(10.0f,0.0f,0.0f),Vec3d(10.0f,0.0f,0.0f),Vec3d(10.0f,0.0f,0.0f));

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_bezierCurve.buildEdges(10);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);

        m_renderer.onLoaded(&getTemplate()->getBezierRenderer(), m_actor->getResourceGroup());

        m_head.initialize(m_actor->getResourceGroup(),getTemplate()->getHeadPath(),1);
        m_tail.initialize(m_actor->getResourceGroup(),getTemplate()->getTailPath(),1);

        m_headState= State_None;
        m_bodyState= State_None;
        m_tailState= State_None;

        m_tailIndex = 0;
        m_headIndex = 0;
        m_bodyIndex = 0;

    }

    //*****************************************************************************

    void Ray_LightningPatchAIComponent::Update( f32 _dt ) 
    {
        if (m_effectBoneIndex == U32_INVALID)
        {
            m_effectBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getEffectBoneName());
        }
        else
        {
            AnimBoneDyn * bone = m_animComponent->getBone(m_effectBoneIndex);
            {
                if (bone)
                {
                    m_effectStart = bone->m_Pos.to3d(m_actor->getDepth());
                }
            }
        }

        if (m_boneIndex != U32_INVALID)
        {
            AnimBoneDyn * bone = m_animComponent->getBone(m_boneIndex);
            {
                if (bone)
                {
                    m_timer = m_timer + getTemplate()->getPlayRate() * _dt; 

                    u32 add = (u32)(m_timer/_dt);
                    if (add > 0)
                        m_timer = f32_Modulo(m_timer,f32(add) * _dt);

                    updateTail(add);
                    updateHead(add);

                    if (m_headState != State_None|| m_tailState != State_None || m_bodyState != State_None)
                        m_start = bone->m_Pos.to3d(m_actor->getDepth());
                    Vec2d dir = bone->getDirection();
                    if (m_bodyState == State_Anticipation || m_bodyState == State_Loop)
                    {
                        m_curDist = getRayCastDist(m_start.truncateTo2D(), dir, m_curDist);
                        m_curDist = Min(getTemplate()->getMaxLength(),m_curDist + _dt * getTemplate()->getSpeed());
                        m_stop = m_start + dir.to3d() * m_curDist;
                    }

                    m_drawDist = m_curDist;

                    updateBody(add);
                }
            }
        }
        else
        {
            m_boneIndex = m_animComponent->getBoneIndex(getTemplate()->getBoneName());
        }

        Super::Update(_dt);
    }


    void Ray_LightningPatchAIComponent::updateTail(u32 _add)
    {
        if (m_tailState == State_Anticipation)
        {
            if (m_tailIndex == U32_INVALID)
            {
                m_tailIndex = 0;
            }
            else if (m_tailIndex >= getTemplate()->getTailLoopStart())
            {
                m_tailState = State_Loop;
            }
            else
            {
                m_tailIndex += _add;
            }
        }
        else if (m_tailState == State_Loop)
        {
            if (getTemplate()->getTailLoopStart() == getTemplate()->getTailLoopStop())
            {
                m_tailIndex = getTemplate()->getTailLoopStart();
            }
            else
            {
                m_tailIndex = getTemplate()->getTailLoopStart() + ((m_tailIndex - getTemplate()->getTailLoopStart() + _add) % (getTemplate()->getTailLoopStop() - getTemplate()->getTailLoopStart()));
            }
        }
        else if (m_tailState == State_End)
        {
            if (m_tailIndex + 1 >= getTemplate()->getTailMaxIndices())
            {
                m_tailState = State_None;
            }
            else

            {
                m_tailIndex += _add;
            }
        }
    }
    void Ray_LightningPatchAIComponent::updateHead(u32 _add)
    {

        if (m_headState == State_Anticipation)
        {
            if (m_headIndex == U32_INVALID)
            {
                m_headIndex = 0;
            }
            else if (m_headIndex >= getTemplate()->getHeadLoopStart())
            {
                m_headState = State_Loop;
            }
            else
            {
                m_headIndex += _add;
            }
        }
        else if (m_headState == State_Loop)
        {
            m_headIndex = getTemplate()->getHeadLoopStart() + ((m_headIndex - getTemplate()->getHeadLoopStart() + _add) % (getTemplate()->getHeadLoopStop() - getTemplate()->getHeadLoopStart()));
        }
        else if (m_headState == State_End)
        {
            if (m_headIndex + 1>= getTemplate()->getHeadMaxIndices())
            {
                m_headState = State_None;
            }
            else

            {
                m_headIndex += _add;
            }
        }
    }
    void Ray_LightningPatchAIComponent::updateBody(u32 _add)
    {
        if (m_bodyState != State_None)
        {
            if (m_bodyState == State_Anticipation)
            {
                if (m_bodyIndex == U32_INVALID)
                {
                    m_bodyIndex = 0;
                }
                else if (m_bodyIndex >= getTemplate()->getBodyLoopStart())
                {
                    m_bodyState = State_Loop;
                }
                else
                {
                    m_bodyIndex += _add;
                }
            }
            else if (m_bodyState == State_Loop)
            {
                m_bodyIndex = getTemplate()->getBodyLoopStart() + ((m_bodyIndex - getTemplate()->getBodyLoopStart() + _add) % (getTemplate()->getBodyLoopStop() - getTemplate()->getBodyLoopStart()));
            }
            else if (m_bodyState == State_End)
            {
                if (m_bodyIndex + 1 >= getTemplate()->getBodyMaxIndices())
                {
                    m_bodyState = State_None;
                }
                else

                {
                    m_bodyIndex += _add;
                }
            }
        }
    }



    f32 Ray_LightningPatchAIComponent::getRayCastDist(const Vec2d & _pos, const Vec2d & _dir, f32 _dist)
    {

        PhysContactsContainer envTouchedContacts;
        PhysShapePolygon polygon;
        Vec2d points[2];

        const Vec2d end = _dir * _dist;


        points[0] = Vec2d::Zero;//halfWidth;
        points[1] = end;// + halfWidth;

        polygon.setPoints(points,2);


        PunchStim * stim = STIMSMANAGER->requestStim<PunchStim>(&polygon);
        if (stim)
        {
            stim->setReceivedHitType(RECEIVEDHITTYPE_FRONTPUNCH);
            stim->setIsRadial(bfalse);
            stim->setDirection(_dir);
            stim->setLevel(1);
            stim->setFaction(RAY_FACTION_ROBOT);

            stim->setSender(m_actor->getRef());
            stim->setAngle(0.0f);
            stim->setPos(_pos);
            stim->setPrevPos(_pos);
            STIMSMANAGER->sendStim(stim);
        }


        PHYSWORLD->checkEncroachment( _pos, _pos, 0.0f, &polygon,ECOLLISIONFILTER_ENVIRONMENT, m_actor->getDepth(), envTouchedContacts );
        PHYSWORLD->collidePhantoms(_pos,_pos,0.0f,
            &polygon,m_actor->getDepth(),ECOLLISIONFILTER_ALL,envTouchedContacts);


        i32 contactIndex = -1;
        f32 time = FLT_MAX;
        f32 contactDist = _dist;
        for ( u32 i = 0; i < envTouchedContacts.size(); i++ )
        {
            const SCollidableContact& contact = envTouchedContacts[i];

            if (  contact.m_collidableUserData == m_actor->getRef() )
            {
                envTouchedContacts.eraseKeepOrder(i);
                i--; 
                continue;
            }

            Actor * actor = ActorRef(contact.m_collidableUserData).getActor();
            if (actor)
            {
                EventQueryIsDead query;
                actor->onEvent(&query);
                if (query.getIsDead())
                {
                    envTouchedContacts.eraseKeepOrder(i);
                    i--;
                    continue;
                }

            }

            if ( contact.m_edgeIndex != U32_INVALID )
            {
                PolyLine* poly(NULL);
                const PolyLineEdge* edge(NULL);
                AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);

                if ( poly && ( poly->getOwnerActor() == m_actor ) )
                {
                    envTouchedContacts.eraseKeepOrder(i);
                    i--; 
                    continue;
                }                


            }
            if (contact.m_t < time)
            {
                time = contact.m_t;
                contactIndex = i;
                contactDist = (contact.m_contactPoint - _pos).norm();
            }
        }		

        if ( contactIndex != -1 )
        {   
            return contactDist;
        }


        return _dist;


    }
    void Ray_LightningPatchAIComponent::Draw()
    {
        Super::Draw();
            if (m_boneIndex != U32_INVALID)
            {
                AnimBoneDyn * bone = m_animComponent->getBone(m_boneIndex);
                if (bone)
                {
                    BezierCurve::Point & point0 = m_bezierCurve.getPointAt(0);
                    point0.m_pos = m_start;
                    point0.m_tanA = m_stop;
                    point0.m_tanB = m_stop;

                    BezierCurve::Point & point1 = m_bezierCurve.getPointAt(1);

                    point1.m_pos = m_stop;
                    point1.m_tanA = m_stop;
                    point1.m_tanB = m_stop;

                    m_bezierCurve.buildEdges(10);


                    Vec2d dir = (m_stop - m_effectStart).truncateTo2D();
                    dir.normalize();

                    f32 angle = dir.getAngle();

                    if (m_bodyState != State_None)
                    {
                        f32 t = 1.0f / f32(getTemplate()->getBodyMaxIndices());

                        m_renderer.setUVy(Vec2d(f32(t * m_bodyIndex), f32(t * (m_bodyIndex+ 1))));
                        m_renderer.setDepth(m_actor->getDepth() + getTemplate()->getBodyZOffset());
                        m_renderer.draw(m_bezierCurve);
                    }

                    if (m_headState != State_None)
                    {
                        Vec2d size = getTemplate()->getHeadSize() * m_actor->getScale();

                        Vec2d center = size * getTemplate()->getHeadCenter();

                        Vec2d p1 = Vec2d(0.0f,0.0f) - center;
                        Vec2d p2 = Vec2d(0.0f,size.m_y) - center;
                        Vec2d p3 = Vec2d(size.m_x,size.m_y) - center;
                        Vec2d p4 = Vec2d(size.m_x,0.0f) - center;
                        p1 = m_stop.truncateTo2D() + p1.Rotate(angle);
                        p2 = m_stop.truncateTo2D() + p2.Rotate(angle);
                        p3 = m_stop.truncateTo2D() + p3.Rotate(angle);
                        p4 = m_stop.truncateTo2D() + p4.Rotate(angle);

                        m_head.addQuad(m_headIndex,p1,p2,p3,p4 );
                        m_head.draw(m_actor->getDepth() + getTemplate()->getHeadZOffset());
                    }

                    //DebugDraw::circle(p1,m_actor->getDepth(),0.1f,Color::red());
                    //DebugDraw::circle(p2,m_actor->getDepth(),0.1f,Color::red());
                    //DebugDraw::circle(p3,m_actor->getDepth(),0.1f,Color::red());
                    //DebugDraw::circle(p4,m_actor->getDepth(),0.1f,Color::red());

                    if (m_tailState !=State_None)
                    {
                        Vec2d size = getTemplate()->getTailSize() * m_actor->getScale();
                        Vec2d center = size * getTemplate()->getTailCenter();

                        Vec2d p1 = Vec2d(0.0f,0.0f) - center;
                        Vec2d p2 = Vec2d(0.0f,size.m_y) - center;
                        Vec2d p3 = Vec2d(size.m_x,size.m_y) - center;
                        Vec2d p4 = Vec2d(size.m_x,0.0f) - center;

                        p1 = m_effectStart.truncateTo2D() + p1.Rotate(angle);
                        p2 = m_effectStart.truncateTo2D() + p2.Rotate(angle);
                        p3 = m_effectStart.truncateTo2D() + p3.Rotate(angle);
                        p4 = m_effectStart.truncateTo2D() + p4.Rotate(angle);
 
                        m_tail.addQuad(m_tailIndex,p1,p2,p3,p4  );
                        m_tail.draw(m_actor->getDepth() + getTemplate()->getTailZOffset());
                    }
                    //DebugDraw::circle(p1,m_actor->getDepth(),0.1f,Color::green());
                    //DebugDraw::circle(p2,m_actor->getDepth(),0.1f,Color::green());
                    //DebugDraw::circle(p3,m_actor->getDepth(),0.1f,Color::green());
                    //DebugDraw::circle(p4,m_actor->getDepth(),0.1f,Color::green());

                }
            }

    }


    void Ray_LightningPatchAIComponent::open()
    {
        m_headState = State_Anticipation;
        m_bodyState = State_Anticipation;
        m_curDist = 0.0f;
        m_drawDist = 0.0f;

        m_headIndex = U32_INVALID;
        m_bodyIndex = U32_INVALID;
    }

    void Ray_LightningPatchAIComponent::close(bbool _force)
    {
        if (_force)
        {
            m_tailState = State_None;
            m_bodyState = State_None;
            m_headState = State_None;
        }
        else
        {
            m_tailState = State_End;
            m_bodyState = State_End;
            m_headState = State_End;
        }
    }

    void Ray_LightningPatchAIComponent::charge()
    {
        if (m_tailState == State_None)
        {
            m_tailState = State_Anticipation;
            m_tailIndex = U32_INVALID;
        }
    }


#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void Ray_LightningPatchAIComponent::drawEdit( class ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
        DebugDraw::line3D(m_start,m_stop,Color::blue());

//        DebugDraw::bezierCurve(Vec3d::Zero,0.0f,m_bezierCurve,Color::blue(),0.1f);
    }



#endif // ITF_SUPPORT_EDITOR


    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_LightningPatchAIComponent_Template)
        BEGIN_SERIALIZATION(Ray_LightningPatchAIComponent_Template)
        SERIALIZE_MEMBER("maxLength",m_maxLength);
    SERIALIZE_MEMBER("boneName",m_boneName);
    SERIALIZE_MEMBER("effectboneName",m_effectBoneName);
    SERIALIZE_MEMBER("HeadPath",m_HeadPath);
    SERIALIZE_MEMBER("BodyPath",m_BodyPath);
    SERIALIZE_MEMBER("TailPath",m_TailPath);
    SERIALIZE_MEMBER("headMaxIndices",m_headMaxIndices);
    SERIALIZE_MEMBER("tailMaxIndices",m_tailMaxIndices);
    SERIALIZE_MEMBER("bodyMaxIndices",m_bodyMaxIndices);
    SERIALIZE_MEMBER("headLoopStart",m_headLoopStart);
    SERIALIZE_MEMBER("headLoopStop",m_headLoopStop);
    SERIALIZE_MEMBER("tailLoopStart",m_tailLoopStart);
    SERIALIZE_MEMBER("tailLoopStop",m_tailLoopStop);
    SERIALIZE_MEMBER("bodyLoopStart",m_bodyLoopStart);
    SERIALIZE_MEMBER("bodyLoopStop",m_bodyLoopStop);
    SERIALIZE_MEMBER("headSize",m_headSize);
    SERIALIZE_MEMBER("tailSize",m_tailSize);
    SERIALIZE_MEMBER("speed",m_speed);
    SERIALIZE_MEMBER("headCenter",m_headCenter);
    SERIALIZE_MEMBER("tailCenter",m_tailCenter);
    SERIALIZE_OBJECT("renderer",m_bezierRenderer);
    SERIALIZE_MEMBER("playRate",m_playRate);
    SERIALIZE_MEMBER("headZOffset",m_headZOffset);
    SERIALIZE_MEMBER("tailZOffset",m_tailZOffset);
    SERIALIZE_MEMBER("bodyZOffset",m_bodyZOffset);
    END_SERIALIZATION()

        //*****************************************************************************

        Ray_LightningPatchAIComponent_Template::Ray_LightningPatchAIComponent_Template()
        : m_maxLength(10.0f)
        , m_headMaxIndices(36)
        , m_tailMaxIndices(25)
        , m_bodyMaxIndices(36)
        , m_headLoopStart(7)
        , m_headLoopStop(24)
        , m_tailLoopStart(15)
        , m_tailLoopStop(20)
        , m_bodyLoopStart(10)
        , m_bodyLoopStop(23)
        , m_speed(10.0f)
        , m_headCenter(0.5f,0.5f)
        , m_tailCenter(0.5f,0.5f)
        , m_playRate(1.0f)
        , m_headZOffset(0.0f)
        , m_tailZOffset(0.0f)
        , m_bodyZOffset(0.0f)
    {
        // none
    }

    //*****************************************************************************

    Ray_LightningPatchAIComponent_Template::~Ray_LightningPatchAIComponent_Template()
    {
        // none
    }


    //*****************************************************************************


};
