#ifndef _ITF_RAY_SHOWERTREECOMPONENT_H_
#define _ITF_RAY_SHOWERTREECOMPONENT_H_

namespace ITF
{
    class Ray_ShowerTreeComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShowerTreeComponent, ActorComponent,126341274)
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ShowerTreeComponent();
        virtual ~Ray_ShowerTreeComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        Update( f32 _dt );
#ifdef ITF_SUPPORT_EDITOR
        virtual void        drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags  ) const;
#endif // ITF_SUPPORT_EDITOR

    private:
        ITF_INLINE const class Ray_ShowerTreeComponent_Template* getTemplate() const;

        AnimLightComponent* m_animComponent;
        AnimatedComponent*  m_animatedComponent;
    };


    //---------------------------------------------------------------------------------------------------

    class Ray_ShowerTreeComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ShowerTreeComponent_Template, TemplateActorComponent,831550084);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ShowerTreeComponent);

    public:

        Ray_ShowerTreeComponent_Template()
            : m_minAngle(btrue, -45)
            , m_maxAngle(btrue, +45)
        {
        }
        virtual ~Ray_ShowerTreeComponent_Template() {}

        const StringID&     getInput() const { return m_input; }
        f32                 getMinAngle() const { return m_minAngle.ToRadians(); }
        f32                 getMaxAngle() const { return m_maxAngle.ToRadians(); }
        const Vec2d&        getPosOffset() const { return m_posOffset; }
        f32                 getAngleOffset() const { return m_angleOffset.ToRadians(); }

    private:

        StringID            m_input;
        Angle               m_minAngle;
        Angle               m_maxAngle;
        Vec2d               m_posOffset;
        Angle               m_angleOffset;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_ShowerTreeComponent_Template* Ray_ShowerTreeComponent::getTemplate() const
    {
        return static_cast<const Ray_ShowerTreeComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAY_SHOWERTREECOMPONENT_H_