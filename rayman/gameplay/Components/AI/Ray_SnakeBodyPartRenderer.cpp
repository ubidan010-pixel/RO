#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_SNAKEBODYPARTRENDERER_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPartRenderer.h"
#endif //_ITF_RAY_SNAKEBODYPARTRENDERER_H_

#ifndef _ITF_RAY_SNAKEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeAIComponent.h"
#endif //_ITF_RAY_SNAKEAICOMPONENT_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{
    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPartActorRenderer_Template
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    BEGIN_SERIALIZATION(BodyPartActorRenderer_Template)
        SERIALIZE_MEMBER("actorPath", m_actorPath);
        SERIALIZE_MEMBER("polyline", m_polyline);
        SERIALIZE_MEMBER("otherPolyline", m_otherPolyline);
        SERIALIZE_MEMBER("endAnimMRK", m_endAnimMRK);
    END_SERIALIZATION()


    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPartActorRenderer
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    void BodyPartActorRenderer::onInit( const BodyPartActorRenderer_Template* _template, const BodyPartBase* _owner )
    {
        m_template = _template;
        SPAWNER->declareNeedsSpawnee(_owner->getParent(), &m_spawner, getTemplate()->getActorPath());
    }

    void BodyPartActorRenderer::clear()
    {
        if ( Actor* actor = m_actorRef.getActor() )
        {
            AIUtils::setAlwaysActive(actor, bfalse);
            actor->requestDestruction();
        }
    }

    void BodyPartActorRenderer::startKeepAlive()
    {
        if ( Actor* actor = m_actorRef.getActor() )
        {
            AIUtils::setAlwaysActive( actor, bfalse );
        }
    }

    bbool BodyPartActorRenderer::validate()
    {
        m_actor = m_actorRef.getActor();
        if ( !m_actor )
        {
            ITF_ASSERT(0);
            return bfalse;
        }

        m_animComponent = m_actor->GetComponent<AnimLightComponent>();
        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        if ( !m_animComponent )
        {
            ITF_ASSERT(0);
            return bfalse;
        }

        if ( !m_animComponent->isLoaded() )
        {
            //ITF_ASSERT(0);
            return bfalse;
        }

        return btrue;
    }

    void BodyPartActorRenderer::positionOnTrajectory( const Vec3d& _pos, const f32 _angle, const BodyPartBase* _owner )
    {
        m_actor->setPos(_pos);
        m_actor->setAngle(_angle);
        m_actor->setScale(_owner->getParent()->getScale());    // TODO: scale should only be set on init and onScaleChanged
        m_actor->setFlipped(_owner->getParent()->isFlipped());
    }



    bbool BodyPartActorRenderer::playAnim( const StringID& _anim, const bbool _reset /*= btrue*/ )
    {
        ITF_ASSERT_MSG(m_animComponent, "should only be called from update, otherwise we don't guaranty that m_animComponent is valid");
        if ( m_animComponent && _anim.isValid() )
        {
            if (_reset)
            {
                m_animComponent->resetCurTime();
            }
            m_animComponent->setAnim(_anim);
            return btrue;
        }

        return bfalse;
    }

    bbool BodyPartActorRenderer::isAnimFinished()
    {
        ITF_ASSERT_MSG(m_animComponent, "should only be called from update, otherwise we don't guaranty that m_animComponent is valid");
        if (m_animComponent)
        {
            ITF_ASSERT(!AIUtils::isAnimLooped(m_animComponent, m_animatedComponent));
            return AIUtils::isAnimFinished(m_animComponent, m_animatedComponent);
        }

        return bfalse;
    }

    bbool BodyPartActorRenderer::isAnimLooped()
    {
        return AIUtils::isAnimLooped(m_animComponent, m_animatedComponent);
    }






    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPartSpriteRenderer_Template
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    BEGIN_SERIALIZATION(BodyPartSpriteRenderer_Template)
        SERIALIZE_OBJECT("anim", m_anim);
        SERIALIZE_OBJECT("drawAABB", m_drawAABB);
        SERIALIZE_MEMBER("color", m_color);
        SERIALIZE_MEMBER("polyline", m_polyline);
        SERIALIZE_MEMBER("otherPolyline", m_otherPolyline);
    END_SERIALIZATION()


    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPartSpriteRenderer
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    void BodyPartSpriteRenderer::onInit( const BodyPartSpriteRenderer_Template* _template )
    {
        m_template = _template;
        m_animPlayer.init(&getTemplate()->getAnim());
        m_color = _template->getColor();
    }

    void BodyPartSpriteRenderer::update( const f32 _dt )
    {
        m_animPlayer.update();
    }

    void BodyPartSpriteRenderer::positionOnTrajectory( const Vec3d& _pos, const f32 _angle, const BodyPartBase* _owner )
    {
        m_pos = _pos;
        m_angle = _angle;
    }

    AABB BodyPartSpriteRenderer::getAABB( const BodyPartBase* _owner ) const
    {
        const Actor* parent = _owner->getParent();
        const Transform2d transform(m_pos.truncateTo2D(), m_angle, parent->isFlipped());
        const Vec2d min = getTemplate()->getDrawAABB().getMin() * parent->getScale();
        const Vec2d max = getTemplate()->getDrawAABB().getMax() * parent->getScale();

        const Vec2d p1 = transform.transformPos( Vec2d(min.m_x, max.m_y) );
        const Vec2d p2 = transform.transformPos( Vec2d(min.m_x, min.m_y) );
        const Vec2d p3 = transform.transformPos( Vec2d(max.m_x, min.m_y) );
        const Vec2d p4 = transform.transformPos( Vec2d(max.m_x, max.m_y) );

        AABB aabb(p1);
        aabb.grow(p2);
        aabb.grow(p3);
        aabb.grow(p4);
        return aabb;
    }

    void BodyPartSpriteRenderer::draw( AtlasDynamicObject& _atlas, const BodyPartBase* _owner )
    {
        /*
            NOTE:
             - m_pos is the most forward point in the trajectory
             - length (template, scaled) gives us the pivot point for the next part
             - this is how the atlas expects the quad:
                    p1 ----- p4
                    |        |
                    |        |
                    p2 ----- p3
        */

        const Actor* parent = _owner->getParent();
        const Transform2d transform(m_pos.truncateTo2D(), m_angle, parent->isFlipped());
        const Vec2d min = getTemplate()->getDrawAABB().getMin() * parent->getScale();
        const Vec2d max = getTemplate()->getDrawAABB().getMax() * parent->getScale();

        const Vec2d p1 = transform.transformPos( Vec2d(min.m_x, max.m_y) );
        const Vec2d p2 = transform.transformPos( Vec2d(min.m_x, min.m_y) );
        const Vec2d p3 = transform.transformPos( Vec2d(max.m_x, min.m_y) );
        const Vec2d p4 = transform.transformPos( Vec2d(max.m_x, max.m_y) );

        _atlas.addQuad(m_animPlayer.getCurAtlas(), p1, p2, p3, p4, m_pos.m_z, m_color.getAsU32());


        // DEBUG
        //{
        //    const Transform3d transform(m_pos, m_angle, parent->isFlipped());
        //    String s;s.setTextFormat("%d", _owner->getIndex());
        //    AABB aabb = getTemplate()->getDrawAABB();
        //    aabb.setMin(aabb.getMin() * parent->getScale());
        //    aabb.setMax(aabb.getMax() * parent->getScale());
        //    DebugDraw::AABB(transform, aabb, Color::red(), 0, 5, s);
        //}
    }

    u32 BodyPartSpriteRenderer::addPolylinePoints( PolyLine& _poly, const BodyPartBase* _owner )
    {
        const Actor* parent = _owner->getParent();
        const Transform2d transform(m_pos.truncateTo2D(), m_angle, parent->isFlipped());
        const Vec2d& scale = parent->getScale();

        for (u32 i = 0; i < getTemplate()->getPolyline().size(); i++)
        {
            const Vec2d& point = getTemplate()->getPolyline()[i];
            _poly.addPoint(transform.transformPos(point * scale));
        }
      
        // return the edge idx then ( pos count - 1 )
        return ( getTemplate()->getPolyline().size() > 0 ) ? _poly.getPosCount() - 1 : _poly.getPosCount();
    }

    u32 BodyPartSpriteRenderer::addOtherPolylinePoints( PolyLine& _poly, const BodyPartBase* _owner )
    {
        const Actor* parent = _owner->getParent();
        const Transform2d transform(m_pos.truncateTo2D(), m_angle, parent->isFlipped());
        const Vec2d& scale = parent->getScale();

        for (u32 i = 0; i < getTemplate()->getOtherPolyline().size(); i++)
        {
            const Vec2d& point = getTemplate()->getOtherPolyline()[i];
            _poly.addPoint(transform.transformPos(point * scale));
        }

        // return the edge idx then ( pos count - 1 )
        return ( getTemplate()->getPolyline().size() > 0 ) ? _poly.getPosCount() - 1 : _poly.getPosCount();
    }

};