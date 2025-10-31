#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAYWMBONUSLEVELSPOTCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_BonusLevelSpotComponent.h"
#endif //_ITF_RAYWMBONUSLEVELSPOTCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_TALKINGBUBBLECOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TalkingBubbleComponent.h"
#endif //_ITF_RAY_TALKINGBUBBLECOMPONENT_H_

#ifndef _ITF_RAYWMPANELCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_PanelComponent.h"
#endif //_ITF_RAYWMPANELCOMPONENT_H_

#ifndef _ITF_UIMENU_H_
#include "gameplay/components/UI/UIMenu.h"
#endif //_ITF_UIMENU_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _LOCALISATION_ID_H_
#include "core/LocalisationId.h"
#endif // _LOCALISATION_ID_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WM_BonusLevelSpotComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_WM_BonusLevelSpotComponent_Template)
    SERIALIZE_MEMBER("bubblePath",m_bubbleAct);
    SERIALIZE_MEMBER("panelAct",m_panelAct);
    SERIALIZE_MEMBER("panelBone",m_panelBone);
    SERIALIZE_MEMBER("panelOffset",m_panelOffset);
    SERIALIZE_CONTAINER("blockedLines", m_blockedLines);
    SERIALIZE_CONTAINER("openLines", m_openedLines);
    END_SERIALIZATION()

    Ray_WM_BonusLevelSpotComponent_Template::Ray_WM_BonusLevelSpotComponent_Template()
    : m_panelBone(StringID::Invalid)
    {

    }

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(Ray_WM_BonusLevelSpotComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_WM_BonusLevelSpotComponent)
    SERIALIZE_CONTAINER("bubbleOffset", m_offset);
    SERIALIZE_MEMBER("bubbleZOffset",m_bubbleZOffset);
    SERIALIZE_CONTAINER("blockedLines", m_blockedLines);
    SERIALIZE_CONTAINER("openLines", m_openedLines);
    END_SERIALIZATION()

    Ray_WM_BonusLevelSpotComponent::Ray_WM_BonusLevelSpotComponent() 
    : m_linkComponent(NULL)
    , m_firstUpdate(btrue)
    , m_bubbleRef(ITF_INVALID_OBJREF)
    , m_blockedLineIndexStart(U32_INVALID)
    , m_openLineIndexStart(U32_INVALID)
    , m_offset(0.0f,0.0f)
    , m_bubbleZOffset(0.0f)
    , m_blockerActive(bfalse)
    , m_currentLineIndex(U32_INVALID)

    {
    }

    void Ray_WM_BonusLevelSpotComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_firstUpdate = btrue;

        if ( !getTemplate()->getBubblePath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_bubbleSpawner, getTemplate()->getBubblePath());

            Actor* bubbleActor = m_bubbleSpawner.getSpawnee(m_actor->getScene(),m_actor->getPos(),m_actor->getAngle());

            if ( bubbleActor )
            {
                m_bubbleRef = bubbleActor->getRef();
            }
        }

        if ( !getTemplate()->getPanelPath().isEmpty() )
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_panelSpawnee, getTemplate()->getPanelPath());

            Actor* panelActor = m_panelSpawnee.getSpawnee(m_actor->getScene(),m_actor->getPos(),m_actor->getAngle());

            if ( panelActor )
            {
                m_panelRef = panelActor->getRef();

                Ray_WM_PanelComponent * panelComponent = getPanelComponent();
                if( panelComponent )
                    panelComponent->setTag(getTag());

                panelActor->disable();
            }
        }

        SafeArray <LocalisationId> lines;
        u32 index = 0;

        //open lines
        if (m_openedLines.size())
        {
            for (u32 i = 0; i < m_openedLines.size(); ++i)
                lines.push_back(m_openedLines[i]);
            m_openLineIndexStart = index;
            index+=m_openedLines.size();
        }
        else
        {
            const SafeArray<LocalisationId> & openedLines = getTemplate()->getOpenedLines();
            if (openedLines.size())
            {
                for (u32 i = 0; i < openedLines.size(); ++i)
                    lines.push_back(openedLines[i]);
                m_openLineIndexStart = index;
                index+=openedLines.size();
            }
        }


        //blocked lines
        if (m_blockedLines.size())
        {
            for (u32 i = 0; i < m_blockedLines.size(); ++i)
                lines.push_back(m_blockedLines[i]);
            m_blockedLineIndexStart = index;
            index+=m_blockedLines.size();

        }
        else
        {
            const SafeArray<LocalisationId> & blockedLines = getTemplate()->getBlockedLines();

            if (blockedLines.size())
            {
                for (u32 i = 0; i < blockedLines.size(); ++i)
                    lines.push_back(blockedLines[i]);
                m_blockedLineIndexStart = index;
                index+=blockedLines.size();
            }
        }


        if (getBubbleComponent())
        {
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
            getBubbleComponent()->setSentences(lines);
        }
        m_currentLineIndex = 0;
    }

    void Ray_WM_BonusLevelSpotComponent::onSceneActive()
    {
        Super::onSceneActive();

        // Disable cam mod
        pauseCamMod(btrue);
    }

    void Ray_WM_BonusLevelSpotComponent::onBecomeActive()
    {
        if ( getState() == SPOT_STATE_CANNOT_ENTER )
        {
            Actor* panelActor = m_panelRef.getActor();
            if ( panelActor )
                panelActor->enable();
        }
    }

    void Ray_WM_BonusLevelSpotComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        Actor* panelActor = m_panelRef.getActor();
        if ( panelActor )
            panelActor->disable();
    }

    void Ray_WM_BonusLevelSpotComponent::Update( f32 _dt )
    {
        // As those level can be updated for something else, update its state to match persistent data one
        updateState();

        Super::Update(_dt);
        //if (m_firstUpdate)
        {
            m_firstUpdate = bfalse;
            switch(getState())
            {
            case SPOT_STATE_CLOSED:
                activateBlocker(bfalse);
                break;
            case SPOT_STATE_NEW:
                activateBlocker(btrue);
                break;
            case SPOT_STATE_OPEN:
                activateBlocker(btrue);
                break;
            case SPOT_STATE_CANNOT_ENTER:
                {
                    Actor* panelActor = m_panelRef.getActor();
                    if ( panelActor )
                        panelActor->enable();

                    activateBlocker(bfalse);
                }
                break;
            case SPOT_STATE_COMPLETED:
                {
                    Actor * actor = getBlocker();
                    if (actor)
                        actor->disable();
                }
                break;
            default:
                ITF_ASSERT(0); // Should never be here
                break;
            }
        }

        Actor* bubbleActor = m_bubbleRef.getActor();

        if ( bubbleActor )
        {
            Actor * actor = getBlocker();
            if (actor)
            {
                f32 depth = 0.0f;
                if ( bubbleActor->getTemplate()->isZForced() )
                {
                    f32 sceneZ = AIUtils::getSceneZFromPickable(bubbleActor);

                    depth = sceneZ + bubbleActor->getTemplate()->getForcedZ() + getBubbleZOffset();
                }
                else
                {
                    depth = actor->getPos().m_z + getBubbleZOffset();
                }

                bubbleActor->setDepth(depth);

                bubbleActor->set2DPos(actor->get2DPos() + getOffset());
            }

        }

        // Update panel pos depending on bone position
        if ( m_animatedComponent )
        {
            Actor* panelActor = m_panelRef.getActor();
            if ( panelActor )
            {
                Vec3d pos = m_actor->getPos();

                u32 boneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getPanelBone());
                if ( boneIndex != U32_INVALID )
                {
                    AnimBoneDyn * bone = m_animatedComponent->getBone(boneIndex);
                    pos = bone->m_Pos.to3d(m_actor->getDepth());

                    // NB[LaurentCou]: if this happens, spawned actors are moved
                    // on the origin, preventing unload of WM level selections
                    // as their bbox gets uber-large
                    ITF_ASSERT_MSG(pos!=Vec3d::Zero, "Invalid bone position");
                    if (pos==Vec3d::Zero)
                    {
                        pos = m_actor->getPos(); // hack
                    }
                }

                pos += getTemplate()->getPanelOffset();
                panelActor->setPos(pos);
            }
        }
    }

    
    void Ray_WM_BonusLevelSpotComponent::onAction()
    {
        Super::onAction();
    }


    Ray_TalkingBubbleComponent* Ray_WM_BonusLevelSpotComponent::getBubbleComponent() const
    {
        Actor* bubbleActor = m_bubbleRef.getActor();

        if ( bubbleActor )
        {
            return bubbleActor->GetComponent<Ray_TalkingBubbleComponent>();
        }

        return NULL;
    }

    Ray_WM_PanelComponent* Ray_WM_BonusLevelSpotComponent::getPanelComponent() const
    {
        Actor* panelActor = m_panelRef.getActor();

        if ( panelActor )
        {
            return panelActor->GetComponent<Ray_WM_PanelComponent>();
        }

        return NULL;
    }

    void Ray_WM_BonusLevelSpotComponent::onConnection()
    {
        Super::onConnection();
    }

    void Ray_WM_BonusLevelSpotComponent::onEnter(bbool _changeMusic /*= bfalse*/)
    {
        Super::onEnter(_changeMusic);
        switch ( getState() )
        {
        case SPOT_STATE_CLOSED:
            ITF_ASSERT_MSG(0,"You shouldn't be able to go here !");
            break;
        case SPOT_STATE_NEW:
        case SPOT_STATE_OPEN:
            {
                // Enable cam mod
                pauseCamMod(bfalse);

                if (m_openLineIndexStart != U32_INVALID)
                {
                    u32 size = m_openedLines.size()?m_openedLines.size():getTemplate()->getOpenedLines().size();
                    m_currentLineIndex = m_currentLineIndex%size;
                    Ray_TalkingBubbleComponent * bubbleComponent = getBubbleComponent();
                    if (bubbleComponent)
                    {
                        bubbleComponent->showSentence(m_currentLineIndex + m_openLineIndexStart);
                        bubbleComponent->setAppear();
                    }
                    m_currentLineIndex++;
                }
            }
            break;
        case SPOT_STATE_COMPLETED:
            break;
        case SPOT_STATE_CANNOT_ENTER:
            if (m_blockedLineIndexStart != U32_INVALID)
            {

                // Enable cam mod
                pauseCamMod(bfalse);

                u32 size = m_blockedLines.size()?m_blockedLines.size():getTemplate()->getBlockedLines().size();
                m_currentLineIndex = m_currentLineIndex%size;
                Ray_TalkingBubbleComponent * bubbleComponent = getBubbleComponent();
                if (bubbleComponent)
                {
                    bubbleComponent->showSentence(m_currentLineIndex + m_blockedLineIndexStart);
                    bubbleComponent->setAppear();
                }
                m_currentLineIndex++;
            }
            // display message to tell the player why he can't enter in this level
            break;
        default:
            ITF_ASSERT(0); // Should never be here
        }

    }

    void Ray_WM_BonusLevelSpotComponent::onExit()
    {
        Super::onExit();
        Ray_TalkingBubbleComponent * bubbleComponent = getBubbleComponent();
        if (bubbleComponent)
        {
            bubbleComponent->setDisappear();
        }

        // Disable cam mod
        pauseCamMod(btrue);
    }

    Actor * Ray_WM_BonusLevelSpotComponent::getBlocker()
    {
        const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();
        u32 numChildren = children.size();

        for ( u32 i = 0; i < numChildren; i++ )
        {
            const ChildEntry & childentry = children[i];
            if (!childentry.hasTag(ITF_GET_STRINGID_CRC(Blocker,4280829862)))
                continue;

            Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, childentry.getPath());
            if ( !pickable )
            {
                continue;
            }

            Actor* child = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            if ( !child )
            {
                continue;
            }

            return child;
        }
        return NULL;
    }

    void Ray_WM_BonusLevelSpotComponent::activateBlocker(bbool _activate)
    {
        if (m_blockerActive != _activate)
        {
            m_blockerActive = _activate;
            EventActivate evt;
            evt.setActivated(_activate);
            const StringID blocker = ITF_GET_STRINGID_CRC(Blocker,4280829862);
            m_linkComponent->sendEventToChildren(&evt,blocker);
        }
    }

    void Ray_WM_BonusLevelSpotComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
        Actor* bubbleActor = m_bubbleRef.getActor();
        if (bubbleActor)
        {
            bubbleActor->requestDestruction();
            m_bubbleRef.invalidate();
        }

        Actor* panelActor = m_panelRef.getActor();
        if (panelActor)
        {
            panelActor->requestDestruction();
            m_panelRef.invalidate();
        }

    }

    void Ray_WM_BonusLevelSpotComponent::pauseCamMod(bbool _pause)
    {
        LinkComponent * pLinkComponent = m_actor->GetComponent<LinkComponent>();
        const LinkComponent::ChildrenList& children  = pLinkComponent->getChildren();
        for (u32 i=0; i<children.size(); ++i)
        {
            Actor *target = (Actor*)SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
            if (target)
            {
                bool isCamera = bfalse;
                if (children[i].getTagValue(ITF_GET_STRINGID_CRC(camera,4083739328), isCamera) && isCamera)
                {
                    EventPause onPause;
                    onPause.setPause(_pause);
                    target->onEvent(&onPause);
                }
            }
        }
    }

};
