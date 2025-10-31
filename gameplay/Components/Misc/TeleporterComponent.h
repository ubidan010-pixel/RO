
#ifndef _ITF_TELEPORTERCOMPONENT_H_
#define _ITF_TELEPORTERCOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_



namespace ITF
{
    class Lua;
    class Spline;

    class TeleporterComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TeleporterComponent,ActorComponent,1305761296);

    public:
        DECLARE_SERIALIZE()

        TeleporterComponent();
        ~TeleporterComponent();
        virtual bbool needsUpdate() const { return btrue; }
        virtual bbool needsDraw() const { return bfalse; }
        virtual bbool needsDraw2D() const { return bfalse; }
        virtual void onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        ITF_INLINE u32 getIndex() const;
        void        Update( f32 _deltaTime) ;

        virtual void onEvent(Event * _event);

    protected:

        ITF_INLINE const class TeleporterComponent_Template*  getTemplate() const;

    private:

        typedef struct TeleportActor
        {
            ObjectRef   m_or;
            Spline      *m_spline;
            f32         m_fTime;
            f32         mzCamDiff;
        } TeleportActor;

        TeleportActor m_teleportActor;
        void clear();

    };

    //---------------------------------------------------------------------------------------------------

    class TeleporterComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(TeleporterComponent_Template,TemplateActorComponent,1759675961);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(TeleporterComponent);

    public:

        TeleporterComponent_Template() : m_index(U32_INVALID) {}
        ~TeleporterComponent_Template() {}

        u32             getIndex() const { return m_index; }

    private:

        u32 m_index;
    };


    const TeleporterComponent_Template*  TeleporterComponent::getTemplate() const {return static_cast<const TeleporterComponent_Template*>(m_template);}

    ITF_INLINE u32 TeleporterComponent::getIndex() const { return  getTemplate()->getIndex(); }
}

#endif // _ITF_TELEPORTERCOMPONENT_H_

