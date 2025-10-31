

#include "precompiled_engine.h"

#ifndef _ITF_ATLASANIMATIONCOMPONENT_H_
#include "engine/actors/components/AtlasAnimationComponent.h"
#endif //_ITF_ATLASANIMATIONCOMPONENT_H_

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


IMPLEMENT_OBJECT_RTTI(AtlasAnimationComponent)
BEGIN_SERIALIZATION_CHILD(AtlasAnimationComponent)
END_SERIALIZATION()

AtlasAnimationComponent::AtlasAnimationComponent()
: Super()
, m_currentIndex(0)
, m_numIndices(0)
, m_timer(0.0f)
{
}

AtlasAnimationComponent::~AtlasAnimationComponent()
{
}


void AtlasAnimationComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);
    m_atlasObject.initialize(m_actor->getResourceGroup(),getTemplate()->getTextureFile(),1);
}

void AtlasAnimationComponent::Draw()
{
    Super::Draw();
    m_atlasObject.setColor(getColorFactor());
    m_atlasObject.setFogColor(getColorFog());
    m_atlasObject.addQuad(m_currentIndex,m_p1,m_p2,m_p3,m_p4);
    m_atlasObject.draw(m_actor->getDepth());

}

void AtlasAnimationComponent::Update( f32 _deltaTime )
{
    Super::Update( _deltaTime );
    if (m_numIndices == 0)
    {
        m_numIndices = m_atlasObject.getNumIndices();
    }
    else
    {
        updatePosition();
        
        updateAnimation(_deltaTime);
    }

    updateAABB();

}

void AtlasAnimationComponent::updatePosition()
{
    const f32 halfWidth = 0.5f * getTemplate()->getWidth() * m_actor->getScale().m_x;
    const f32 halfHeight = 0.5f * getTemplate()->getHeight() * m_actor->getScale().m_y;
    const Vec2d scaledOffset = (getTemplate()->getPosOffset() * m_actor->getScale());
    const Vec2d p1=Vec2d(-halfWidth,halfHeight) + scaledOffset;
    const Vec2d p2=Vec2d(-halfWidth,-halfHeight) + scaledOffset;
    const Vec2d p3=Vec2d(halfWidth,-halfHeight) + scaledOffset;
    const Vec2d p4=Vec2d(halfWidth,halfHeight) + scaledOffset;

    const Transform2d transform(m_actor->get2DPos(), m_actor->getAngle() + getTemplate()->getRotOffset().ToRadians(), m_actor->isFlipped());
    m_p1 = transform.transformPos(p1);
    m_p2 = transform.transformPos(p2);
    m_p3 = transform.transformPos(p3);
    m_p4 = transform.transformPos(p4);
}

void AtlasAnimationComponent::updateAnimation( f32 _deltaTime )
{
    m_timer = m_timer + getTemplate()->getPlayRate() * _deltaTime; 

    u32 add = (u32)(m_timer/_deltaTime);
    if (add > 0)
        m_timer = f32_Modulo(m_timer,f32(add) * _deltaTime);

    m_currentIndex = (m_currentIndex + add) % m_numIndices;
}

void AtlasAnimationComponent::updateAABB()
{
    AABB aabb(m_p1);
    aabb.grow(m_p2);
    aabb.grow(m_p3);
    aabb.grow(m_p4);
    m_actor->growRelativeAABBFromAbsolute(aabb);
}

//-------------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(AtlasAnimationComponent_Template)
BEGIN_SERIALIZATION_CHILD(AtlasAnimationComponent_Template)

SERIALIZE_MEMBER("texture",m_textureFile);
SERIALIZE_MEMBER("playRate",m_playRate);
SERIALIZE_MEMBER("width",m_width);
SERIALIZE_MEMBER("height",m_height);

END_SERIALIZATION()

AtlasAnimationComponent_Template::AtlasAnimationComponent_Template()
: m_width(1.0f)
, m_height(1.0f)
, m_playRate(1.0f)
{
}

}



