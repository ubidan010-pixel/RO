#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RayVita_FrescoManagerAIComponent_H_
#include "rayman/gameplay/GamePlayVita/Managers/RayVita_FrescoManagerAIComponent.h"
#endif //_ITF_RayVita_FrescoManagerAIComponent_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_RAY_MOVIEPLAYERCOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_MoviePlayerComponent.h"
#endif //_ITF_RAY_MOVIEPLAYERCOMPONENT_H_

    namespace ITF
    {

#define FRESCO_GRP_INDEX 30

    IMPLEMENT_OBJECT_RTTI(RayVita_FrescoManagerAIComponent)

    BEGIN_SERIALIZATION_CHILD(RayVita_FrescoManagerAIComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(RayVita_FrescoManagerAIComponent)
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "This component requires an LinkComponent.");
    END_VALIDATE_COMPONENT()


    //*****************************************************************************
    RayVita_FrescoManagerAIComponent::RayVita_FrescoManagerAIComponent()
    : m_linkComponent(NULL)
    , m_isInit(bfalse)
    {
        // none
    }

    //*****************************************************************************
    RayVita_FrescoManagerAIComponent::~RayVita_FrescoManagerAIComponent()
    {
        // none
    }

    //*****************************************************************************

    void RayVita_FrescoManagerAIComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // Get components
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        ITF_ASSERT_MSG(m_linkComponent, "The door MUST have an LinkComponent.");

        // Event
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventGeneric,4063838687), this);

    }

    //*****************************************************************************
    void RayVita_FrescoManagerAIComponent::init()
    {
		m_isInit = btrue;

		bbool isFrescoUnlocked = bfalse;
		if(RAY_GAMEMANAGER->getPersistentGameData()->getFrescoData().IsFrescoUnlocked(getTemplate()->getFrescoId()))
		{
			isFrescoUnlocked = true;
		}

        if (m_linkComponent)
        {

            const LinkComponent::ChildrenList& children  = m_linkComponent->getChildren();
            for (u32 i = 0; i < children.size(); i++)
            {
                Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());

                if ( pickable )
                {
                    Actor* childActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                    if (childActor)
                    {
						ActorRef childRef =childActor->getRef();

						if (childRef.isValid())
						{
							if(!isFrescoUnlocked)
							{
								if(i<MAX_FRESCO_RELIC)
								{
									if(RAY_GAMEMANAGER->getPersistentGameData()->getFrescoData().GetRelicStatus(getTemplate()->getFrescoId(),i)==RELIC_STATUS_UNLOCKED)
									{
										LOG("\n RayVita_FrescoManagerAIComponent Display RELIC ID %d ---> %s",i+1,children[i].getPath().getStrId());
										childActor->enable();
									}
									else
									{
										childActor->disable();
									}
								}
								else if(i == FRESCO_GRP_INDEX)
								{
									childActor->disable();
								}
							}
							else
							{
								if(i == FRESCO_GRP_INDEX)
								{
									LOG("\n FrescoUnlocked RayVita_FrescoManagerAIComponent Display GRP %d ",i,children[i].getPath().getStrId());
									childActor->enable();
								}
								else
								{
									childActor->disable();
								}
							}

						}
                    }
                }
            }
        }

		if(1)
		{
			startPlayingVideo();
		}
    }

    //*****************************************************************************
    void RayVita_FrescoManagerAIComponent::onBecomeActive( )
    {
        Super::onBecomeActive();
    }

    //*****************************************************************************
    void RayVita_FrescoManagerAIComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

#ifdef ITF_SUPPORT_CHEAT

		InputAdapter::PressStatus buttons[JOY_MAX_BUT];
		f32 axes[ JOY_MAX_AXES ];
		INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentEngine, 0, buttons, JOY_MAX_BUT);
		INPUT_ADAPTER->getGamePadPos(InputAdapter::EnvironmentEngine,0, axes, JOY_MAX_AXES);

	#ifdef ITF_VITA
		if (axes[m_joyTrigger_Right] == InputAdapter::Pressed &&
			axes[m_joyTrigger_Left] == InputAdapter::Pressed &&
			buttons[m_joyButton_Y] == InputAdapter::JustPressed )
		{
			m_isInit = false;
		}
	#else
		if (axes[m_joyTrigger_Right] > 0.f &&
			axes[m_joyTrigger_Left] > 0.f &&
			buttons[m_joyButton_Y] == InputAdapter::JustPressed )
		{
			m_isInit = false;
		}
	#endif
#endif

        if (!m_isInit)
        {
            init();
        }
    }

	///////////////////////////////////////////////////////////////////////////////////////////
	void RayVita_FrescoManagerAIComponent::startPlayingVideo()
	{
		if(m_actor->GetComponent<Ray_MoviePlayerComponent>())
		{
			m_actor->GetComponent<Ray_MoviePlayerComponent>()->setListener(this);
			m_actor->GetComponent<Ray_MoviePlayerComponent>()->playMovie();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void RayVita_FrescoManagerAIComponent::stopPlayingVideo()
	{
		if(m_actor->GetComponent<Ray_MoviePlayerComponent>())
		{
			m_actor->GetComponent<Ray_MoviePlayerComponent>()->stopMovie();
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void RayVita_FrescoManagerAIComponent::onMovieStarted()
	{
		RAY_GAMEMANAGER->setInputModeForGamePlay(bfalse);
		RAY_GAMEMANAGER->setInputModeForMenu(bfalse);
		RAY_GAMEMANAGER->setIsPlayingFrescoVideo(btrue);
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	void RayVita_FrescoManagerAIComponent::onMovieStopped()
	{
		RAY_GAMEMANAGER->setInputModeForGamePlay(btrue);
		RAY_GAMEMANAGER->setInputModeForMenu(btrue);
		RAY_GAMEMANAGER->setIsPlayingFrescoVideo(bfalse);
	}

    //*****************************************************************************

	IMPLEMENT_OBJECT_RTTI(RayVita_FrescoManagerAIComponent_Template)

    BEGIN_SERIALIZATION_CHILD(RayVita_FrescoManagerAIComponent_Template)
        SERIALIZE_MEMBER("frescoId", m_frescoId);
    END_SERIALIZATION()


    //*****************************************************************************

    RayVita_FrescoManagerAIComponent_Template::RayVita_FrescoManagerAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************

    RayVita_FrescoManagerAIComponent_Template::~RayVita_FrescoManagerAIComponent_Template()
    {
        // none
    }

    //*****************************************************************************



}
