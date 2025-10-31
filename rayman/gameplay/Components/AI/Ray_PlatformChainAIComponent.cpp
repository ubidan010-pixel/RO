#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLATFORMCHAINAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_PlatformChainAIComponent.h"
#endif //_ITF_RAY_PLATFORMCHAINAICOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_TWEENCOMPONENT_H_
#include "gameplay/Components/Common/TweenComponent.h"
#endif //_ITF_TWEENCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_PlatformChainAIComponent_Template)

    BEGIN_SERIALIZATION(Ray_PlatformChainAIComponent_Template)
        SERIALIZE_MEMBER("rollbackDepth", m_rollbackDepth);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_PlatformChainAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_PlatformChainAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_PlatformChainAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "Ray_PlatformChainAIComponent requires a LinkComponent");
        VALIDATE_COMPONENT_PARAM("", m_tweenComponent, "Ray_PlatformChainAIComponent requires a TweenComponent");
    END_VALIDATE_COMPONENT()

    Ray_PlatformChainAIComponent::Ray_PlatformChainAIComponent()
        : m_linkComponent(NULL)
        , m_tweenComponent(NULL)
        , m_appeared(bfalse)
    {
    }

    Ray_PlatformChainAIComponent::~Ray_PlatformChainAIComponent()
    {
    }


    void Ray_PlatformChainAIComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_tweenComponent = m_actor->GetComponent<TweenComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSetTarget,4067373371), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventWallJump,2476933602), this);
    }

    void Ray_PlatformChainAIComponent::onSceneActive()
    {
        Super::onSceneActive();

        sendDataToChildren();

        AIUtils::addListenersToBoundFrises(m_actor);
    }

    void Ray_PlatformChainAIComponent::onCheckpointLoaded()
    {
        Super::onCheckpointLoaded();

        sendDataToChildren();

        AIUtils::addListenersToBoundFrises(m_actor);
    }

    void Ray_PlatformChainAIComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();

        if(m_actor)
            AIUtils::removeListenersToBoundFrises(m_actor);
    }

    void Ray_PlatformChainAIComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        if ( EventSetTarget* eventInfo = _event->DynamicCast<EventSetTarget>(ITF_GET_STRINGID_CRC(EventSetTarget,4067373371)) )
        {
            receiveDataFromParent(eventInfo);
        }
        else if ( EventTrigger* eventTrigger = _event->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)) )
        {
            if (eventTrigger->getActivated())
            {
                appear();
            }
        }
        else if ( EventStickOnPolyline* eventStick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
        {
            if (eventStick->isSticked())
            {
                makeChildrenAppear();
                forceParentsToDisappear();
            }
        }
        else if ( EventHanging* eventHang = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)) )
        {
            if (eventHang->isHanging())
            {
                makeChildrenAppear();
                forceParentsToDisappear();
            }
        }
        else if ( _event->IsClassCRC(ITF_GET_STRINGID_CRC(EventWallJump,2476933602)) )
        {
            makeChildrenAppear();
            forceParentsToDisappear();
        }
    }


    void Ray_PlatformChainAIComponent::sendDataToChildren() const
    {
        EventSetTarget data;
        data.setTarget(m_actor->getRef());

        m_linkComponent->sendEventToChildren(&data);
    }

    void Ray_PlatformChainAIComponent::receiveDataFromParent( EventSetTarget* _data )
    {
        m_parents.push_back(_data->getTarget());
    }

    Ray_PlatformChainAIComponent* Ray_PlatformChainAIComponent::getParent( const u32 _index ) const
    {
        if ( Actor* parent = m_parents[_index].getActor() )
        {
            return parent->GetComponent<Ray_PlatformChainAIComponent>();
        }

        return NULL;
    }


    void Ray_PlatformChainAIComponent::appear()
    {
        if (m_appeared)
        {
            return;
        }

        m_appeared = btrue;

        m_tweenComponent->playSet(ITF_GET_STRINGID_CRC(Appear,1975175726));
    }

    void Ray_PlatformChainAIComponent::disappear()
    {
        if (!m_appeared)
        {
            return;
        }

        m_appeared = bfalse;

        m_tweenComponent->playSet(ITF_GET_STRINGID_CRC(Disappear,2647978385));
    }


    void Ray_PlatformChainAIComponent::makeChildrenAppear()
    {
        AIUtils::LinkIterator iterator(m_linkComponent);
        while ( Actor* childActor = iterator.getNextRelativeActor() )
        {
            if ( Ray_PlatformChainAIComponent* child = childActor->GetComponent<Ray_PlatformChainAIComponent>() )
            {
                child->appear();
            }
        }
    }

    void Ray_PlatformChainAIComponent::forceParentsToDisappear()
    {
        u32 depth = getTemplate()->getRollbackDepth();
        if (depth > 0)
        {
            forceParentsToDisappear(depth);
        }
    }

    void Ray_PlatformChainAIComponent::forceParentsToDisappear( u32& _depth )
    {
        if (_depth == 1)
        {
            for (u32 iP = 0; iP < m_parents.size(); iP++)
            {
                if ( Ray_PlatformChainAIComponent* ancestor = getParent(iP) )
                {
                    ancestor->disappear();
                }
            }
        }
        else
        {
            _depth--;
            for (u32 iP = 0; iP < m_parents.size(); iP++)
            {
                if ( Ray_PlatformChainAIComponent* ancestor = getParent(iP) )
                {
                    ancestor->forceParentsToDisappear(_depth);
                }
            }
        }
    }

};