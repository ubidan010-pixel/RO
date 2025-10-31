#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_WM_LOADINGSCREENCOMPONENT_H_
#include "rayman/gameplay/Components/WorldMap/Ray_WM_LoadingScreenComponent.h"
#endif //_ITF_RAY_WM_LOADINGSCREENCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_WM_LoadingScreenComponent_Template)

        BEGIN_SERIALIZATION(Ray_WM_LoadingScreenComponent_Template)
        END_SERIALIZATION()

        //-------------------------------------------------------------------------------------

        IMPLEMENT_OBJECT_RTTI(Ray_WM_LoadingScreenComponent)

        BEGIN_SERIALIZATION_CHILD(Ray_WM_LoadingScreenComponent)
        END_SERIALIZATION()

        BEGIN_VALIDATE_COMPONENT(Ray_WM_LoadingScreenComponent)
        VALIDATE_COMPONENT_PARAM_CATEGORY(GPP,"m_linkComponent",m_linkComponent,"Ray_WM_LoadingScreenComponent requires a linkComponent");
    END_VALIDATE_COMPONENT()

        Ray_WM_LoadingScreenComponent::Ray_WM_LoadingScreenComponent()
        : m_linkComponent(NULL)
        , m_playingSequence(bfalse)
        , m_loading(bfalse)
        , m_started(bfalse)
    {
    }

    Ray_WM_LoadingScreenComponent::~Ray_WM_LoadingScreenComponent()
    {
    }


    void Ray_WM_LoadingScreenComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_started = bfalse;
    }

    void Ray_WM_LoadingScreenComponent::onBecomeActive()
    {
        Super::onBecomeActive();

    }

    void Ray_WM_LoadingScreenComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
        if (!m_started)
        {
            m_started = btrue;

            //start sequence
            EventSequenceControl control;
            control.setSender(m_actor->getRef());
            control.setState(SequencePlayerComponent::State_Playing);
            m_linkComponent->sendEventToChildren(&control);
            m_playingSequence = btrue;

            //preload map
            StringID tag = RAY_GAMEMANAGER->getWMLastLevelTag();
            //m_world = RAY_GAMEMANAGER->preLoadMap(RAY_GAMEMANAGER->getLevelPath(tag));
            //m_level = RAY_GAMEMANAGER->preLoadMap("/Scene/Production/1_4_ForestGate/1_4_ForestGate.isc");
            m_loading = btrue;

        }
        else
        {
            //once loading is done and sequence is finished switch map.
            if (m_loading)
            {
                if(RAY_GAMEMANAGER->isPreloadedMapLoaded(m_world))
                {
                    m_loading = bfalse;
                    //tell sequence to stop looping
                    if (!setSequenceLooping(bfalse))
                    {
                        StringID tag = RAY_GAMEMANAGER->getWMLastLevelTag();
                        RAY_GAMEMANAGER->loadGameplayMap(RAY_GAMEMANAGER->getLevelPath(tag), bfalse, bfalse,m_world);
                        //RAY_GAMEMANAGER->switchToPreloadedMap(m_world);
                    }
                }
            }
            else
            {
                if (m_playingSequence)
                {
                    SequencePlayerComponent * sequence = getSequence();
                    if (sequence)
                    {
                        if(sequence->getState() == SequencePlayerComponent::State_Stopped)
                        {
                            m_playingSequence = bfalse;
                            StringID tag = RAY_GAMEMANAGER->getWMLastLevelTag();
                            RAY_GAMEMANAGER->loadGameplayMap(RAY_GAMEMANAGER->getLevelPath(tag), bfalse, bfalse,m_world);
                            //RAY_GAMEMANAGER->switchToPreloadedMap(m_world);
                        }
                    }
                    else
                    {
                        StringID tag = RAY_GAMEMANAGER->getWMLastLevelTag();
                        RAY_GAMEMANAGER->loadGameplayMap(RAY_GAMEMANAGER->getLevelPath(tag), bfalse, bfalse,m_world);
                        //RAY_GAMEMANAGER->switchToPreloadedMap(m_world);
                    }

                }
            }
        }
    }

    SequencePlayerComponent * Ray_WM_LoadingScreenComponent::getSequence( )
    {
        const LinkComponent::ChildrenList &children = m_linkComponent->getChildren();
        for(u32 i = 0; i < children.size(); i++)
        {
            Pickable *p= SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());
            if (p)
            {
                if (p->getObjectType() == BaseObject::eActor)
                {
                    Actor * sequenceActor  = static_cast<Actor*>(p);
                    if (sequenceActor)
                    {
                        return sequenceActor->GetComponent<SequencePlayerComponent>();
                    }
                }
            }
        }	
        return NULL;
    }

    bbool Ray_WM_LoadingScreenComponent::setSequenceLooping(bbool _enable)
    {
        SequencePlayerComponent * sequence = getSequence();
        if (sequence)
        {
            u32 numEvents = sequence->getEventsListCount();;
            for (u32 i = 0; i <numEvents; ++i)
            {
                SequenceEvent * evt = sequence->getEventAtIndex(i);
                if (evt)
                {
                    if (evt->getTemplate()->getChannelID() == ITF_GET_STRINGID_CRC(loadingScreen,479360800))
                    {
                        evt->setEnableByChannel(_enable);
                        return btrue;
                    }
                }
            }

        }
        return bfalse;
    }

};
