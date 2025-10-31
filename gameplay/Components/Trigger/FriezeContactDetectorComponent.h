#ifndef _ITF_FRIEZECONTACTDETECTORCOMPONENT_H_
#define _ITF_FRIEZECONTACTDETECTORCOMPONENT_H_

#ifndef _ITF_DETECTORCOMPONENT_H_
#include "gameplay/Components/Trigger/DetectorComponent.h"
#endif //_ITF_DETECTORCOMPONENT_H_

namespace ITF
{
    class FriezeContactDetectorComponent : public DetectorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(FriezeContactDetectorComponent, DetectorComponent,715604426);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:

        FriezeContactDetectorComponent();
        virtual ~FriezeContactDetectorComponent();

        void clear();

        virtual void        onActorClearComponents();

        virtual void        onActorLoaded( Pickable::HotReloadType _hotReload );
        virtual void        onEvent( Event* _event );

        void checkRemove( ActorRef actorRef );

        void checkAdd( ActorRef &actorRef );


    protected:
        ITF_INLINE const class FriezeContactDetectorComponent_Template*  getTemplate() const;
        class LinkComponent * m_linkComponent;
        SafeArray<ActorRef> m_currentActors;
    };

    //-------------------------------------------------------------------------------------
    class FriezeContactDetectorComponent_Template : public DetectorComponent_Template
    {
        DECLARE_OBJECT_CHILD_RTTI(FriezeContactDetectorComponent_Template,DetectorComponent_Template,4035314567);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(FriezeContactDetectorComponent);

    public:

        FriezeContactDetectorComponent_Template();
        ~FriezeContactDetectorComponent_Template() {}


        ITF_INLINE Faction getFactionToDetect() const { return m_factionToDetect; }
        ITF_INLINE bbool getAllowDeadActors() const { return m_allowDeadActors; }
        ITF_INLINE bbool getDetectHang() const { return m_detectHang; }
    private:

        Faction     m_factionToDetect;
        bbool       m_allowDeadActors;
        bbool       m_detectHang;
    };


    const FriezeContactDetectorComponent_Template*  FriezeContactDetectorComponent::getTemplate() const {return static_cast<const FriezeContactDetectorComponent_Template*>(m_template);}

};

#endif //_ITF_FRIEZECONTACTDETECTORCOMPONENT_H_