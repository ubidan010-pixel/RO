#include "precompiled_gameplay.h"

#ifndef _ITF_ARENADOORCOMPONENT_H_
#include "gameplay/Components/Misc/ArenaDoorComponent.h"
#endif //_ITF_ARENADOORCOMPONENT_H_

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_WINDCOMPONENT_H_
#include "gameplay/components/misc/WindComponent.h"
#endif //_ITF_WINDCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(ArenaDoorComponent)

    BEGIN_SERIALIZATION_CHILD(ArenaDoorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(ArenaDoorComponent)
        VALIDATE_COMPONENT_PARAM("", m_animComponent, "ArenaDoorComponent requires an AnimLightComponent/AnimatedComponent");
        VALIDATE_COMPONENT_PARAM("openAnim", getTemplate()->getOpenAnim().isValid(), "please provide an open anim");
    END_VALIDATE_COMPONENT()


    ArenaDoorComponent::ArenaDoorComponent()
        : m_animComponent(NULL)
        , m_linkComponent(NULL)
        , m_isOpen(bfalse)
        , m_justStarted(bfalse)
    {
    }

    ArenaDoorComponent::~ArenaDoorComponent()
    {
    }

    void ArenaDoorComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();  // mandatory, validated
        m_linkComponent = m_actor->GetComponent<LinkComponent>();  // not mandatory, we can have linked children OR bound children

        m_isOpen = bfalse;
        m_justStarted = btrue;
    }

    void ArenaDoorComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_justStarted)
        {
            m_justStarted = bfalse;
            return;
        }

        if (m_isOpen)
        {
            return;
        }


        bbool allDead = btrue;

        // LINKED CHILDREN
        if (m_linkComponent)
        {
            AIUtils::LinkIterator linkIterator(m_linkComponent, btrue);

            while ( Actor* child = linkIterator.getNextRelativeActor() )
            {
                if ( !queryIsDead(child) )
                {
                    allDead = bfalse;
                    break;
                }
            }
        }

        // BOUND CHILDREN
        if (allDead)
        {
			//this call is not thread safe
            static SafeArray<BaseObject*> children;
			children.clear();
		    
			ID_SERVER->getObjectListNoNull ( m_actor->getChildrenBindHandler().getChildren(), children );

            for (u32 i = 0; i < children.size(); i++)
            {
                Actor* child = static_cast<Actor*>(children[i]);

                if ( !queryIsDead(child) )
                {
                    allDead = bfalse;
                    break;
                }
            }
        }

        if (allDead)
        {
            openDoor();
        }
    }

    bbool ArenaDoorComponent::queryIsDead( Actor* _child )
    {
        if ( _child->isEnabled() )
        {
            EventQueryIsDead query;
            _child->onEvent(&query);
            return query.getIsDead();
        }
        // else: if it's inactive, we consider it's dead

        return btrue;
    }

    void ArenaDoorComponent::openDoor()
    {
        m_isOpen = btrue;
        m_animComponent->setAnim( getTemplate()->getOpenAnim());
    }


    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(ArenaDoorComponent_Template)
    BEGIN_SERIALIZATION(ArenaDoorComponent_Template)
        SERIALIZE_MEMBER("openAnim", m_openAnim);
    END_SERIALIZATION()

    ArenaDoorComponent_Template::ArenaDoorComponent_Template()
    {
    }

};