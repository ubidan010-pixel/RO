#include "precompiled_gameplay.h"

#ifndef _ITF_UITOGGLEOPTIONCOMPONENT_H_
#include "gameplay/components/UI/UIToggleOptionComponent.h"
#endif //_ITF_UITOGGLEOPTIONCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif //_ITF_OBJECTPATH_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIToggleOptionComponent)
    BEGIN_SERIALIZATION_CHILD(UIToggleOptionComponent)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            SERIALIZE_MEMBER("checkboxOnPath", m_checkboxOnPath);
            SERIALIZE_MEMBER("checkboxOffPath", m_checkboxOffPath);
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIToggleOptionComponent::UIToggleOptionComponent()
    : Super()
    , m_checkboxOnActor(NULL)
    , m_checkboxOffActor(NULL)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIToggleOptionComponent::~UIToggleOptionComponent()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::clear()
    {
        m_checkboxOnActor = NULL;
        m_checkboxOffActor = NULL;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::resolveCheckboxActors()
    {
        m_checkboxOnActor = NULL;
        m_checkboxOffActor = NULL;

        if (!m_checkboxOnPath.isEmpty())
        {
            ObjectPath checkboxOnPath;
            ITF_STDSTRING pathStr = m_checkboxOnPath.cStr();
            checkboxOnPath.fromString(pathStr);

            if (checkboxOnPath.isValid())
            {
                Pickable* pickable = NULL;
                if (checkboxOnPath.getIsAbsolute())
                {
                    pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(checkboxOnPath);
                }
                else
                {
                    pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, checkboxOnPath);
                }

                if (pickable)
                {
                    m_checkboxOnActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
                }
            }
        }

        if (!m_checkboxOffPath.isEmpty())
        {
            ObjectPath checkboxOffPath;
            ITF_STDSTRING pathStr = m_checkboxOffPath.cStr();
            checkboxOffPath.fromString(pathStr);

            if (checkboxOffPath.isValid())
            {
                Pickable* pickable = NULL;
                if (checkboxOffPath.getIsAbsolute())
                {
                    pickable = SceneObjectPathUtils::getObjectFromAbsolutePath(checkboxOffPath);
                }
                else
                {
                    pickable = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, checkboxOffPath);
                }

                if (pickable)
                {
                    m_checkboxOffActor = pickable->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor, 2546623115));
                }
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        resolveCheckboxActors();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onBecomeActive()
    {
        Super::onBecomeActive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onPressed()
    {
        Super::onPressed();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onReleased()
    {
        Super::onReleased();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onRollover()
    {
        Super::onRollover();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onRollout()
    {
        Super::onRollout();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::onAction(const StringID & action)
    {
        Super::onAction(action);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIToggleOptionComponent::setValue(bbool value)
    {
        if (m_checkboxOnActor)
        {
            if (value)
                m_checkboxOnActor->enable();
            else
                m_checkboxOnActor->disable();
        }
        
        if (m_checkboxOffActor)
        {
            if (!value)
                m_checkboxOffActor->enable();
            else
                m_checkboxOffActor->disable();
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIToggleOptionComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIToggleOptionComponent_Template)
    END_SERIALIZATION()

    UIToggleOptionComponent_Template::UIToggleOptionComponent_Template()
    : Super()
    {
    }

    UIToggleOptionComponent_Template::~UIToggleOptionComponent_Template()
    {
    }
}


