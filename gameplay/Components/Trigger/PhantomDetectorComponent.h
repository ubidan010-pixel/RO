#ifndef _ITF_PHANTOMDETECTORCOMPONENT_H_
#define _ITF_PHANTOMDETECTORCOMPONENT_H_

#ifndef _ITF_SHAPEDETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/ShapeDetectorComponent.h"
#endif //_ITF_SHAPEDETECTORCOMPONENT_H_

namespace ITF
{
    class PhantomDetectorComponent : public ShapeDetectorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PhantomDetectorComponent, ShapeDetectorComponent,2329854250);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        PhantomDetectorComponent();
        virtual ~PhantomDetectorComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        Update( f32 _dt );

    private:
        const class PhantomDetectorComponent_Template* getTemplate() const;

    };

    //-------------------------------------------------------------------------------------
    class PhantomDetectorComponent_Template : public ShapeDetectorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(PhantomDetectorComponent_Template,ShapeDetectorComponent_Template,4113622071);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PhantomDetectorComponent);

    public:

        PhantomDetectorComponent_Template()
            : m_factionToDetect(FACTION_UNKNOWN)    // unknown = don't filter
            , m_allowDeadActors(bfalse)
        {
        }
        ~PhantomDetectorComponent_Template() {}

        ITF_INLINE Faction getFactionToDetect() const { return m_factionToDetect; }
        bbool              getAllowDeadActors() const { return m_allowDeadActors; }

    private:

        Faction     m_factionToDetect;
        bbool       m_allowDeadActors;
    };

    ITF_INLINE const PhantomDetectorComponent_Template* PhantomDetectorComponent::getTemplate() const
    {
        return static_cast<const PhantomDetectorComponent_Template*>(m_template);
    }

};

#endif //_ITF_PHANTOMDETECTORCOMPONENT_H_