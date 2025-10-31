#ifndef _ITF_RAY_SNAKEBODYPARTRENDERER_H_
#define _ITF_RAY_SNAKEBODYPARTRENDERER_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif //_ITF_SPAWNER_H_

#ifndef _ITF_ATLASDYNAMICOBJECT_H_
#include "engine/display/AtlasDynamicObject.h"
#endif //_ITF_ATLASDYNAMICOBJECT_H_

#ifndef _ITF_ANIMATIONATLAS_H_
#include "gameplay/AI/Utils/AnimationAtlas.h"
#endif //_ITF_ANIMATIONATLAS_H_

namespace ITF
{
    class BodyPartBase;

    class BodyPartRenderer
    {
    public:

        virtual void    clear() {}
        virtual void    startKeepAlive() {}
        virtual bbool   validate() { return btrue; }
        virtual void    update( const f32 _dt ) {}
        virtual void    positionOnTrajectory( const Vec3d& _pos, const f32 _angle, const BodyPartBase* _owner ) = 0;
        virtual AABB    getAABB( const BodyPartBase* _owner ) const = 0;

        virtual bbool   playAnim( const StringID& _anim, const bbool _reset = btrue ) = 0;
        virtual bbool   isAnimFinished() = 0;
        virtual bbool   isAnimLooped() = 0;

        virtual const Vec3d& getPos() const = 0;
        virtual void    setPos( const Vec3d& _pos ) = 0;
        virtual const Vec2d& get2DPos() const = 0;
        virtual void    set2DPos( const Vec2d& _pos ) = 0;
        virtual f32     getAngle() const = 0;
        virtual void    setAngle( const f32 _angle ) = 0;
    };


    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPartActorRenderer
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    class BodyPartActorRenderer_Template;

    class BodyPartActorRenderer : public BodyPartRenderer
    {
    public:
        BodyPartActorRenderer()
            : m_template(NULL)
            , m_actor(NULL)
            , m_animComponent(NULL)
            , m_animatedComponent(NULL)
        {
        }

                void    onInit( const BodyPartActorRenderer_Template* _template, const BodyPartBase* _owner );
        ITF_INLINE const BodyPartActorRenderer_Template* getTemplate() const { return m_template; }
        virtual void    clear();
        virtual void    startKeepAlive();

        virtual bbool   validate();
        virtual void    positionOnTrajectory( const Vec3d& _pos, const f32 _angle, const BodyPartBase* _owner );
        virtual AABB    getAABB( const BodyPartBase* _owner ) const { return m_actor->getAABB(); }

        virtual bbool   playAnim( const StringID& _anim, const bbool _reset = btrue );
        virtual bbool   isAnimFinished();
        virtual bbool   isAnimLooped();

        virtual const Vec3d& getPos() const { return m_actor->getPos(); }
        virtual void    setPos( const Vec3d& _pos ) { m_actor->setPos(_pos); }
        virtual const Vec2d& get2DPos() const { return m_actor->get2DPos(); }
        virtual void    set2DPos( const Vec2d& _pos ) { m_actor->set2DPos(_pos); }
        virtual f32     getAngle() const { return m_actor->getAngle(); }
        virtual void    setAngle( const f32 _angle ) { m_actor->setAngle(_angle); }

    //private:
        const BodyPartActorRenderer_Template* m_template;

        SpawneeGenerator    m_spawner;
        ActorRef            m_actorRef;
        // CAUTION: these are only valid for the current frame
        Actor*              m_actor;
        AnimLightComponent* m_animComponent;
        AnimatedComponent*  m_animatedComponent;
    };


    class BodyPartActorRenderer_Template
    {
        DECLARE_SERIALIZE();

    public:

        BodyPartActorRenderer_Template()
        {
        }

        ITF_INLINE const Path& getActorPath() const { return m_actorPath; }
        ITF_INLINE const StringID& getPolyline() const { return m_polyline; }
        ITF_INLINE const StringID& getOtherPolyline() const { return m_otherPolyline; }
        ITF_INLINE const StringID& getEndAnimMRK() const { return m_endAnimMRK; }

