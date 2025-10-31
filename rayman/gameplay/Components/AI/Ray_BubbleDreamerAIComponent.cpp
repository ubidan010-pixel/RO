#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_BUBBLEDREAMERAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_BubbleDreamerAIComponent.h"
#endif //_ITF_RAY_BUBBLEDREAMERAICOMPONENT_H_

#ifndef _ITF_RAY_AIUTILS_H_
#include "Rayman/gameplay/AI/Utils/Ray_AIUtils.h"
#endif //_ITF_RAY_AIUTILS_H_

#ifndef _ITF_RAY_TALKINGBUBBLECOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TalkingBubbleComponent.h"
#endif //_ITF_RAY_TALKINGBUBBLECOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_


#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

namespace ITF
{

    IMPLEMENT_OBJECT_RTTI(Ray_BubbleDreamerAIComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_BubbleDreamerAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_BubbleDreamerAIComponent)
    END_VALIDATE_COMPONENT()

    //------------------------------------------------------------------
    Ray_BubbleDreamerAIComponent::Ray_BubbleDreamerAIComponent()
    : m_timer(0.0f)
    , m_lineIndex(0)
    , m_bubbleRef(ITF_INVALID_OBJREF)
    , m_bubbleBoneIndex(U32_INVALID)
    , m_animComponent(NULL)
    , m_outroLinesIndex(U32_INVALID)
    , m_outroLinesSize(U32_INVALID)
    , m_linkComponent(NULL)
    , m_caged(bfalse)
    {
    }

    //------------------------------------------------------------------
    Ray_BubbleDreamerAIComponent::~Ray_BubbleDreamerAIComponent()
    {
    }

    //------------------------------------------------------------------
    void Ray_BubbleDreamerAIComponent::Update(const f32 _dt)
    {
        Super::Update(_dt);

        if (m_caged)
        {


        }
        else
        {
            Actor* bubble = m_bubbleRef.getActor();

            if ( bubble )
            {
                if ( m_bubbleBoneIndex != U32_INVALID )
                {
                    Vec3d bonePos;

                    if ( m_animComponent->getBonePos(m_bubbleBoneIndex,bonePos) )
                    {
                        if ( bubble->getTemplate()->isZForced() )
                        {
                            f32 sceneZ = AIUtils::getSceneZFromPickable(bubble);

                            bonePos.m_z = sceneZ + bubble->getTemplate()->getForcedZ() + getTemplate()->getBubbleZOffset();
                        }
                        else
                        {
                            bonePos.m_z = m_actor->getPos().m_z + getTemplate()->getBubbleZOffset();
                        }

                        bubble->setPos(bonePos);
                    }
                }
            }

            if (m_timer == 0.0f && !m_currentUser.isValid())
            {
                if (getBubbleComponent())
                    getBubbleComponent()->setDisappear();
            }

            m_timer = Max(m_timer - _dt, 0.0f);
        }

    }

