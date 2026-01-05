#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_MURPHYASSISTBUBBLEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MurphyAssistBubbleAIComponent.h"
#endif //_ITF_RAY_MURPHYASSISTBUBBLEAICOMPONENT_H_

#ifndef _ITF_RAY_MURPHYASSISTAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MurphyAssistAIComponent.h"
#endif //_ITF_RAY_MURPHYASSISTAICOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_MurphyAssistBubbleAIComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_MurphyAssistBubbleAIComponent)
    END_SERIALIZATION()

    //--------------------------------------------------------------------------------------------------------
    Ray_MurphyAssistBubbleAIComponent::Ray_MurphyAssistBubbleAIComponent()
    : Super()
    , m_owner(NULL)
    {
    }

    Ray_MurphyAssistBubbleAIComponent::~Ray_MurphyAssistBubbleAIComponent()
    {
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistBubbleAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor, ITF_GET_STRINGID_CRC(PunchStim, 200533519), this);
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistBubbleAIComponent::Update(f32 _dt)
    {
        Super::Update(_dt);
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistBubbleAIComponent::processPunchEvent(const PunchStim* const _stim)
    {
        m_actor->requestDestruction();
		ObjectRef sender = _stim->getSender();

		Player* player = GAMEMANAGER->getPlayerFromActor(sender);
		if (player)
		{
			u32 index = player->getIndex();
			broadcastHitEventToOwner(index);
		}
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistBubbleAIComponent::broadcastHitEventToOwner(u32 _playerIndex)
    {
        if (m_owner != NULL)
        {
            m_owner->setCanFollowPlayer(btrue, _playerIndex);
            RAY_GAMEMANAGER->setIsMurphyAssistFollowingPlayer(btrue);
        }
    }
    //--------------------------------------------------------------------------------------------------------
    void Ray_MurphyAssistBubbleAIComponent::onEvent(Event* _event)
    {
        Super::onEvent(_event);

        if (_event->IsClassCRC(EventInteractionQuery::GetClassCRCStatic()))
            return;

        if (PunchStim* stim = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim, 200533519)))
        {
            if (GAMEMANAGER->isPlayerActor(stim->getSender()))
            {
				processPunchEvent(stim);
            }
        }
    }
    //--------------------------------------------------------------------------------------------------------
    Ray_MurphyAssistBubbleAIComponent_Template::Ray_MurphyAssistBubbleAIComponent_Template()
    : Super()
    {
    }

    Ray_MurphyAssistBubbleAIComponent_Template::~Ray_MurphyAssistBubbleAIComponent_Template()
    {
    }

    IMPLEMENT_OBJECT_RTTI(Ray_MurphyAssistBubbleAIComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_MurphyAssistBubbleAIComponent_Template)

    END_SERIALIZATION()

}