        ITF_INLINE bbool usePolyline() const { return m_polyline.isValid() || m_otherPolyline.isValid(); }

    private:

        Path m_actorPath;
        StringID m_polyline;
        StringID m_otherPolyline;
        StringID m_endAnimMRK;
    };



    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPartSpriteRenderer
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    typedef SafeArray<Vec2d> SpritePolyline;

    class BodyPartSpriteRenderer_Template;

    class BodyPartSpriteRenderer : public BodyPartRenderer
    {
    public:

        BodyPartSpriteRenderer()
            : m_template(NULL)
            , m_pos(Vec3d::Zero)
            , m_angle(0.f)
            , m_color(Color::white())
        {
        }

                void    onInit( const BodyPartSpriteRenderer_Template* _template );
        ITF_INLINE const BodyPartSpriteRenderer_Template* getTemplate() const { return m_template; }

        virtual void    update( const f32 _dt );
        virtual void    positionOnTrajectory( const Vec3d& _pos, const f32 _angle, const BodyPartBase* _owner );
        virtual AABB    getAABB( const BodyPartBase* _owner ) const;
                void    draw( AtlasDynamicObject& _atlas, const BodyPartBase* _owner );

        virtual bbool   playAnim( const StringID& _anim, const bbool _reset = btrue ) { ITF_ASSERT_MSG(0, "Implement me"); /*m_animPlayer.init(_anim);*/ return btrue; }
        virtual bbool   isAnimFinished() { return bfalse; }
        virtual bbool   isAnimLooped() { return btrue; }

                u32     addPolylinePoints( PolyLine& _poly, const BodyPartBase* _owner );       // return the last poly edge idx
                u32     addOtherPolylinePoints( PolyLine& _poly, const BodyPartBase* _owner );  // return the last poly edge idx

        virtual const Vec3d& getPos() const { return m_pos; }
        virtual void    setPos( const Vec3d& _pos ) { m_pos = _pos; }
        virtual const Vec2d& get2DPos() const { return m_pos.truncateTo2D(); }
        virtual void    set2DPos( const Vec2d& _pos ) { m_pos.m_x = _pos.m_x; m_pos.m_y = _pos.m_y; }
        virtual f32     getAngle() const { return m_angle; }
        virtual void    setAngle( const f32 _angle ) { m_angle = _angle; }

        ITF_INLINE const Color& getColor() const { return m_color; }
        ITF_INLINE void setColor( const Color& _value ) { m_color = _value; }

    private:

        const BodyPartSpriteRenderer_Template* m_template;

        Vec3d   m_pos;
        f32     m_angle;
        AnimationAtlasPlayer m_animPlayer;
        Color   m_color;
    };


    class BodyPartSpriteRenderer_Template
    {
        DECLARE_SERIALIZE();

    public:

        BodyPartSpriteRenderer_Template() 
            : m_drawAABB(Vec2d(-0.5f, -1.f), Vec2d(0.5f, 0.f))
            , m_color(Color::white())
        {
        }

        ITF_INLINE const AnimationAtlas& getAnim() const { return m_anim; }
        ITF_INLINE const AABB& getDrawAABB() const { return m_drawAABB; }
        ITF_INLINE const Color& getColor() const { return m_color; }
        ITF_INLINE const SpritePolyline& getPolyline() const { return m_polyline; }
        ITF_INLINE const SpritePolyline& getOtherPolyline() const { return m_otherPolyline; }

        ITF_INLINE bbool usePolyline() const { return m_polyline.size() > 0 || m_otherPolyline.size() > 0; }

    private:

        AnimationAtlas m_anim;
        AABB m_drawAABB;
        Color m_color;
        SpritePolyline m_polyline;
        SpritePolyline m_otherPolyline;
    };

};

#endif //_ITF_RAY_SNAKEBODYPARTRENDERER_H_
