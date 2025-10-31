

#include "precompiled_engine.h"

#ifndef _ITF_TEXTUREGRAPHICCOMPONENT_H_
#include "engine/actors/components/texturegraphiccomponent.h"
#endif //_ITF_TEXTUREGRAPHICCOMPONENT_H_

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

IMPLEMENT_OBJECT_RTTI(TextureGraphicComponent)
BEGIN_SERIALIZATION_CHILD(TextureGraphicComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("texture",m_textureFile);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

TextureGraphicComponent::TextureGraphicComponent()
: Super()
, m_curAngle(Vec3d::Zero)
{
}

TextureGraphicComponent::~TextureGraphicComponent()
{
    clear();
}

void TextureGraphicComponent::clear()
{
    if ( m_actor && m_textureID.isValidResourceId() )
    {
        m_actor->removeResource(m_textureID);
        m_textureID.invalidateResourceId();
    }
}

void TextureGraphicComponent::reset()
{
    m_curAngle.m_x = getTemplate()->getAngleX().ToRadians();
    m_curAngle.m_y = getTemplate()->getAngleY().ToRadians();
    m_curAngle.m_z = getTemplate()->getAngleZ().ToRadians();
}

void TextureGraphicComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    if ( !m_textureFile.isEmpty() )
    {
        m_textureID = m_actor->addResource(Resource::ResourceType_Texture, m_textureFile);
    }
    else if ( !getTemplate()->getTextureFile().isEmpty() )
    {
        m_textureID = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getTextureFile());
    }

    reset();
}

void TextureGraphicComponent::Draw()
{
    Super::Draw();

    Texture* tex = static_cast<Texture*>(m_textureID.getResource());
    GFX_ADAPTER->addPrimitiveInZList((void*)&m_quad, GFX_ITF_3DQUAD, tex, GetActor()->getDepth(), m_renderInTarget,ObjectRef::InvalidRef);
}

void TextureGraphicComponent::Update( f32 _deltaTime )
{
    Super::Update( _deltaTime );

    // setup quad information.
    Color col = getTemplate()->getDefaultColor() * getColorFactor();
    col.m_a *= m_alpha;
    m_quad.m_color          = col.getAsU32();
    m_quad.m_fogColor       = getColorFog();
    m_quad.m_size           = m_actor->getScale();
    m_quad.m_pos            = getWorldTransform(m_actor).transformPos(getTemplate()->getPosOffset());
    m_quad.m_blendMode      = getTemplate()->getBlendMode();
    m_quad.m_rotation.m_x   = m_curAngle.m_x + (m_actor->isFlipped() ? MTH_PI : 0.f );
    m_quad.m_rotation.m_y   = m_curAngle.m_y;
    m_quad.m_rotation.m_z   = m_actor->getAngle() + m_curAngle.m_z + (m_actor->isFlipped() ? MTH_PI : 0.f );

    // update angle.
    m_curAngle.m_x += getTemplate()->getSpeedRotX() * _deltaTime;
    m_curAngle.m_y += getTemplate()->getSpeedRotY() * _deltaTime;
    m_curAngle.m_z += getTemplate()->getSpeedRotZ() * _deltaTime;

    // update AABB
    const Vec2d halfSize = m_quad.m_size * 0.5f;
    AABB aabb(halfSize);
    aabb.grow(-halfSize);
    aabb.Rotate(m_actor->getAngle(), btrue);
    m_actor->growRelativeAABB(aabb);
}

#ifdef ITF_SUPPORT_EDITOR
void TextureGraphicComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    Update(0.0f);
}
#endif // ITF_SUPPORT_EDITOR

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(TextureGraphicComponent_Template)
BEGIN_SERIALIZATION_CHILD(TextureGraphicComponent_Template)

    SERIALIZE_MEMBER("texture",m_textureFile);
    SERIALIZE_MEMBER("defaultColor",m_defaultColor); 
    SERIALIZE_MEMBER("angleX",m_angleX); 
    SERIALIZE_MEMBER("angleY",m_angleY); 
    SERIALIZE_MEMBER("angleZ",m_angleZ); 
    SERIALIZE_MEMBER("speedRotX",m_speedRotX); 
    SERIALIZE_MEMBER("speedRotY",m_speedRotY); 
    SERIALIZE_MEMBER("speedRotZ",m_speedRotZ); 

END_SERIALIZATION()

TextureGraphicComponent_Template::TextureGraphicComponent_Template()
: m_defaultColor(Color::white())
, m_angleX(btrue, 0.f)
, m_angleY(btrue, 0.f)
, m_angleZ(btrue, 0.f)
, m_speedRotX(0.f)
, m_speedRotY(0.f)
, m_speedRotZ(0.f)
{
}

}



