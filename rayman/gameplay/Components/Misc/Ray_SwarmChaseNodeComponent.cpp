#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SWARMCHASENODECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_SwarmChaseNodeComponent.h"
#endif //_ITF_RAY_SWARMCHASENODECOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_SwarmChaseNodeComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_SwarmChaseNodeComponent)
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_SwarmChaseNodeComponent::Ray_SwarmChaseNodeComponent()
    {
        // none
    }

    //*****************************************************************************

    Ray_SwarmChaseNodeComponent::~Ray_SwarmChaseNodeComponent()
    {
        // none
    }

    //*****************************************************************************

    void Ray_SwarmChaseNodeComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        // none
    }

    //*****************************************************************************

    f32 Ray_SwarmChaseNodeComponent::getRadius() const
    {
        return m_actor->getScale().m_x;  
    }

#ifdef ITF_SUPPORT_EDITOR
    //*****************************************************************************
    void Ray_SwarmChaseNodeComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        DebugDraw::circle(m_actor->getPos(), getRadius(), Color::white());
    }
#endif // ITF_SUPPORT_EDITOR

    //*****************************************************************************
    // Template
    //*****************************************************************************

    IMPLEMENT_OBJECT_RTTI(Ray_SwarmChaseNodeComponent_Template)
    BEGIN_SERIALIZATION(Ray_SwarmChaseNodeComponent_Template)
    END_SERIALIZATION()

    //*****************************************************************************

    Ray_SwarmChaseNodeComponent_Template::Ray_SwarmChaseNodeComponent_Template()
    {
        // none
    }

    //*****************************************************************************

    Ray_SwarmChaseNodeComponent_Template::~Ray_SwarmChaseNodeComponent_Template()
    {
        // none
    }

    //*****************************************************************************


}
