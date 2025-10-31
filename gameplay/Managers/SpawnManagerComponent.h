#ifndef _ITF_SPAWNMANAGERCOMPONENT_H_
#define _ITF_SPAWNMANAGERCOMPONENT_H_


namespace ITF
{

    class SpawnManagerComponent : public ActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SpawnManagerComponent, ActorComponent,502828938);

    public:
        DECLARE_SERIALIZE();
        DECLARE_VALIDATE_COMPONENT();

        SpawnManagerComponent();
        virtual ~SpawnManagerComponent();

        virtual bbool       needsUpdate() const { return bfalse; }
        virtual bbool       needsDraw() const { return bfalse; }
        virtual bbool       needsDraw2D() const { return bfalse; }

        virtual void        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);

    private:
        void                clear();
        void                unspawnAll();
    };

    //-------------------------------------------------------------------------------------
    class SpawnManagerComponent_Template : public TemplateActorComponent
    {
        DECLARE_OBJECT_CHILD_RTTI(SpawnManagerComponent_Template,TemplateActorComponent,1136686137);
        DECLARE_SERIALIZE();
        DECLARE_ACTORCOMPONENT_TEMPLATE(SpawnManagerComponent);

    public:

        SpawnManagerComponent_Template();
        ~SpawnManagerComponent_Template() {}

    private:

    };
};

#endif //_ITF_SPAWNMANAGERCOMPONENT_H_