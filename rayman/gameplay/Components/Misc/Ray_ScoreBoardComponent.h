#ifndef _ITF_RAYSCOREBOARDCOMPONENT_H_
#define _ITF_RAYSCOREBOARDCOMPONENT_H_

namespace ITF
{
    class Ray_ScoreBoardComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ScoreBoardComponent, ActorComponent,2003984371)
        DECLARE_SERIALIZE()

    public:

        Ray_ScoreBoardComponent();
        virtual ~Ray_ScoreBoardComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onBecomeActive();
        virtual void        onEvent(Event* _event);

    private:
        ITF_INLINE const class Ray_ScoreBoardComponent_Template* getTemplate() const;

        f32     m_timer;

    };


    //---------------------------------------------------------------------------------------------------

    class Ray_ScoreBoardComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(Ray_ScoreBoardComponent_Template, TemplateActorComponent,2670213952);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_ScoreBoardComponent);

    public:

        Ray_ScoreBoardComponent_Template()
            : m_baseName(StringID::Invalid)
        {
        }
        virtual ~Ray_ScoreBoardComponent_Template() {}

        ITF_INLINE const StringID& getBaseName() const { return m_baseName; }

    private:

        StringID    m_baseName;
    };


    //---------------------------------------------------------------------------------------------------

    ITF_INLINE const Ray_ScoreBoardComponent_Template* Ray_ScoreBoardComponent::getTemplate() const
    {
        return static_cast<const Ray_ScoreBoardComponent_Template*>(m_template);
    }
};

#endif //_ITF_RAYSCOREBOARDCOMPONENT_H_