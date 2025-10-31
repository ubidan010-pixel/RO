#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_AIPERFORMHITPUNCHACTION_H_
#include "rayman/gameplay/AI/Actions/Ray_AIPerformHitPunchAction.h"
#endif //_ITF_RAY_AIPERFORMHITPUNCHACTION_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H
#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_AIPerformHitPunchAction_Template)

        BEGIN_SERIALIZATION_CHILD(Ray_AIPerformHitPunchAction_Template)

    SERIALIZE_OBJECT_WITH_FACTORY("shape", m_shape, PHYSWORLD->getShapesFactory());
    SERIALIZE_MEMBER("shapeOffset", m_shapeOffset);

    END_SERIALIZATION()


        Ray_AIPerformHitPunchAction_Template::Ray_AIPerformHitPunchAction_Template()
        : Super()
        , m_shape(NULL)
        , m_shapeOffset(Vec2d::Zero)
    {
        if (!m_shape)
        {
            // create a default shape
            m_shape = newAlloc(mId_Gameplay, PhysShapeCircle(1.f));
        }
    }

    IMPLEMENT_OBJECT_RTTI(Ray_AIPerformHitPunchAction)

        Ray_AIPerformHitPunchAction::Ray_AIPerformHitPunchAction()
        : m_shape(NULL)
    {
    }

    Ray_AIPerformHitPunchAction::~Ray_AIPerformHitPunchAction()
    {
        SF_DEL(m_shape);
    }


    void Ray_AIPerformHitPunchAction::onActivate()
    {
        Super::onActivate();
    }
    void Ray_AIPerformHitPunchAction::performHit()
    {
        bbool flipped = m_actor->isFlipped();
        // HACK: if the fake backpedaling hack is on in the roam behavior, flipped is wrong
        if ( Actor* victim = AIUtils::getActor(m_targetVictim) )
        {
            Vec2d vecToVictim = victim->get2DPos() - m_actor->get2DPos();
            flipped = (vecToVictim.dot(Vec2d::Right) < 0);
        }

        if (PhysShapePolygon* polygon = m_shape->DynamicCast<PhysShapePolygon>(ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112)))
            polygon->setFlipped(flipped);

        Vec2d offset = getTemplate()->getShapeOffset();
        Vec2d dir = Vec2d::Right.Rotate(m_actor->getAngle());

        PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>(m_shape);

        if(stim)
        {
            stim->setDepth(m_actor->getDepth());
            stim->setSender(m_actor->getRef());
            stim->setLevel(getTemplate()->getHitLevel());
            stim->setReceivedHitType(getTemplate()->getHitType());
            stim->setAngle(m_actor->getAngle());
            stim->setFaction(m_aiComponent->getFaction());

            if ( flipped )
            {
                dir *= -1.f;
                offset.m_x *= -1.f;
            }

            offset = offset.Rotate(m_actor->getAngle());

            Vec2d shapePos = m_actor->get2DPos() + offset;
            stim->setPos(shapePos);
            stim->setPrevPos(shapePos);
            stim->setDirection(dir);
            stim->setFxPos(shapePos.to3d(m_actor->getPos().m_z));

            //AIUtils::drawDBGPhysShape(m_actor->get2DPos() + offset,m_actor->getDepth(),m_actor->getAngle(),m_shape,Color::white(),1);

            // set original sender (for rewards' Action_KillHunterWithOwnBullet)
            stim->setOriginalSender(m_originalSender);

            STIMSMANAGER->sendStim(stim);
        }
    }

    void Ray_AIPerformHitPunchAction::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded( _hotReload );
        const PhysShape * template_shape = getTemplate()->getShape();
        if ( template_shape)
        {            
            m_shape = static_cast<PhysShape*>(template_shape->CreateNewObject());
            BinaryClone<PhysShape>(const_cast<PhysShape*>(getTemplate()->getShape()),m_shape);
        }
    }

}