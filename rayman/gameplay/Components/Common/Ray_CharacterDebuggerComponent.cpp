#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_CHARACTERDBUGGERCOMPONENT_H_
#include "rayman/gameplay/components/Common/Ray_CharacterDebuggerComponent.h"
#endif //_ITF_RAY_CHARACTERDBUGGERCOMPONENT_H_

#ifndef _ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerControllerComponent.h"
#endif //_ITF_RAY_PLAYERCONTROLLERCOMPONENT_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_CharacterDebuggerComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_CharacterDebuggerComponent)
    END_SERIALIZATION()

    Ray_CharacterDebuggerComponent::Ray_CharacterDebuggerComponent()
        : Super()
        , m_hangingPolyline(ITF_INVALID_OBJREF)
        , m_hangingEdge(U32_INVALID)
        , m_currentEfficiency(0.f)
    {
    }

    Ray_CharacterDebuggerComponent::~Ray_CharacterDebuggerComponent()
    {
    }


    void Ray_CharacterDebuggerComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        Ray_PlayerControllerComponent* player = m_actor->GetComponent<Ray_PlayerControllerComponent>();
        if (player)
        {
            m_hangingPolyline = player->getHangingPolyline();
            m_hangingEdge = player->getHangingEdge();
            m_currentEfficiency = player->getCurrentEfficiency();
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_CharacterDebuggerComponent::editor_draw()
    {
        if ( !m_active )
            return;

        if ( m_hangingPolyline != ITF_INVALID_OBJREF )
        {
            PolyLine* poly;
            const PolyLineEdge* edge;

            AIUtils::getPolyLine(m_hangingPolyline,m_hangingEdge,poly,edge);

            if ( edge )
            {
                Vec2d pA = edge->getPos();
                Vec2d pB = edge->getPos() + edge->m_vector;

                GFX_ADAPTER->drawDBGLine(pA, pB, 0.f,0.f,1.f,0.f);
            }
        }

        #define MAX_BUFF 256
        char buf[MAX_BUFF];

        f32 r = Clamp(1.f-m_currentEfficiency,0.f,1.f);
        f32 g = Clamp(m_currentEfficiency,0.f,1.f);
        SPRINTF_S(buf,MAX_BUFF,"Efficiency: %f",m_currentEfficiency);
        GFX_ADAPTER->drawDBGText(buf,ITF_POS2D_AUTO,ITF_POS2D_AUTO,r,g,0.f);

        if ( m_hangingPolyline != ITF_INVALID_OBJREF )
        {
            SPRINTF_S(buf,MAX_BUFF,"Hanging");
            GFX_ADAPTER->drawDBGText(buf);
        }

        Super::editor_draw();
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_CharacterDebuggerComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_CharacterDebuggerComponent_Template)
    END_SERIALIZATION()
}