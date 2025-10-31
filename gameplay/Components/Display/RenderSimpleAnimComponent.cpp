#include "precompiled_gameplay.h"

#ifndef _ITF_RENDERSIMPLEANIMCOMPONENT_H_
#include "gameplay/Components/Display/RenderSimpleAnimComponent.h"
#endif //_ITF_RENDERSIMPLEANIMCOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_ANIMATIONRES_H_
#include "engine/animation/AnimationRes.h"
#endif //_ITF_ANIMATIONRES_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_ANIMATIONTRACK_H_
#include "engine/animation/AnimationTrack.h"
#endif //_ITF_ANIMATIONTRACK_H_

#ifndef _ITF_ANIMLIGHTCOMPONENT_H_
#include "engine/actors/components/AnimLightComponent.h"
#endif //_ITF_ANIMLIGHTCOMPONENT_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(RenderSimpleAnimComponent)
BEGIN_SERIALIZATION_CHILD(RenderSimpleAnimComponent)
END_SERIALIZATION()


RenderSimpleAnimComponent::RenderSimpleAnimComponent()
: m_animComponent(NULL)
, m_computeLocalToGlobal(btrue)
{
}

RenderSimpleAnimComponent::~RenderSimpleAnimComponent()
{
}

void RenderSimpleAnimComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    m_animComponent = m_actor->GetComponent<AnimLightComponent>();

    ITF_ASSERT(m_animComponent);

    if ( m_animComponent )
    {
        m_animComponent->setIsProcedural(btrue);
    }
}

bbool RenderSimpleAnimComponent::needsDraw() const
{
    if ( !m_animComponent )
    {
        return bfalse;
    }
    return !m_animComponent->getDraw2D();
}


bbool RenderSimpleAnimComponent::needsDraw2D() const
{
    if ( !m_animComponent )
    {
        return bfalse;
    }

    return m_animComponent->getDraw2D();
}

void RenderSimpleAnimComponent::Draw()
{
    if ( !m_animComponent )
    {
        return;
    }

    m_animComponent->Draw();
}


void RenderSimpleAnimComponent::Draw2D()
{
    if ( !m_animComponent )
    {
        return;
    }

    m_animComponent->Draw2D();
}

void RenderSimpleAnimComponent::Update( f32 _deltaTime )
{
    if ( m_animComponent && m_animComponent->isLoaded() )
    {
        AnimMeshScene* animMeshScene = m_animComponent->getAnimMeshScene();
        
        ITF_ASSERT_MSG(animMeshScene && animMeshScene->m_AnimInfo.m_frameInfo.size(), "Anim component not initialized correctly or with no animation set!");
        if (  animMeshScene && animMeshScene->m_AnimInfo.m_frameInfo.size() )
        {
            if ( m_computeLocalToGlobal )
            {
                animMeshScene->m_AnimInfo.m_Pos     = m_animComponent->getOffsettedPos();
                animMeshScene->m_AnimInfo.m_Angle   = m_animComponent->getOffsettedAngle();
                animMeshScene->m_AnimInfo.m_Scale   = m_actor->getScale();
                animMeshScene->m_AnimInfo.m_globalData.clear();
                animMeshScene->m_AnimInfo.ComputeBonesFromLocalToWorld(btrue);
                animMeshScene->m_AnimInfo.ComputePolylinePoints();
            }

            m_animComponent->updatePatchAndAABB();
        }

        EventAnimUpdated animUpdated(_deltaTime);
        m_actor->onEvent(&animUpdated);
    }
}

#ifdef ITF_SUPPORT_EDITOR
void RenderSimpleAnimComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    if ( m_animComponent && m_animComponent->isLoaded() )
    {
        m_animComponent->updatePatchAndAABB();
    }
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(RenderSimpleAnimComponent_Template)
BEGIN_SERIALIZATION(RenderSimpleAnimComponent_Template)
END_SERIALIZATION()

}
