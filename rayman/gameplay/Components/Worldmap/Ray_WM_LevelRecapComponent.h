#ifndef _ITF_RAYWMLEVELRECAPCOMPONENT_H_
#define _ITF_RAYWMLEVELRECAPCOMPONENT_H_

namespace ITF
{
    class Ray_WM_LevelRecapComponent : public ActorComponent, public IUpdateAnimInputCriteria
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_LevelRecapComponent, ActorComponent, 1226660666)
        DECLARE_SERIALIZE();

    public:

        Ray_WM_LevelRecapComponent()
            : m_electoons(0)
            , m_levelTag(StringID::Invalid)
            , m_animatedComponent(NULL)
            , m_state(STATE_HIDDEN)
            , m_scaleStart(Vec2d::One)
            , m_scaleEnd(Vec2d::One)
            , m_transitionTime(0.0f)
            , m_currentTransitionTime(0.0f)
        {};
        virtual ~Ray_WM_LevelRecapComponent() {};

        virtual bbool       needsUpdate()   const { return btrue; }
        virtual bbool       needsDraw()     const { return bfalse; }
        virtual bbool       needsDraw2D()   const { return btrue; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        Update( f32 _dt );
        virtual void        onEvent(Event * _event){};
        virtual void        Draw2D();

        virtual void        updateAnimInput();

        ITF_INLINE void     setLevelTag(const StringID& _levelTag) { m_levelTag = _levelTag; }

        ITF_INLINE bbool    isHidden() { return m_state == STATE_HIDDEN; }

        void                show(const Vec2d& _scaleStart, const Vec2d& _scaleEnd, const float& _time);
        void                hide(const Vec2d& _scaleStart, const Vec2d& _scaleEnd, const float& _time);

    private:

        enum ELevelRecapState
        {
            STATE_DISPLAYING,
            STATE_DISPLAYED,
            STATE_HIDING,
            STATE_HIDDEN,
        };

        ITF_INLINE const class Ray_WM_LevelRecapComponent_Template* getTemplate() const;

        u32                 m_electoons;
        StringID            m_levelTag;

        AnimatedComponent*  m_animatedComponent;

        ELevelRecapState    m_state;

        Vec2d               m_scaleStart;
        Vec2d               m_scaleEnd;
        float               m_transitionTime;
        float               m_currentTransitionTime;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_WM_LevelRecapComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_WM_LevelRecapComponent_Template, TemplateActorComponent,1212497558);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_WM_LevelRecapComponent);

    public:

        Ray_WM_LevelRecapComponent_Template() {}
        virtual ~Ray_WM_LevelRecapComponent_Template() {}

    private:
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_WM_LevelRecapComponent_Template* Ray_WM_LevelRecapComponent::getTemplate() const
    {
        return static_cast<const Ray_WM_LevelRecapComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAYWMLEVELRECAPCOMPONENT_H_