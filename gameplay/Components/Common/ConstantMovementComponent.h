#ifndef _ITF_CONSTANTMOVEMENTCOMPONENT_H_
#define _ITF_CONSTANTMOVEMENTCOMPONENT_H_

namespace ITF
{
    class ConstantMovementComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ConstantMovementComponent, ActorComponent,3168343774);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        ConstantMovementComponent();
        virtual ~ConstantMovementComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        Update( f32 _dt );

        ITF_INLINE const Vec3d& getWorldSpeed() const;
    private:

        ITF_INLINE const class ConstantMovementComponent_Template*  getTemplate() const;

    };

    //-------------------------------------------------------------------------------------

    class ConstantMovementComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(ConstantMovementComponent_Template,TemplateActorComponent,1235751057);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(ConstantMovementComponent);

    public:

        ConstantMovementComponent_Template();
        ~ConstantMovementComponent_Template() {}

        const Vec2d&            getLocalSpeed() const { return m_localSpeed; }
        const Vec3d&            getWorldSpeed() const { return m_worldSpeed; }
        const Angle&            getWorldAngularSpeed() const { return m_worldAngularSpeed; }

    private:

        Vec2d   m_localSpeed;
        Vec3d   m_worldSpeed;
        Angle   m_worldAngularSpeed;
    };


    const ConstantMovementComponent_Template*  ConstantMovementComponent::getTemplate() const {return static_cast<const ConstantMovementComponent_Template*>(m_template);}
    ITF_INLINE const Vec3d& ConstantMovementComponent::getWorldSpeed() const { return getTemplate()->getWorldSpeed(); }
};

#endif //_ITF_CONSTANTMOVEMENTCOMPONENT_H_