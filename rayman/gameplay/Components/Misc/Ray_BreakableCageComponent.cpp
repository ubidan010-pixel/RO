#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BREAKABLECAGECOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_BreakableCageComponent.h"
#endif //_ITF_RAY_BREAKABLECAGECOMPONENT_H_


namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_BreakableCageComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BreakableCageComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(Ray_BreakableCageComponent)
    END_VALIDATE_COMPONENT()

        Ray_BreakableCageComponent::Ray_BreakableCageComponent()
        : m_state(STAGE1)
    {
    }

    void Ray_BreakableCageComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_animComponent = m_actor->GetComponent<AnimLightComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),this);

        setState(STAGE1);
        m_justLeft.clear();
        m_justLanded.clear();
    }

    Ray_BreakableCageComponent::~Ray_BreakableCageComponent()
    {
    }


	//----------------------------------------------------------------------------------------------------------------------------
    void Ray_BreakableCageComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        if (m_state == STAGE1to2 || m_state == STAGE2to3)
        {
            if (m_animComponent->isSubAnimFinished())
                setState(STATES(m_state + 1));
        }
        if (m_state != STAGE3)
        {
            //////////////////////////////////////////////////////////////////////////
            bbool changeState = bfalse;
            for (u32 i = 0; i < m_justLanded.size(); ++i)
            {
                i32 index = m_justLeft.find(m_justLanded[i]);
                if ( index != -1)
                {
                    changeState = btrue;
                    m_justLeft.eraseNoOrder(index);
                }
            }
            m_justLanded.clear();

            if (changeState)
            {
                setState(STATES(m_state + 1));
            }
            //////////////////////////////////////////////////////////////////////////
        }
    }

    void Ray_BreakableCageComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        if (EventStickOnPolyline * onStick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)))
        {
            if (onStick->isSticked())
                m_justLanded.push_back(onStick->getActor());
            else
                m_justLeft.push_back(onStick->getActor());
        }
    }

    void Ray_BreakableCageComponent::setState( STATES _state )
    {
        switch(_state)
        {
        case STAGE1:
            m_animComponent->setAnim(getTemplate()->getStage1Anim());
            break;
        case STAGE1to2:
            m_animComponent->setAnim(getTemplate()->getStage1toStage2Anim());
            break;
        case STAGE2:
            m_animComponent->setAnim(getTemplate()->getStage2Anim());
            break;
        case STAGE2to3:
            m_animComponent->setAnim(getTemplate()->getStage2toStage3Anim());
            break;
        case STAGE3:
            m_animComponent->setAnim(getTemplate()->getStage3Anim());
            break;
        }
        m_state = _state;
    }

#ifdef ITF_SUPPORT_EDITOR
    void Ray_BreakableCageComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface,_flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_BreakableCageComponent_Template)
    BEGIN_SERIALIZATION(Ray_BreakableCageComponent_Template)
    SERIALIZE_MEMBER("stage1Anim",m_stage1Anim);
    SERIALIZE_MEMBER("stage2Anim",m_stage2Anim);
    SERIALIZE_MEMBER("stage3Anim",m_stage3Anim);
    SERIALIZE_MEMBER("stage1toStage2Anim",m_stage1toStage2Anim);
    SERIALIZE_MEMBER("stage2toStage3Anim",m_stage2toStage3Anim);
    END_SERIALIZATION()


    Ray_BreakableCageComponent_Template::Ray_BreakableCageComponent_Template()
    {
    }

    Ray_BreakableCageComponent_Template::~Ray_BreakableCageComponent_Template()
    {
    }

}
