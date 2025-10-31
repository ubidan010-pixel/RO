#include "precompiled_gameplay_war.h"

#ifndef _ITF_WAR_AIPERFORMHITRAYCASTACTION_H_
#include "war/gameplay/ai/Actions/War_AIPerformHitRaycastAction.h"
#endif //_ITF_WAR_AIPERFORMHITRAYCASTACTION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

#ifndef _ITF_AICOMPONENT_H_
#include "gameplay/Components/AI/AIComponent.h"
#endif //_ITF_AICOMPONENT_H_

#ifndef _ITF_AIBEHAVIOR_H_
#include "gameplay/ai/Behaviors/AIBehavior.h"
#endif //_ITF_AIBEHAVIOR_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_FACTIONMANAGER_H_
#include "gameplay/managers/FactionManager.h"
#endif //_ITF_FACTIONMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(War_AIPerformHitRaycastAction)

    BEGIN_SERIALIZATION_CHILD(War_AIPerformHitRaycastAction)
        SERIALIZE_MEMBER("boneName", m_boneName, ESerialize_Template);
        SERIALIZE_MEMBER("polylineImpactFX", m_polylineImpactFX, ESerialize_Template);
    END_SERIALIZATION()

    War_AIPerformHitRaycastAction::War_AIPerformHitRaycastAction()
        : Super()
        , m_boneIndex(U32_INVALID)
    {
    }

    War_AIPerformHitRaycastAction::~War_AIPerformHitRaycastAction()
    {
    }

    void War_AIPerformHitRaycastAction::performHit( AnimGameplayEvent* _animEvent )
    {
        if (m_boneIndex == U32_INVALID)
        {
            m_boneIndex = m_animComponent->getBoneIndex(m_boneName);
            ITF_ASSERT(m_boneIndex != U32_INVALID);
        }

        f32 angle = m_actor->getAngle();
        f32 z = m_actor->getDepth();
        AnimBoneDyn* bone = m_animComponent->getBone(m_boneIndex);
        const Vec2d& from = bone->m_Pos;
        Vec2d direction = bone->getDirection();
        Vec2d to = from + direction * 10;


        // we need to check against the environment AND the phantoms,
        // and pick the closest one (contacts are added and sorted by t).

        f32 raycastRadius = 0.1f;
        PhysShapeCircle shape(raycastRadius);
        PhysContactsContainer contacts;

        PHYSWORLD->checkEncroachment(from, to, angle,
            &shape, ECOLLISIONFILTER_ALL, m_actor->getDepth(), contacts);

        PHYSWORLD->collidePhantoms(from, to, angle,
            &shape, m_actor->getDepth(), ECOLLISIONFILTER_ALL, contacts);


        GFX_ADAPTER->drawDBGLine(from, to, 0, 0, 1, 0.5f);
        GFX_ADAPTER->drawDBGCircle(from, raycastRadius, 0, 0, 1, 0.5f);

        for (u32 i = 0; i < contacts.size(); i++)
        {
            SCollidableContact& contact = contacts[i];
            GFX_ADAPTER->drawDBGLine(contact.m_contactPoint, contact.m_contactPoint + contact.m_normal, 1, 0, 0, 0.5f);
        }


        for (u32 i = 0; i < contacts.size(); i++)
        {
            SCollidableContact& contact = contacts[i];

            if (contact.m_collidableUserData == m_actor->getRef())
                continue;

            //BaseObject* phantomHit = GETOBJECT( contact.m_collidable );
            BaseObject* objectHit = GETOBJECT( contact.m_collidableUserData );

            if (Actor* actorHit = objectHit->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
            {
                // we just hit an actor

                if ( AIUtils::isTarget(m_actor, actorHit) )
                {
                    War_HitStim stim;

                    stim.setPrevPos(from);
                    stim.setPos(to);
                    stim.setAngle(angle);
                    stim.setDepth(m_actor->getDepth());
                    stim.setSender(m_actor->getRef());

                    stim.setLevel(0);
                    stim.setReceivedHitType(WAR_RECEIVEDHITTYPE_NORMALHIT);
                    stim.setFxPos(contact.m_contactPoint.to3d(&z));

                    actorHit->onEvent(&stim);

                    break;
                }
            }
            else if (PolyLine* lineHit = objectHit->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747)))
            {
                // we just hit a polyline
                if (m_polylineImpactFX.isValid())
                {
                    z += 0.1f;
                    m_fxController->playFX(m_polylineImpactFX, contact.m_contactPoint.to3d(&z));
                }

                break;
            }
            else
            {
                LOG_BEN("we just shot a %s, not sure what to do with it!", objectHit->GetObjectClassName());
            }
        }
    }

}