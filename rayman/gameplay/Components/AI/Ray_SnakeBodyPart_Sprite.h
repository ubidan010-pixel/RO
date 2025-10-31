#ifndef _ITF_RAY_SNAKEBODYPART_SPRITE_H_
#define _ITF_RAY_SNAKEBODYPART_SPRITE_H_

#ifndef _ITF_RAY_SNAKEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeAIComponent.h"
#endif //_ITF_RAY_SNAKEAICOMPONENT_H_

#ifndef _ITF_RAY_SNAKEBODYPARTRENDERER_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPartRenderer.h"
#endif //_ITF_RAY_SNAKEBODYPARTRENDERER_H_

namespace ITF
{
    /*
        Super-simple version that does nothing but draw a sprite.
    */

    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPartSprite
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    class BodyPartSprite : public BodyPartBase
    {
        DECLARE_OBJECT_CHILD_RTTI(BodyPartSprite, BodyPartBase,802355147)

    public:

        ITF_INLINE const class BodyPartSprite_Template* getTemplate() const;

        BodyPartSprite()
        {
            m_renderMode = RenderMode_Sprite;
        }

        virtual void        onInit();
        virtual void        update( f32 _dt ) { m_renderer.update(_dt); }
        virtual void        positionOnTrajectory( const Vec3d& _pos, const f32 _angle ) { m_renderer.positionOnTrajectory(_pos, _angle, this); }
        virtual AABB        getAABB() const { return m_renderer.getAABB(this); }
        virtual void        draw_sprite( AtlasDynamicObject& _atlas ) { m_renderer.draw(_atlas, this); }

        virtual void        addPolylinePoints_sprite( PolyLine& _poly ) { m_renderer.addPolylinePoints(_poly, this); }
        virtual void        addOtherPolylinePoints_sprite( PolyLine& _poly ) { m_renderer.addOtherPolylinePoints(_poly, this); }

    protected:

        BodyPartSpriteRenderer m_renderer;
    };


    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPartSprite_Template
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    class BodyPartSprite_Template : public BodyPartBase_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(BodyPartSprite_Template, BodyPartBase_Template,4078730029);
        DECLARE_BODYPART_TEMPLATE(BodyPartSprite);
        DECLARE_SERIALIZE();

    public:

        BodyPartSprite_Template() {}

        ITF_INLINE const BodyPartSpriteRenderer_Template& getRenderer() const { return m_renderer; }

        virtual bbool usePolyline() const { return m_renderer.usePolyline(); }

    private:

        BodyPartSpriteRenderer_Template m_renderer;
    };

    ITF_INLINE const BodyPartSprite_Template* BodyPartSprite::getTemplate() const { return static_cast<const BodyPartSprite_Template*>(m_template); }
};

#endif //_ITF_RAY_SNAKEBODYPART_SPRITE_H_
