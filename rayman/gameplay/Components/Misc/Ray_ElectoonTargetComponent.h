#ifndef _ITF_RAY_ELECTOONTARGETCOMPONENT_H_
#define _ITF_RAY_ELECTOONTARGETCOMPONENT_H_

namespace ITF
{
    class Ray_ElectoonTargetComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ElectoonTargetComponent, ActorComponent,3976685342);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        Ray_ElectoonTargetComponent();
        virtual ~Ray_ElectoonTargetComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        f32 getMinRange() const;
        f32 getMaxRange() const;

        static f32 getDeltaXFromCageToTarget(f32 _dx, f32 _landingDistance);

    private:
        ITF_INLINE const class Ray_ElectoonTargetComponent_Template* getTemplate() const;

    };


    //---------------------------------------------------------------------------------------------------

    class Ray_ElectoonTargetComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ElectoonTargetComponent_Template, TemplateActorComponent,853125459);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ElectoonTargetComponent);

    public:
        Ray_ElectoonTargetComponent_Template() : m_typicalMinRange(1.5f), m_typicalMaxRange(2.5f) {}
        virtual ~Ray_ElectoonTargetComponent_Template() {}

        f32 getTypicalMinRange() const {return m_typicalMinRange;}
        f32 getTypicalMaxRange() const {return m_typicalMaxRange;}

    private:
        f32 m_typicalMinRange, m_typicalMaxRange;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_ElectoonTargetComponent_Template* Ray_ElectoonTargetComponent::getTemplate() const
    {
        return static_cast<const Ray_ElectoonTargetComponent_Template*>(m_template);
    }
};

#endif //_ITF_Ray_ElectoonTargetComponent_H_