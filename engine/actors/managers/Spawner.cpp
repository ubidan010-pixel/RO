#include "precompiled_engine.h"

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif

#ifndef _ITF_WORLD_MANAGER_H_
#include "engine/scene/worldManager.h"
#endif //_ITF_WORLD_MANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_FILEPATH_H_
#include "core/file/filepath.h"
#endif //_ITF_FILEPATH_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef SERIALIZEROBJECTBINARY_H
#include "core/serializer/ZSerializerObjectBinary.h"
#endif // SERIALIZEROBJECTBINARY_H


namespace ITF
{
    Spawner* Spawner::s_instance=NULL;

    ///////////////////////////////////////////////////////////////////////////////////////////
    Spawner::Spawner()
    {
        m_reserve.reserve(64); //memory reservation. 64 is arbitrary.
        initialize();
    }

    Spawner::~Spawner()
    {
        destroy();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Spawner::create()
    {
        ITF_ASSERT(!s_instance);
        s_instance = newAlloc(mId_Gameplay, Spawner());
    }

    void Spawner::terminate()
    {
        SF_DEL(s_instance);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Spawner::initialize()
    {
        ITF_ASSERT(!m_spawnerWorld.isValid());

        m_spawnerWorld = WORLD_MANAGER->createAndLoadEmptyWorld(btrue);
        World *world = (World*)m_spawnerWorld.getObject();
        ITF_ASSERT(world);
        if (world)
        {
            m_spawnerScene = world->getRootScene()->getRef();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Spawner::destroyReferenceItems()
    {
        ITF_ASSERT_MSG(m_reserve.size()==0, "Spawner reserve is not empty!");
        while (m_reserve.size())
        {
            u32 indexToDel = 0;
            u32 key = m_reserve.getKeyAt(indexToDel);
            SpawnerReferenceItem *item = m_reserve.getAt(indexToDel);
            ITF_ASSERT(item);
            while (item->m_clientCount>0)
            {
                item->unrefReferenceItem(m_spawnerScene);
            }
            SF_DEL(item);    
            m_reserve.erase(key);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Spawner::destroy()
    {
        World *world = (World*)m_spawnerWorld.getObject();
        ITF_ASSERT(world);
        if (world)
        {
            world->flushPending();
        }
        destroyReferenceItems();
        if (world)
        {
            WORLD_MANAGER->deleteWorld(world);
        }
        m_spawnerWorld.invalidate();
        m_spawnerScene.invalidate();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Spawner::getIDFromLua(const Path & _lua, StringID &_result)
    {
        _result = _lua.getStringID();
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Spawner::SpawnerReferenceItem *Spawner::createReferenceItem(const Path &_lua)
    {
        Scene *scene = (Scene*)m_spawnerScene.getObject();
        ITF_ASSERT(scene);
        Actor *theActor = ACTORSMANAGER->spawnActor(Vec3d(0,0,0), 
            scene, 
            _lua,
            btrue,
            NULL);
        if (!theActor)
            return NULL;

        SpawnerReferenceItem *refItem = newAlloc(mId_Gameplay, SpawnerReferenceItem(theActor));
        return refItem;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Spawner::SpawnerReferenceItem *Spawner::findReferenceItem(u32 _id, i32 *_indexInReserve)
    {
        i32 index = m_reserve.find(_id);

        if (_indexInReserve!=NULL)
        {
            *_indexInReserve = index;
        }

        if (index<0)
        {
            return NULL;
        }

        return m_reserve.getAt((u32)index);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Spawner::SpawnerReferenceItem::unrefReferenceItem(ObjectRef _spawnerScene)
    {
        ITF_ASSERT(m_clientCount>0);
        m_clientCount--;
        ITF_ASSERT(m_referenceObject.getActor()!=NULL);
        bbool mustDestroy = m_clientCount<=0;
        if (mustDestroy)
        {
            Scene *theScene = (Scene*)_spawnerScene.getObject();
            Actor *theActor = m_referenceObject.getActor();
            ITF_ASSERT(theScene!=NULL && theActor!=NULL);
            if (theScene!=NULL && theActor!=NULL)
            {
                theScene->unregisterPickable(theActor);
            }
            m_referenceObject.invalidate();
        }

        return mustDestroy;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Actor* SpawneeGenerator::getSpawnee(Scene *_scene, const Vec3d &_pos, f32 _angle)
    {
        return SPAWNER->createSpawnee(_scene, _pos, _angle, m_luaID);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    SpawneeGenerator::~SpawneeGenerator()
    {
        SPAWNER->declareNoLongerNeedsSpawnee(this);
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    SpawneeGenerator::SpawneeGenerator(const SpawneeGenerator &_other)
    {
        m_luaID = _other.m_luaID;
        if (m_luaID!=StringID::InvalidId)
        {
            SPAWNER->increaseRefCount(m_luaID);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    SpawneeGenerator& SpawneeGenerator::operator=(const SpawneeGenerator &_other)
    {
        if (m_luaID != _other.m_luaID)
        {
            if (_other.isInitialized())
            {
                SPAWNER->increaseRefCount(_other.m_luaID);
            }
            SPAWNER->declareNoLongerNeedsSpawnee(this);
            m_luaID = _other.m_luaID;
        }

        return *this;
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    void Spawner::declareNoLongerNeedsSpawnee(SpawneeGenerator *_generator)
    {
        if (_generator->isInitialized())
        {
            u32 id = _generator->m_luaID;

            //Search for LUA in reference item list. If not found, do nothing
            i32 itemIndexInReserve=-1;
            SpawnerReferenceItem *referenceItem = findReferenceItem(id, &itemIndexInReserve);
            if (referenceItem)
            {            
                if (referenceItem->unrefReferenceItem(m_spawnerScene))
                {
                    ITF_ASSERT(itemIndexInReserve>=0);
                    SF_DEL(referenceItem);
                    m_reserve.erase((u32)id);
                }
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Actor* Spawner::createSpawnee(Scene *_scene, const Vec3d &_pos, f32 _angle, u32 _luaID)
    {
        SpawnerReferenceItem *referenceItem = findReferenceItem(_luaID);
        ITF_WARNING( NULL, referenceItem, "The spawnee generator was unregistered but is still used" );
        if (!referenceItem)
            return NULL;

        ITF_ASSERT(referenceItem->m_clientCount>0);
        ITF_ASSERT(referenceItem->m_referenceObject.getActor()!=nullptr);
        referenceItem->m_archiveMem.rewindForReading();
        Actor *newActor = Actor::createFromDataArchive(referenceItem->m_archiveMem, _scene, bfalse, bfalse);
        if (!newActor)
        {
            ITF_ASSERT_MSG(newActor, "could not clone actor for spawning");
            return NULL;
        }
        newActor->setIsSpawned(btrue);
        newActor->setWorldInitialPos(_pos);
        newActor->forceMove(_pos);
        newActor->setWorldInitialRot(_angle);
        newActor->setAngle(_angle);
        newActor->setSerializable(bfalse);
        newActor->resetCurrentInfos();
        i32 index = 0;
        newActor->getUserFriendly().strstr("@",bfalse,&index);
        if (index != -1)
        {
            newActor->setUserFriendly(newActor->getUserFriendly().substr(0,index));
        }
        newActor->generateNewUserFriendly();
		newActor->onLoaded();

        return newActor;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    Spawner::SpawnerReferenceItem::SpawnerReferenceItem(Actor *_theActor) : m_clientCount(0)
    {
        m_referenceObject = _theActor->getRef();
#ifndef ITF_FINAL
        if(const Path* path = _theActor->getTemplatePath())
            m_actorName = path->getString().cStr();
#endif
        CSerializerObjectBinary serializeBin(&m_archiveMem);
        _theActor->Serialize(&serializeBin, ESerialize_Data_Save);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Spawner::declareNeedsSpawnee(Actor *_owner, SpawneeGenerator *_generator, const Path &_lua)
    {
        ITF_ASSERT_CRASH(_owner!=NULL, "invalid owner actor");

        return declareNeedsSpawnee(_owner->getResourceGroup(), _generator, _lua);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Spawner::increaseRefCount(u32 _luaID)
    {
        SpawnerReferenceItem *referenceItem = findReferenceItem(_luaID);
        ITF_ASSERT_MSG(referenceItem!=NULL, "unknown model");
        if (referenceItem)
        {
            referenceItem->m_clientCount++;
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    bbool Spawner::declareNeedsSpawnee(ResourceGroup *_ownerResourceGroup, SpawneeGenerator *_generator, const Path &_lua)
    {
        ITF_ASSERT_MSG(m_spawnerWorld.isValid(), "Spawner is either not initialized, or its world was destroyed");

        if (_generator->isInitialized())
        {
#ifdef _DEBUG
            StringID id;
            getIDFromLua(_lua, id);
            if (id!=_generator->m_luaID)
            {
                ITF_ASSERT_MSG(bfalse, "inconsistent SpawneeGenerator declaration");
                return bfalse;
            }
#endif
            return btrue;
        }

        StringID id;
        getIDFromLua(_lua, id);

        //Search for lua in reference item list. If not found, create it. return false if error.
        SpawnerReferenceItem *referenceItem = findReferenceItem(id);
        if (!referenceItem)
        {
            referenceItem = createReferenceItem(_lua);
            if (!referenceItem)
            {
                ITF_WARNING(NULL, referenceItem, "Could not create spawner model for %s", _lua.getString8().cStr());
                return bfalse;
            }
            m_reserve.setID(id, referenceItem);
        }

        //declare as client
        referenceItem->m_clientCount++;
        ActorRef refObject = referenceItem->m_referenceObject;
        Actor *refActor = refObject.getActor();

        ITF_ASSERT(refActor!=NULL);
        if (!refActor)
        {
            ITF_WARNING_CATEGORY(Engine,NULL, refActor!=NULL, "Spawner : reference model for %s was destroyed", _lua.getString8().cStr());
            return bfalse;
        }

        //Add resource to parent object
        ResourceGroup *spawneeResource = refActor->getResourceGroup();
        if (spawneeResource)
        {
            ResourceGroup *clientRsc = _ownerResourceGroup;
            ITF_ASSERT_CRASH(clientRsc, "client object has no resource group!");
            clientRsc->addResource(spawneeResource, btrue, bfalse);
        }
        //initialize output
        _generator->initialize(id);
        return btrue;
    }

    ActorRef Spawner::getReferenceItem( const SpawneeGenerator& _generator )
    {
        if ( SpawnerReferenceItem* item = findReferenceItem(_generator.m_luaID) )
        {
            return item->m_referenceObject;
        }

        return ActorRef();
    }

}
