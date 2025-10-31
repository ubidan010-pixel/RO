#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_UIFADESCREENCOMPONENT_H_
#include "rayman/gameplay/Ray_UIFadeScreenComponent.h"
#endif //_ITF_RAY_UIFADESCREENCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_UIFadeScreenComponent)
    BEGIN_SERIALIZATION_CHILD(Ray_UIFadeScreenComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_UIFadeScreenComponent)

        VALIDATE_COMPONENT_PARAM("Ray_UIFadeScreenComponent", m_animComponent != NULL, "Anim Component is mandatory");

    END_VALIDATE_COMPONENT()

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_UIFadeScreenComponent::Ray_UIFadeScreenComponent()
    : m_animComponent(NULL)
    , m_currentFadeTime(0.f)
    , m_currentFadeIndex(U32_INVALID)
    , m_fadeOut(bfalse)
    , m_animPosInputIndex(U32_INVALID)
    , m_fadeOutInputIndex(U32_INVALID)
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    Ray_UIFadeScreenComponent::~Ray_UIFadeScreenComponent()
    {
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_UIFadeScreenComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animComponent = m_actor->GetComponent<AnimatedComponent>();

        if ( m_animComponent )
        {
            m_animComponent->enableDraw(bfalse);
            m_animComponent->setUpdateAnimInput(this);
            m_animComponent->setRank2D(UI_TEXTMANAGER->getFadeScreenDepthRank());
        }

        RAY_GAMEMANAGER->setUIFadeActorRef(m_actor->getRef());
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_UIFadeScreenComponent::onResourceReady()
    {
        Super::onResourceReady();

        if ( m_animPosInputIndex == U32_INVALID )
        {
            m_animPosInputIndex = m_animComponent->findInputIndex(ITF_GET_STRINGID_CRC(AnimPos,1351367425));
            m_fadeOutInputIndex = m_animComponent->findInputIndex(ITF_GET_STRINGID_CRC(FadeOut,872188705));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_UIFadeScreenComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if ( m_currentFadeIndex != U32_INVALID )
        {
            const UIFadeEntry& currentFade = getTemplate()->getTypesList()[m_currentFadeIndex];

            m_currentFadeTime = Min(m_currentFadeTime+_dt,currentFade.getDuration());

            if ( !m_fadeOut )
            {
                if ( m_currentFadeTime == currentFade.getDuration() )
                {
                    clearFade();
                }
            }

            f32 screenWidth = static_cast<f32>(GFX_ADAPTER->getScreenWidth());
            f32 screenHeight = static_cast<f32>(GFX_ADAPTER->getScreenHeight());
            f32 scaleX = screenWidth / getTemplate()->getAnimSize().m_x;
            f32 scaleY = screenHeight / getTemplate()->getAnimSize().m_y;

            m_actor->setScale(Vec2d(scaleX,scaleY)*1.1f);
        }
        else
        {
            m_animComponent->enableDraw(bfalse);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_UIFadeScreenComponent::isFadeOutFinished() const
    {
        if ( m_currentFadeIndex != U32_INVALID )
        {
            return !m_fadeOut || m_currentFadeTime >= getTemplate()->getTypesList()[m_currentFadeIndex].getDuration();
        }
        else
        {
            return btrue;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Ray_UIFadeScreenComponent::isFadeInFinished() const
    {
        if ( m_currentFadeIndex != U32_INVALID )
        {
            return m_currentFadeTime >= getTemplate()->getTypesList()[m_currentFadeIndex].getDuration();
        }
        else
        {
            return btrue;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_UIFadeScreenComponent::startFade( const StringID& _id, bbool _force )
    {
        if ( m_fadeOut )
            return;

        u32 fadeIndex = getFadeEntryIndex(_id);

        if ( fadeIndex != U32_INVALID )
        {
            m_currentFadeIndex = fadeIndex;
            m_currentFadeTime = 0.f;
            m_fadeOut = btrue;

            m_animComponent->enableDraw(btrue);

            const UIFadeEntry& fadeEntry = getTemplate()->getTypesList()[fadeIndex];
            EventShow showEvent(1.f,0.f);

            if (_force)
                m_currentFadeTime = fadeEntry.getDuration();

            showEvent.setOverrideColor(fadeEntry.getColor());

            m_actor->onEvent(&showEvent);

            m_animComponent->setAnim(fadeEntry.getType());
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_UIFadeScreenComponent::stopFade( bbool _force )
    {
        if ( m_currentFadeIndex != U32_INVALID )
        {
            if ( _force )
            {
                clearFade();
            }
            else
            {
                m_fadeOut = bfalse;
                m_currentFadeTime = 0.f;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_UIFadeScreenComponent::clearFade()
    {
        m_animComponent->enableDraw(bfalse);
        m_currentFadeIndex = U32_INVALID;
        m_currentFadeTime = 0.f;
        m_fadeOut = bfalse;
    }

    u32 Ray_UIFadeScreenComponent::getFadeEntryIndex( const StringID& _id ) const
    {
        u32 numAnims = getTemplate()->getTypesList().size();

        for ( u32 i = 0; i < numAnims; i++ )
        {
            const UIFadeEntry& entry = getTemplate()->getTypesList()[i];

            if ( entry.getId() == _id )
            {
                return i;
            }
        }

        return U32_INVALID;
    }

    void Ray_UIFadeScreenComponent::updateAnimInput()
    {
        if ( m_currentFadeIndex != U32_INVALID && m_animPosInputIndex != U32_INVALID )
        {
            const UIFadeEntry& currentFade = getTemplate()->getTypesList()[m_currentFadeIndex];
            f32 t = m_currentFadeTime / currentFade.getDuration();

            m_animComponent->setInputFromIndex(m_animPosInputIndex,t);
            m_animComponent->setInputFromIndex(m_fadeOutInputIndex,m_fadeOut);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_UIFadeScreenComponent_Template)

    BEGIN_SERIALIZATION_CHILD(Ray_UIFadeScreenComponent_Template)

        SERIALIZE_CONTAINER_OBJECT("types",m_types);

    END_SERIALIZATION()

    BEGIN_SERIALIZATION(UIFadeEntry)

        SERIALIZE_MEMBER("id",m_id);
        SERIALIZE_MEMBER("duration",m_duration);
        SERIALIZE_MEMBER("color",m_color);
        SERIALIZE_MEMBER("anim",m_animType);

    END_SERIALIZATION()

}
