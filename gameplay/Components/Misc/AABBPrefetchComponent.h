#ifndef _ITF_AABBPREFETCH_COMPONENT_H_
#define _ITF_AABBPREFETCH_COMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_OBJECTPATH_H_
#include "core/ObjectPath.h"
#endif _ITF_OBJECTPATH_H_

namespace ITF
{
    class View;
    class Event;

    class AABBPrefetchComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AABBPrefetchComponent, ActorComponent,3064131908);
        DECLARE_SERIALIZE()
        DECLARE_VALIDATE_COMPONENT()

    public:
        AABBPrefetchComponent();
        virtual ~AABBPrefetchComponent();

        virtual bbool       needsUpdate() const { return btrue; }
        virtual bbool       needsDraw() const   { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType _hotReload);
        virtual void        onActorRemovedFromScene(Scene* _pScene, bbool _forDeletion);

        virtual void        onSceneActive();
        virtual void        onSceneInactive();

        virtual void        onBecomeActive();
        virtual void        onBecomeInactive();

        virtual void        Update( f32 _deltaTime );

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Tell if the requested prefetch is over
        /// @return (ITF::bbool) btrue if the requested prefetch is over
        bbool               prefetchComplete() const;
        
        // This function will force the prefetch, but the object calling it is responsible for turning it off when it's finished
        void                forceStart( bbool _val );

        void                forceUpdateByChild(bbool _b);
        bbool               isUpdateForcedByChild()const        { return m_forceUpdateByChild; }
        u32                 getForceUpdateFrame()const          { return m_forceUpdateByChild_Frame; }

    private:

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Begin prefetch
        void                start();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Stop prefetch
        void                stop();

        bbool               isPrefetching()const { return m_view != NULL; }
        bbool               m_ready;
        bbool               m_forceStart;
        bbool               m_forceUpdateByChild;
        u32                 m_forceUpdateByChild_Frame;
        View*               m_view;
        f32                 m_failSafeCounter;
    };

    //---------------------------------------------------------------------------------------------------

    class AABBPrefetchComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(AABBPrefetchComponent_Template,TemplateActorComponent,1890823098);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(AABBPrefetchComponent);

    public:

        AABBPrefetchComponent_Template() {}
        ~AABBPrefetchComponent_Template() {}

    private:

    };
}

#endif // _ITF_PREFETCH_COMPONENT_H_