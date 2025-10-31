#ifndef _ITF_WAR_PICKUPCOMPONENT_H_
#define _ITF_WAR_PICKUPCOMPONENT_H_

namespace ITF
{
    class War_PickupComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(War_PickupComponent, ActorComponent,1727468072)

    public:
        DECLARE_SERIALIZE();

        War_PickupComponent();
        virtual ~War_PickupComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded();
        virtual void        onEvent( Event* _event);

    private:

        bbool       m_pickedUp;
        void        onPickup();
    };
};

#endif //_ITF_WAR_PICKUPCOMPONENT_H_