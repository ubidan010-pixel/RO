#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SNAKEBODYPART_SPRITE_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPart_Sprite.h"
#endif //_ITF_RAY_SNAKEBODYPART_SPRITE_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(BodyPartSprite_Template)

    BEGIN_SERIALIZATION_CHILD(BodyPartSprite_Template)
        SERIALIZE_OBJECT("spriteRenderer", m_renderer);
    END_SERIALIZATION()


    IMPLEMENT_OBJECT_RTTI(BodyPartSprite)

    void BodyPartSprite::onInit()
    {
        Super::onInit();

        m_renderer.onInit(&getTemplate()->getRenderer());
    }

};