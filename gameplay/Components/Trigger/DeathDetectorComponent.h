#ifndef _ITF_DEATHDETECTORCOMPONENT_H_
#define _ITF_DEATHDETECTORCOMPONENT_H_


#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

namespace ITF
{
    class DeathDetectorComponent : public DetectorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(DeathDetectorComponent, DetectorComponent,4247132828);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:



        DeathDetectorComponent();
        virtual ~DeathDetectorComponent();

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        Update( f32 _dt );


    private:
        class LinkComponent * m_linkComponent;

        ITF_INLINE const class DeathDetectorComponent_Template*  getTemplate() const;
    };

    //-------------------------------------------------------------------------------------

    class DeathDetectorComponent_Template : public DetectorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(DeathDetectorComponent_Template,DetectorComponent_Template,1075865440);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(DeathDetectorComponent);

    public:

        DeathDetectorComponent_Template();
        ~DeathDetectorComponent_Template() {}
        ITF_INLINE bbool getIsAnd() const { return m_isAnd; }

    private:
        bbool m_isAnd;
    };


    const DeathDetectorComponent_Template*  DeathDetectorComponent::getTemplate() const {return static_cast<const DeathDetectorComponent_Template*>(m_template);}
};

#endif //_ITF_DEATHDETECTORCOMPONENT_H_