    void Ray_BubbleDreamerAIComponent::onEvent( Event * _event )
    {
        Super::onEvent(_event);
        if (EventHanging * hang = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)))
        {
            if (m_caged)
            {
                if (hang->isHanging())
                {
                    if (m_currentUsers.size() == 0)
                    {
                        m_animComponent->setAnim(getTemplate()->getCagePullAnim());
                        m_currentUsers.push_back(hang->getActor());
                    }
                    else
                    {
                        i32 i = m_currentUsers.find(hang->getActor());
                        if (i == -1)
                        {
                            m_currentUsers.push_back(hang->getActor());
                        }
                    }


                }
                else
                {
                    i32 i = m_currentUsers.find(hang->getActor());
                    if (i != -1)
                    {
                        m_currentUsers.eraseNoOrder(i);
                    }
                    if (m_currentUsers.size() == 0)
                    {
                        m_caged = bfalse;
                        m_animComponent->setAnim(getTemplate()->getFreeStandAnim());
                        EventTrigger trigger;
                        trigger.setActivated(btrue);
                        trigger.setSender(m_actor->getRef());
                        trigger.setActivator(m_actor->getRef());
                        if (m_linkComponent)
                            m_linkComponent->sendEventToChildren(&trigger);

                    }

                }
            
            }
            else
            {
                if (hang->isHanging())
                {
                    if (!m_currentUser.isValid())
                    {
                        m_currentUser = hang->getActor();
                        if (hang->getActor() == m_lastUser && m_timer > 0.0f)
                        {
                            //increase line
                            ++m_lineIndex;
                        }
                        else
                        {
                            //new user
                            //line 0
                            m_lineIndex = 0;
                        }
                        playLine(m_currentUser,m_lineIndex);

                    }


                    if (m_currentUsers.size() == 0)
                    {
                        m_animComponent->setAnim(getTemplate()->getFreePullAnim());
                        m_currentUsers.push_back(hang->getActor());
                    }
                    else
                    {
                        i32 i = m_currentUsers.find(hang->getActor());
                        if (i == -1)
                        {
                            m_currentUsers.push_back(hang->getActor());
                        }
                    }

                }
                else
                {
                    if (m_currentUser.isValid() && hang->getActor() == m_currentUser)
                    {
                        m_currentUser.invalidate();
                        m_lastUser = hang->getActor();
                        m_timer = getTemplate()->getUserTime();
                    }

                    i32 i = m_currentUsers.find(hang->getActor());
                    if (i != -1)
                    {
                        m_currentUsers.eraseNoOrder(i);
                    }

                    if (m_currentUsers.size() == 0)
                    {
                        m_animComponent->setAnim(getTemplate()->getFreeStandAnim());
                    }

                }
            }
        }
        else if (Ray_EventQueryOpen * onQueryOpen = _event->DynamicCast<Ray_EventQueryOpen>(ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153)))
        {
            onQueryOpen->setOpen(!m_caged);
        }
    }

    void Ray_BubbleDreamerAIComponent::onActorLoaded( Pickable::HotReloadType _tupe )
    {
        Super::onActorLoaded(_tupe);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventHanging,2826410601),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(Ray_EventQueryOpen,251146153),this);
        
        m_animComponent = m_actor->GetComponent<AnimatedComponent>();

        m_animComponent->setUpdateAnimInput(this);

        if ( !getTemplate()->getBubblePath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_bubbleSpawner, getTemplate()->getBubblePath());

            Actor* bubbleActor = m_bubbleSpawner.getSpawnee(m_actor->getScene(),m_actor->getPos(),m_actor->getAngle());

            if ( bubbleActor )
            {
                m_bubbleRef = bubbleActor->getRef();
            }
        }

        m_linkComponent = m_actor->GetComponent<LinkComponent>();

        m_caged = getTemplate()->getCaged();

        //setup lines
        SafeArray<LocalisationId> lines;

        const ITF_VECTOR<Ray_BubbleDreamerAIComponent_Template::Dialog> & dialogs  = getTemplate()->getDialogs();

        u32 currentIndex = 0;
        for (ITF_VECTOR<Ray_BubbleDreamerAIComponent_Template::Dialog>::const_iterator it = dialogs.begin(); it != dialogs.end(); ++it)
        {
            const Ray_BubbleDreamerAIComponent_Template::Dialog & dial = *it;
            ITF_ASSERT(m_lines.find(dial.m_player) == m_lines.end());

            #ifndef ITF_CONSOLE
            LOG("player %s %d %d",dial.m_player.getDebugString(),currentIndex,dial.m_lines.size());
            #endif // ITF_CONSOLE

            m_lines[dial.m_player] = std::pair<u32,u32>(currentIndex,dial.m_lines.size());
            for(u32 i = 0; i < dial.m_lines.size(); ++i)
            {
                lines.push_back(dial.m_lines[i]);
            }
            currentIndex += dial.m_lines.size();
        }

        m_outroLinesIndex = currentIndex;
        m_outroLinesSize = getTemplate()->getOutroLines().size();
        for(u32 i = 0; i < getTemplate()->getOutroLines().size(); ++i)
        {
            lines.push_back(getTemplate()->getOutroLines()[i]);
        }

        u32 numSentences = lines.size();
        ResourceGroup *rs = m_actor->getResourceGroup();

        for ( u32 i = 0; i < numSentences; i++ )
        {
            Path audioPath = LOCALISATIONMANAGER->getAudioPath(lines[i]);
            if (!audioPath.isEmpty())
            {
#ifdef  ITF_SUPPORT_WWISE
                ResourceID mpR = rs->addResource(Resource::ResourceType_MusicPart, audioPath);
                const StringID wwiseEventGuid = AUDIO_ADAPTER->getLocalizationWWiseGUIdFromLocId(lines[i].value);
                MusicPart* musicPart = (MusicPart*)mpR.getResource();
                if (musicPart)
                    musicPart->setWwiseEventGuid(wwiseEventGuid);
#else
                rs->addResource(Resource::ResourceType_MusicPart, audioPath);
#endif
            }
        }

        if(getBubbleComponent())
            getBubbleComponent()->setSentences(lines);

        if (m_caged)
            m_animComponent->setAnim(getTemplate()->getCageStandAnim());
        else 
            m_animComponent->setAnim(getTemplate()->getFreeStandAnim());
    }

    void Ray_BubbleDreamerAIComponent::onResourceReady()
    {
        Super::onResourceReady();

        if ( getTemplate()->getBubbleBone().isValid() )
        {
            m_bubbleBoneIndex = m_animComponent->getBoneIndex(getTemplate()->getBubbleBone());
        }
    }

    Ray_TalkingBubbleComponent* Ray_BubbleDreamerAIComponent::getBubbleComponent() const
    {
        Actor* bubbleActor = m_bubbleRef.getActor();

        if ( bubbleActor )
        {
            return bubbleActor->GetComponent<Ray_TalkingBubbleComponent>();
        }

        return NULL;
    }

    void Ray_BubbleDreamerAIComponent::playLine( ActorRef _user,u32 _index )
    {
        Ray_TalkingBubbleComponent * bubble = getBubbleComponent();
        if (bubble)
        {
            const Player * player = GAMEMANAGER->getPlayerFromActor(_user);
            ITF_ASSERT(player);
            if (player)
            {
                const PlayerIDInfo* playerIDinfo = GAMEMANAGER->getPlayerIDInfoFromPlayerIndex(player->getIndex());
                ITF_ASSERT(playerIDinfo);
                if (playerIDinfo)
                {
                    StringID id(playerIDinfo->getId().cStr());


                    std::pair<u32,u32> & info = m_lines[id];

                    if (m_outroLinesSize)
                    {
                        if (_index < info.second)
                        {
                            bubble->showSentence(info.first + _index);
                            bubble->setAppear();
                        }
                        else
                        {
                            u32 random = Seeder::getSharedSeeder().GetU32(m_outroLinesIndex,m_outroLinesIndex + m_outroLinesSize-1);
                            ITF_ASSERT(random < m_outroLinesSize + m_outroLinesIndex);
                            bubble->showSentence(random);
                            bubble->setAppear();
                        }
                    }
                    else
                    {
                        _index = _index % info.second;
                        if (_index < info.second)
                        {
                            bubble->showSentence(info.first + _index);
                            bubble->setAppear();
                        }
                    }
                }
            }
        }
    }

    void Ray_BubbleDreamerAIComponent::updateAnimInput()
    {

        const f32 rand = Seeder::getSharedSeeder().GetFloat();
        if ( m_animComponent)
        {
            m_animComponent->setInput(ITF_GET_STRINGID_CRC(RandomN,218439229), rand);
        }
    }

    void Ray_BubbleDreamerAIComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
        Actor* bubbleActor = m_bubbleRef.getActor();
        if (bubbleActor)
        {
            bubbleActor->requestDestruction();
            m_bubbleRef.invalidate();
        }
    }





    //------------------------------------------------------------------
    // Template
    //------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_BubbleDreamerAIComponent_Template)
    
    BEGIN_SERIALIZATION(Ray_BubbleDreamerAIComponent_Template)
    SERIALIZE_MEMBER("bubbleBone",m_bubbleBone);
    SERIALIZE_MEMBER("bubblePath",m_bubbleAct);
    SERIALIZE_MEMBER("userTime", m_userTime);
    SERIALIZE_MEMBER("cageStandAnim", m_cageStandAnim);
    SERIALIZE_MEMBER("cagePullAnim", m_cagePullAnim);
    SERIALIZE_MEMBER("freeStandAnim", m_freeStandAnim);
    SERIALIZE_MEMBER("freePullAnim", m_freePullAnim);
    SERIALIZE_MEMBER("caged", m_caged);
    SERIALIZE_MEMBER("bubbleZoffset", m_bubbleZOffset);
    SERIALIZE_CONTAINER_OBJECT("dialogs",m_dialogs);
    SERIALIZE_CONTAINER("outrolines", m_outroLines);
    END_SERIALIZATION()

    BEGIN_SERIALIZATION_SUBCLASS(Ray_BubbleDreamerAIComponent_Template,Dialog)
    SERIALIZE_MEMBER("player", m_player);
    SERIALIZE_CONTAINER("lines", m_lines);
    END_SERIALIZATION()

    //------------------------------------------------------------------
    Ray_BubbleDreamerAIComponent_Template::Ray_BubbleDreamerAIComponent_Template()
    : m_userTime(3.0f)
    , m_caged(bfalse)
    , m_bubbleZOffset(0.0f)
    {
    }

    //------------------------------------------------------------------
    Ray_BubbleDreamerAIComponent_Template::~Ray_BubbleDreamerAIComponent_Template()
    {
    }

} // namespace ITF

