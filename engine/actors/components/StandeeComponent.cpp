#include "precompiled_engine.h"

#ifndef _ITF_STANDEECOMPONENT_H_
#include "engine/actors/components/StandeeComponent.h"
#endif //_ITF_STANDEECOMPONENT_H_

#ifndef _ITF_ACTOR_H_
#include "engine/actors/actor.h"
#endif //_ITF_ACTOR_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_TEXTURE_H_
#include "engine/display/Texture.h"
#endif //_ITF_TEXTURE_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif //_ITF_FILESERVER_H_

#ifndef _ITF_TRANSFORM2D3D_H_
#include "engine/gameplay/Transform.h"
#endif //_ITF_TRANSFORM2D3D_H_

namespace ITF
{
    BEGIN_SERIALIZATION(StandeeMapping)
        SERIALIZE_MEMBER("MapName", m_MapName);
        SERIALIZE_MEMBER("StandeePath", m_StandeePath);
    END_SERIALIZATION()

    IMPLEMENT_OBJECT_RTTI(StandeeComponent)
    BEGIN_SERIALIZATION_CHILD(StandeeComponent)

        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
            // SERIALIZE_MEMBER("texture",m_textureFile);
        END_CONDITION_BLOCK()

    END_SERIALIZATION()

    StandeeComponent::StandeeComponent()
        : Super()
    {
    }

    StandeeComponent::~StandeeComponent()
    {
        Super::~TextureGraphicComponent();
    }

    void StandeeComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
    }

    void StandeeComponent::Draw()
    {
        Super::Draw();
    }

    void StandeeComponent::Update(f32 _deltaTime)
    {
        Super::Update(_deltaTime);
    }

    void StandeeComponent::ChangeStandee(StringID& _levelName)
    {
        auto standees = getTemplate()->m_standeeMapping;
        for (auto standee : standees)
        {
            if (standee == _levelName)
            {
                setTextureFile(standee.m_StandeePath);
                ReloadStandee();
                break;
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void StandeeComponent::onEditorMove(bbool _modifyInitialPos)
    {
        Super::onEditorMove(_modifyInitialPos);

        Update(0.0f);
    }
#endif // ITF_SUPPORT_EDITOR
    void StandeeComponent::ReloadStandee()
    {
        onActorLoaded(Pickable::HotReloadType_Checkpoint); 
    }
    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(StandeeComponent_Template)
    BEGIN_SERIALIZATION_CHILD(StandeeComponent_Template)
        SERIALIZE_CONTAINER_OBJECT("standeeMapping", m_standeeMapping);
    END_SERIALIZATION()

    StandeeComponent_Template::StandeeComponent_Template(): Super()
    {
    }
}
