#ifndef _ITF_SPAWNER_H_
#define _ITF_SPAWNER_H_

#ifndef _ITF_STRINGID_H_
#include "core/StringID.h"
#endif //_ITF_STRINGID_H_

#ifndef _ITF_PATH_H_
#include "core/file/Path.h"
#endif //_ITF_PATH_H_

#ifndef _ITF_KEYARRAY_H_
#include "core/container/KeyArray.h"
#endif //_ITF_KEYARRAY_H_

namespace ITF
{

    #define SPAWNER Spawner::getInstance()

    class Scene;

    ///////////////////////////////////////////////////////////////////////////////////////////
    class SpawneeGenerator
    {
        friend class Spawner;
    public:
        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Creation (must be initialized by Spawner)
        SpawneeGenerator() : m_luaID(StringID::InvalidId) {}

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Copy constructor. Increases ref count.
        SpawneeGenerator(const SpawneeGenerator &_other);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// Destruction. Decreases reference count.
        ~SpawneeGenerator();

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// operator=
        SpawneeGenerator& operator=(const SpawneeGenerator &_other);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// create a spawnee at given scene, pos, angle
        /// @param _scene : The destination scene
        /// @param _pos : The creation position
        /// @param _angle : the angle
        Actor* getSpawnee(Scene *_scene, const Vec3d &_pos, f32 _angle=0);

        ///////////////////////////////////////////////////////////////////////////////////////////
        /// returns btrue if this object was correctly initialized by Spawner class
        bbool isInitialized() const {return m_luaID!=StringID::InvalidId;}

    private:
        void initialize(const StringID &_luaID) {m_luaID = _luaID.GetValue();} //init by spawner (friend)
        void invalidate(const StringID &_luaID) {m_luaID = StringID::InvalidId;} //invalidation by spawner (friend)


        u32 m_luaID;
    };


    ///////////////////////////////////////////////////////////////////////////////////////////
    ///The spawner (singleton)
    class Spawner
    {
        friend class SpawneeGenerator;
    private:
        static Spawner*    s_instance;

        Spawner();

        static void create();

    public:

        ~Spawner();

        ///////////////////////////////////////////////////////////////////////////////////////////
        static Spawner *getInstance() {if (!s_instance) {create();} return s_instance;}

        ///////////////////////////////////////////////////////////////////////////////////////////
        ///Terminates the singleton
        static void terminate();

        ///create a reference object and links its resources to the parent's resource group
        ///The spawnee generator is initialized only once. 2nd time, nothing happens.
        /// @param[in,out] _ownerResourceGroup : The resource group of the parent. Resources of the spawnees will be linked to this resource group. 
        /// @param[out] _generator : the generator to initialize
        /// @param[in] _lua : path to the lua (.act) file
        bbool declareNeedsSpawnee(Actor *_owner, SpawneeGenerator *_generator, const Path &_lua);

        ///create a reference object and links its resources to the given resource group
        ///The spawnee generator is initialized only once. 2nd time, nothing happens.
        /// @param[in,out] _ownerResourceGroup : The resource group of the parent. Resources of the spawnees will be linked to this resource group. 
        /// @param[out] _generator : the generator to initialize
        /// @param[in] _lua : path to the lua (.act) file
        bbool declareNeedsSpawnee(ResourceGroup *_ownerResourceGroup, SpawneeGenerator *_generator, const Path &_lua);

        ///unregister the reference object
        void declareNoLongerNeedsSpawnee(SpawneeGenerator *_generator);

        ///unregister the reference object
        ActorRef getReferenceItem( const SpawneeGenerator& _generator );

        //////////////////////////////////////////////////////////////////////////
        ///Get object  spawner world
        World* getSpawnerWorld() const {return IRTTIObject::SafeDynamicCast<World>(m_spawnerWorld.getObject(),ITF_GET_STRINGID_CRC(World,3307309411));}

    private:
        ///////////////////////////////////////////////////////////////////////////////////////////
        class SpawnerReferenceItem
        {
        public:
            SpawnerReferenceItem(Actor *_theActor);

            ~SpawnerReferenceItem() {ITF_ASSERT(m_clientCount==0);}

            bbool unrefReferenceItem(ObjectRef _spawnerScene); //returns btrue if ref count reached 0 and the object was destroyed

            ActorRef m_referenceObject;
            i32 m_clientCount;
            ArchiveMemory m_archiveMem;
#ifndef ITF_FINAL
            String8 m_actorName;
#endif
        };
        ///////////////////////////////////////////////////////////////////////////////////////////
        void getIDFromLua(const Path & _lua, StringID &_result);
        SpawnerReferenceItem *findReferenceItem(u32 _id, i32 *_indexInReserve=NULL);
        void destroyReferenceItems();
        void destroy();
        SpawnerReferenceItem *createReferenceItem(const Path &_lua); //Create a SpawerReferenceItem, but do not add to m_reserve
        Actor* createSpawnee(Scene *_scene, const Vec3d &_position, f32 _angle, u32 _luaID);
        void initialize();
        void increaseRefCount(u32 _luaID);


        ///////////////////////////////////////////////////////////////////////////////////////////
        KeyArray<SpawnerReferenceItem*> m_reserve;
        ObjectRef m_spawnerWorld;
        ObjectRef m_spawnerScene;

    };
}
#endif
