#ifndef _ITF_RAY_EYEANIMATIONCOMPONENT_H_
#define _ITF_RAY_EYEANIMATIONCOMPONENT_H_

namespace ITF
{
    class Ray_EyeAnimationComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_EyeAnimationComponent, ActorComponent,2959174830)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_EyeAnimationComponent();
        virtual ~Ray_EyeAnimationComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onResourceReady();
        virtual void        Update( f32 _dt );

    private:
        ITF_INLINE const class Ray_EyeAnimationComponent_Template * getTemplate() const;

        AnimatedComponent*  m_animatedComponent;

        Vec2d       m_currentEyeCursor;
        u32         m_eyeInputIndexX;
        u32         m_eyeInputIndexY;
    };

    //-------------------------------------------------------------------------------------
    class Ray_EyeAnimationComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_EyeAnimationComponent_Template,TemplateActorComponent,2374979028);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_EyeAnimationComponent);

    public:

        Ray_EyeAnimationComponent_Template();
        ~Ray_EyeAnimationComponent_Template() {}

        f32                 getEyeInputSmoothFactor() const { return m_eyeInputSmoothFactor; }
        const StringID&     getEyeInputX() const { return m_eyeInputX; }
        const StringID&     getEyeInputY() const { return m_eyeInputY; }
        const Vec2d&        getEyeInputMin() const { return m_eyeInputMin; }
        const Vec2d&        getEyeInputMax() const { return m_eyeInputMax; }
        const Vec2d&        getTargetOffset() const { return m_targetOffset; }
        const Vec2d&        getEyeOffset() const { return m_eyeOffset; }
        f32                 getDistanceFactor() const { return m_distanceFactor; }

    private:

        f32         m_eyeInputSmoothFactor;
        StringID    m_eyeInputX;
        StringID    m_eyeInputY;
        Vec2d       m_eyeInputMin;
        Vec2d       m_eyeInputMax;
        Vec2d       m_targetOffset;
        Vec2d       m_eyeOffset;
        f32         m_distanceFactor;  // the closer the target, the closer the eye is to the center
    };
    ITF_INLINE const class Ray_EyeAnimationComponent_Template * Ray_EyeAnimationComponent::getTemplate() const
    {
        return static_cast<const class Ray_EyeAnimationComponent_Template *>(m_template);
    }
};

#endif //_ITF_RAY_EYEANIMATIONCOMPONENT_H_