#ifndef _ITF_RAY_SNAKEBODYPART_MIXED_H_
#define _ITF_RAY_SNAKEBODYPART_MIXED_H_

#ifndef _ITF_RAY_SNAKEAICOMPONENT_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeAIComponent.h"
#endif //_ITF_RAY_SNAKEAICOMPONENT_H_

#ifndef _ITF_RAY_SNAKEBODYPARTRENDERER_H_
#include "rayman/gameplay/Components/AI/Ray_SnakeBodyPartRenderer.h"
#endif //_ITF_RAY_SNAKEBODYPARTRENDERER_H_

namespace ITF
{
    /*
        This body part can be rendered either with a sprite or an actor.
    */


    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPart
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    class BodyPart : public BodyPartBase
    {
        DECLARE_OBJECT_CHILD_RTTI(BodyPart, BodyPartBase,3043470114)

    public:

        ITF_INLINE const class BodyPart_Template* getTemplate() const;

        BodyPart()
            : m_renderer(NULL)

            , m_rightFirstEdgeIdx( U32_INVALID )
            , m_rightLastEdgeIdx( U32_INVALID )
            , m_leftFirstEdgeIdx( U32_INVALID )
            , m_leftLastEdgeIdx( U32_INVALID )

            // Destructible
            , m_curHealth(-1)
            , m_isDead(bfalse)
        {
        }

        virtual void        onInit();
        virtual void        onBecomeActive();
        virtual void        clear() { m_renderer->clear(); }

        virtual bbool       validate() { return m_renderer->validate(); }
        virtual void        update( f32 _dt );
        virtual void        onEvent( Event* _event );
        virtual void        positionOnTrajectory( const Vec3d& _pos, const f32 _angle ) { m_renderer->positionOnTrajectory(_pos, _angle, this); }
        virtual AABB        getAABB() const { return m_renderer->getAABB(this); }

        ITF_INLINE bbool    isHead() const { return getIndex() == 0; }
        ITF_INLINE bbool    isTail() const { return getIndex() == getBodyPartCount()-1; }
        ITF_INLINE bbool    isBody() const { return !isHead() && !isTail(); }


        ITF_INLINE const Vec3d& getPos() const { return m_renderer->getPos(); }
        ITF_INLINE void     setPos( const Vec3d& _pos ) { m_renderer->setPos(_pos); }
        ITF_INLINE const Vec2d& get2DPos() const { return m_renderer->get2DPos(); }
        ITF_INLINE void     set2DPos( const Vec2d& _pos ) { m_renderer->set2DPos(_pos); }
        ITF_INLINE f32      getAngle() const { return m_renderer->getAngle(); }
        ITF_INLINE void     setAngle( const f32 _angle ) { m_renderer->setAngle(_angle); }


        // SPECIFIC TO RenderMode_Actor
        ITF_INLINE Actor*   getActor() const { ITF_ASSERT(m_renderMode == RenderMode_Actor); return m_actorRenderer.m_actor; }
        ITF_INLINE void     setActor( Actor* _value ) { ITF_ASSERT(m_renderMode == RenderMode_Actor); m_actorRenderer.m_actor = _value; }
        ITF_INLINE ActorRef getActorRef() const { ITF_ASSERT(m_renderMode == RenderMode_Actor); return m_actorRenderer.m_actorRef; }
        ITF_INLINE void     setActorRef( const ActorRef _value ) { m_actorRenderer.m_actorRef = _value; }
        ITF_INLINE AnimLightComponent* getAnimComponent() const { ITF_ASSERT(m_renderMode == RenderMode_Actor); return m_actorRenderer.m_animComponent; }
        ITF_INLINE AnimatedComponent* getAnimatedComponent() const { ITF_ASSERT(m_renderMode == RenderMode_Actor); return m_actorRenderer.m_animatedComponent; }


        // SPECIFIC TO RenderMode_Sprite
        virtual void        addPolylinePoints_sprite( PolyLine& _poly );
        virtual void        addOtherPolylinePoints_sprite( PolyLine& _poly );
        virtual void        draw_sprite( AtlasDynamicObject& _atlas ) { ITF_ASSERT(m_renderMode == RenderMode_Sprite); m_spriteRenderer.draw(_atlas, this); }



        // SPECIFIC TO DESTRUCTIBLES
        virtual bbool       receiveHit( HitStim* _hitStim, bbool _leftSide );
        virtual bbool       processHitStim( HitStim* _hitStim ) { return bfalse; }

        virtual bbool       removeAndKeepAliveOnDeath() { return bfalse; } // called on destruction call to keep dead part update 
        virtual void        startKeepAlive() { m_renderer->startKeepAlive(); }
        virtual bbool       updateKeepAlive( f32 _dt )  { return btrue; }   // returns true if the part need to be destroyed

        ITF_INLINE bbool    isDead()                                const { return m_isDead; }
        ITF_INLINE bbool    isInvincible()                          const { return ( m_curHealth < 0 ); }
        i32                 m_curHealth;
        bbool               m_isDead;


        // used to distribute hits on the shell
        u32                 m_rightFirstEdgeIdx;    // idx of the first right edge
        u32                 m_rightLastEdgeIdx;     // idx of the last right edge
        u32                 m_leftFirstEdgeIdx;     // idx of the first left edge
        u32                 m_leftLastEdgeIdx;      // idx of the last left edge


#ifdef ITF_SUPPORT_EDITOR
        virtual void onEditorMove();
#endif // ITF_SUPPORT_EDITOR

    protected:

        BodyPartRenderer*   m_renderer;

        void                playAnim( const StringID & _anim );
        bbool               isAnimFinished() { return m_renderer->isAnimFinished(); }
        virtual void        onAnimFinished();
        StringID            m_curPlayingAnim;


        // SPECIFIC TO RenderMode_Actor
        virtual Actor*      createBodyPart();
        virtual Actor*      spawnBodyPart();


    //private:

        // SPECIFIC TO RenderMode_Actor
        BodyPartActorRenderer m_actorRenderer;

        // SPECIFIC TO RenderMode_Sprite
        BodyPartSpriteRenderer m_spriteRenderer;
    };




