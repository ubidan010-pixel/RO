#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_HUNTERHOLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Shooter/Ray_HunterHoleAIComponent.h"
#endif //_ITF_RAY_HUNTERHOLEAICOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_
#include "rayman/gameplay/ai/Behaviors/Shooter/Ray_AIHunterAttackBehavior.h"
#endif //_ITF_RAY_AIHUNTERATTACKBEHAVIOR_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_HunterHoleAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_HunterHoleAIComponent)
    END_SERIALIZATION()


    Ray_HunterHoleAIComponent::Ray_HunterHoleAIComponent() 
        : Super()
        , m_idleBehavior(NULL)
        , m_attackBehavior(NULL)
    {
    }

    Ray_HunterHoleAIComponent::~Ray_HunterHoleAIComponent()
    {
    }

    void Ray_HunterHoleAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        unpause();

        setBehavior(m_idleBehavior);
    }

    void Ray_HunterHoleAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        m_idleBehavior = createAiBehavior(getTemplate()->getIdleBehavior());
        m_attackBehavior = static_cast<Ray_AIHunterAttackBehavior *>(createAiBehavior(getTemplate()->getAttackBehavior()));
        Super::onActorLoaded(_hotReload);

        setBehavior(m_idleBehavior);
    }



    void Ray_HunterHoleAIComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        bbool closeRange = bfalse;
        ObjectRef enemy = findEnemy(closeRange);
        if (enemy != ITF_INVALID_OBJREF)
        {
            m_attackBehavior->setVictim(enemy);
            setBehavior(m_attackBehavior);            
        }
        else if ( m_currentBehavior != m_idleBehavior )
        {
            setBehavior(m_idleBehavior);
        }
    }

    ObjectRef Ray_HunterHoleAIComponent::findEnemy(bbool & _closeRange) const
    {
        ObjectRef enemy = ITF_INVALID_OBJREF;
        if (getTemplate()->getUseRadius())
        {

            Actor * actor =  AIUtils::getClosestEnemy(m_actor, m_actor->isFlipped(), getTemplate()->getFindEnemyRadius());
            if (actor)
            {
                enemy = actor->getRef();
            }

        }
        else
        {
            u32 enemyCount;
            Actor * actor =  AIUtils::getClosestEnemy(m_actor, m_actor->isFlipped(), getTemplate()->getDetectionRange(), &enemyCount);
            if (actor)
            {
                _closeRange = bfalse;
                enemy = actor->getRef();
            }
        }
        return enemy;
    }

    void Ray_HunterHoleAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);
    }


    void Ray_HunterHoleAIComponent::onBehaviorFinished()
    {
        Super::onBehaviorFinished();

        if (m_currentBehavior == m_attackBehavior)
        {
            // let Update decide what to do next
            setBehavior(m_idleBehavior);
        }
    }

    void Ray_HunterHoleAIComponent::updateAnimInput()
    {
        Super::updateAnimInput();
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_HunterHoleAIComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface,_flags);
        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if (getTemplate()->getUseRadius())
        {
            GFX_ADAPTER->drawDBGCircle(m_actor->get2DPos(),getTemplate()->getFindEnemyRadius(),Color::red());
        }
        else
        {
            AABB aabb = getTemplate()->getDetectionRange();
            bbool flip = m_actor->isFlipped(); 
            if (flip)
                aabb.FlipHorizontaly(bfalse);
            DebugDraw::AABB(m_actor->getPos(),0.0f,aabb,Color::red(),0.f,1.f,"DetectionRange");
        }
    }
#endif // ITF_SUPPORT_EDITOR

    //----------------------------------------------------------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_HunterHoleAIComponent_Template)
        BEGIN_SERIALIZATION_CHILD(Ray_HunterHoleAIComponent_Template)
        SERIALIZE_MEMBER("findEnemyRadius", m_findEnemyRadius);
        SERIALIZE_MEMBER("useRadius", m_useRadius);
        SERIALIZE_OBJECT("detectionRange", m_detectionRange);
        SERIALIZE_OBJECT_WITH_FACTORY("idleBehavior", m_idleBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        SERIALIZE_OBJECT_WITH_FACTORY("attackBehavior", m_attackBehavior, ACTORSMANAGER->getAIBehaviorsFactory());
        END_SERIALIZATION()

        Ray_HunterHoleAIComponent_Template::Ray_HunterHoleAIComponent_Template()
        : m_findEnemyRadius(4.f)
        , m_detectionRange(Vec2d(-2, -2), Vec2d(5, 2))     // 5 meters front, 2 behind, 2 above & below
        , m_useRadius(btrue)
        , m_idleBehavior(NULL)
        , m_attackBehavior(NULL)
    {
    }


} // namespace ITF

