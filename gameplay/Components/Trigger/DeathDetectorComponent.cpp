#include "precompiled_gameplay.h"

#ifndef _ITF_DEATHDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DeathDetectorComponent.h"
#endif //_ITF_DEATHDETECTORCOMPONENT_H_


#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(DeathDetectorComponent)

    BEGIN_SERIALIZATION_CHILD(DeathDetectorComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(DeathDetectorComponent)
    VALIDATE_COMPONENT_PARAM("m_linkComponent",m_linkComponent,"LinkComponent required for DeathDetectorComponent");
    END_VALIDATE_COMPONENT()


    DeathDetectorComponent::DeathDetectorComponent()
    : m_linkComponent    (NULL)
    {
    }

    DeathDetectorComponent::~DeathDetectorComponent()
    {
    }

    void DeathDetectorComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        m_actorsInside.clear();

        if ( m_disabled  )
        {
            return;
        }

        const LinkComponent::ChildrenList& children = m_linkComponent->getChildren();

        StringID s_death = ITF_GET_STRINGID_CRC(death,2142372323);

        if (getTemplate()->getIsAnd())
        {
            ActorRefList list;
            list.reserve(children.size());
            u32 killableCount = 0;
            for (LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it)
            {
                const ChildEntry & child = *it;
                if (child.hasTag(s_death))
                {
                    const ObjectPath& obj = child.getPath();
                    Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,obj);

                    if (pickable)
                    {
                        if (Actor * actor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                        {
                            killableCount++;
                            EventQueryIsDead query;
                            query.setSender(m_actor->getRef());
                            actor->onEvent(&query);
                            if (query.getIsDead())
                            {
                                list.push_back(actor->getRef());

                            }
                        }
                    }
                }
            }
            if (list.size() == killableCount)
            {
                m_actorsInside  =  list;
            }

        }
        else
        {
            for (LinkComponent::ChildrenList::const_iterator it = children.begin(); it != children.end(); ++it)
            {
                const ChildEntry & child = *it;
                if (child.hasTag(s_death))
                {
                    const ObjectPath& obj = child.getPath();
                    Pickable* pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor,obj);

                    if (pickable)
                    {
                        if (Actor * actor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)))
                        {
                            EventQueryIsDead query;
                            query.setSender(m_actor->getRef());
                            actor->onEvent(&query);
                            if (query.getIsDead())
                            {
                                m_actorsInside.push_back(actor->getRef());
                            }
                        }
                    }
                }
            }
        }
    }

    void DeathDetectorComponent::onActorLoaded( Pickable::HotReloadType _hotReload )
    {
        Super::onActorLoaded(_hotReload);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();
    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(DeathDetectorComponent_Template)
    BEGIN_SERIALIZATION_CHILD(DeathDetectorComponent_Template)
    SERIALIZE_MEMBER("isAnd",m_isAnd);
    END_SERIALIZATION()

    DeathDetectorComponent_Template::DeathDetectorComponent_Template()
    : m_isAnd(bfalse)
    {
    }
};