    //**********************************************************************************************************************************
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //                                    BodyPart_Template
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //**********************************************************************************************************************************

    class BodyPart_Template : public BodyPartBase_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(BodyPart_Template, BodyPartBase_Template,2951019815);
        DECLARE_BODYPART_TEMPLATE(BodyPart);
        DECLARE_SERIALIZE();

    public:
        BodyPart_Template();

        ITF_INLINE const BodyPartActorRenderer_Template& getActorRenderer() const { return m_actorRenderer; }
        ITF_INLINE const BodyPartSpriteRenderer_Template& getSpriteRenderer() const { return m_spriteRenderer; }

        RenderMode getRenderMode() const { return m_actorRenderer.getActorPath().isEmpty() ? RenderMode_Sprite : RenderMode_Actor; }
        virtual bbool usePolyline() const { return (getRenderMode() == RenderMode_Sprite) ? m_spriteRenderer.usePolyline() : m_actorRenderer.usePolyline(); }


        ITF_INLINE i32              getHealth() const { return m_health; }
        ITF_INLINE bbool            destroyOnDeath() const { return m_destroyOnDeath; }
        ITF_INLINE const SafeArray<u32> &getDamageLevels() const { return m_damageLevels; }
        ITF_INLINE const StringID&  getLeftHitAnim() const { return m_leftHitAnim; }
        ITF_INLINE const StringID&  getRightHitAnim() const { return m_rightHitAnim; }
        ITF_INLINE const StringID&  getDeathAnim() const { return m_deathAnim; }

    private:

        BodyPartActorRenderer_Template m_actorRenderer;
        BodyPartSpriteRenderer_Template m_spriteRenderer;


        i32                 m_health;
        bbool               m_destroyOnDeath;
        SafeArray <u32>     m_damageLevels;
        StringID            m_leftHitAnim;
        StringID            m_rightHitAnim;
        StringID            m_deathAnim;
    };

    ITF_INLINE const BodyPart_Template* BodyPart::getTemplate() const { return static_cast<const BodyPart_Template*>(m_template); }
};

#endif //_ITF_RAY_SNAKEBODYPART_MIXED_H_
