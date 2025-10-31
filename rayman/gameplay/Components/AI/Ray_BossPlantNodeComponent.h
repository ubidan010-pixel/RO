#ifndef _ITF_RAY_BOSSPLANTNODECOMPONENT_H_
#define _ITF_RAY_BOSSPLANTNODECOMPONENT_H_

#ifndef _ITF_RAY_GAMEPLAYEVENTS_H_
#include "rayman/gameplay/Ray_GameplayEvents.h"
#endif //_ITF_RAY_GAMEPLAYEVENTS_H_

namespace ITF
{
    class LinkComponent;

    class Ray_BossPlantNodeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossPlantNodeComponent, ActorComponent,4281200548)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_BossPlantNodeComponent();
        virtual ~Ray_BossPlantNodeComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        Update( f32 _dt );
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
        virtual void        onEditorCreated( Actor* _original );
#endif // ITF_SUPPORT_EDITOR

        Ray_BossPlantNodeComponent* getNextNode() const;
        Ray_BossPlantNodeComponent* getPrevNode() const;

        bbool               isStationary() const;
        const Vec2d&        getCycleVector() const;
        f32                 getRawCycleCount( const Vec2d& _movement, bbool* _exact = NULL ) const;
        u32                 getCycleCount( const Vec2d& _movement ) const { return getCycleCount( getRawCycleCount(_movement) ); }
        u32                 getCycleCount( f32 _rawCycleCount ) const;

        const StringID&     getAnim() const;
        f32                 getAnimPlayRate() const;

        void                onNodeReached( Actor* _activator );

#ifdef ITF_SUPPORT_DEBUGFEATURE
        ITF_INLINE static bbool getDrawDebug() { return s_drawDebug; }
        ITF_INLINE static void  setDrawDebug( bbool _value ) { s_drawDebug = _value; }
#endif // ITF_SUPPORT_DEBUGFEATURE

    private:

        ITF_INLINE const class Ray_BossPlantNodeComponent_Template* getTemplate() const;

        LinkComponent* m_linkComponent;

#ifdef ITF_SUPPORT_DEBUGFEATURE
        typedef FixedArray<const Ray_BossPlantNodeComponent*, 128> ProcessedNodeList;
        void        drawDebugRoot() const;
        void        drawDebug( ProcessedNodeList& _processedNodes ) const;
        void        drawEmptyCircles() const;

        static bbool s_drawDebug;
#endif // ITF_SUPPORT_DEBUGFEATURE
    };


    //-------------------------------------------------------------------------------------

    class Ray_BossPlantNodeComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_BossPlantNodeComponent_Template, TemplateActorComponent,1110965916);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_BossPlantNodeComponent);

    public:

        Ray_BossPlantNodeComponent_Template();
        ~Ray_BossPlantNodeComponent_Template() {}

        const StringID&             getAnim() const { return m_anim; }
        f32                         getAnimPlayRate() const { return m_animPlayRate; }
        const Vec2d&                getCycleVector() const { return m_cycleVector; }
        const Ray_EventNodeReached& getEventNodeReached() const { return m_eventNodeReached; }

    private:

        StringID                m_anim;
        f32                     m_animPlayRate;
        Vec2d                   m_cycleVector;
        Ray_EventNodeReached    m_eventNodeReached;
    };


    //-------------------------------------------------------------------------------------

    ITF_INLINE const Ray_BossPlantNodeComponent_Template* Ray_BossPlantNodeComponent::getTemplate() const
    {
        return static_cast<const Ray_BossPlantNodeComponent_Template*>(m_template);
    }

    ITF_INLINE bbool Ray_BossPlantNodeComponent::isStationary() const
    {
        return getTemplate()->getCycleVector().IsEqual(Vec2d::Zero, MTH_EPSILON);
    }

    ITF_INLINE const Vec2d& Ray_BossPlantNodeComponent::getCycleVector() const
    {
        return getTemplate()->getCycleVector();
    }

    ITF_INLINE const StringID& Ray_BossPlantNodeComponent::getAnim() const
    {
        return getTemplate()->getAnim();
    }

    ITF_INLINE f32 Ray_BossPlantNodeComponent::getAnimPlayRate() const
    {
        return getTemplate()->getAnimPlayRate();
    }

};

#endif //_ITF_RAY_BOSSPLANTNODECOMPONENT_H_