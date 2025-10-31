#ifndef _ITF_PREFETCH_COMPONENT_H_
#define _ITF_PREFETCH_COMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif _ITF_OBJECTPATH_H_

namespace ITF
{
    class Event;

    class PrefetchComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PrefetchComponent, ActorComponent,3811247622);

    public:
        DECLARE_SERIALIZE()

        PrefetchComponent();
        virtual ~PrefetchComponent();

        void                validate(bbool &_isComponentValidated);

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const   { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
        virtual void        onEvent( Event * _event);

        virtual void        Update( f32 _deltaTime );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Tell if the requested prefetch is over
        /// @return (ITF::bbool) btrue if the requested prefetch is over
        bbool               prefetchComplete() const;

        bbool               getPrefetchTargetWorld(ObjectRef& _result)const;

        bbool               getPrefetchTargetPos(Vec3d& _result)const;

    private:

        struct WorldPrefetch
        {
            WorldPrefetch() : m_ref(ITF_INVALID_OBJREF), m_prefetchComplete(bfalse), m_prefetchCalled(bfalse), m_requestPrefetchStop(bfalse) {}

            String8             m_worldName;
            ObjectRef           m_ref;
            Vector <ObjectPath> m_objects;
            bbool               m_prefetchComplete;
            bbool               m_prefetchCalled;
            bbool               m_requestPrefetchStop;
        };
        typedef Vector <WorldPrefetch> PrefetchList;

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Begin prefetch
        void                start();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Stop prefetch
        void                stop();

        PrefetchList        m_prefetchs;
    };

    //---------------------------------------------------------------------------------------------------

    class PrefetchComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(PrefetchComponent_Template,TemplateActorComponent,3860101993);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(PrefetchComponent);

    public:

        PrefetchComponent_Template() {}
        ~PrefetchComponent_Template() {}

    private:

    };
}

#endif // _ITF_PREFETCH_COMPONENT_H_