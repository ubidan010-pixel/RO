#include "precompiled_gameplay.h"

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_BASEOBJECT_H_
#include "core/baseObject.h"
#endif //_ITF_BASEOBJECT_H_

#ifndef _ITF_ID_SERVER_H_
#include "core/IDServer.h"
#endif //_ITF_ID_SERVER_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSSHAPES_H_
#include "engine/physics/PhysShapes.h"
#endif //_ITF_PHYSSHAPES_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_POLYLINE_H_
#include "engine/physics/PolyLine.h"
#endif //_ITF_POLYLINE_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

#ifndef _ITF_DEBUGINFO_H_
#include "engine/debug/debugInfo.h"
#endif //_ITF_DEBUGINFO_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

namespace ITF
{

    DECLARE_RASTER(stimsManager_update, RasterGroup_Misc, Color::grey());

StimsManager::StimsManager()
#ifdef ITF_SUPPORT_DEBUGFEATURE
: m_drawMode(0)
#endif //ITF_SUPPORT_DEBUGFEATURE
{
    createPools();
}

StimsManager::~StimsManager()
{
    for ( StimsRecyclingMap::iterator it = m_recyclingStims.begin(); it != m_recyclingStims.end(); ++it )
    {
        StimsQueue& queue = it->second;

        while ( !queue.empty() )
        {
            EventStim* stim = queue.front();
            queue.pop();
            delete stim;
        }
    }

    m_recyclingStims.clear();

    destroyPools();
}

void StimsManager::update( f32 _deltaTime )
{
    TIMERASTER_SCOPE(stimsManager_update);

    StimsArray stims = m_pendingStims;
    m_pendingStims.clear();
    

    for ( StimsArray::iterator it = stims.begin(); it != stims.end(); ++it )
    {
        EventStim* stim = *it;

        SafeArray<ObjectRef> processed;
        processStim(stim, processed);
        recycleStim(stim);
    }


    restorePools();
}

void StimsManager::sendStim( EventStim* _stim )
{
#ifndef ITF_FINAL
    if ( !_stim->getShape() || !_stim->getShape()->isValid() )
    {
        BaseObject* obj = GETOBJECT(_stim->getSender());
        ITF_WARNING(obj, 0, "Trying to send a stim with an invalid shape! Call a GPP!");
        return;
    }
#endif // ITF_FINAL

    m_pendingStims.push_back(_stim);
}

void StimsManager::processStim( EventStim* _stim, SafeArray<ObjectRef>& _processed )
{
    BaseObject* obj = GETOBJECT(_stim->getSender());

    if ( !obj )
    {
        // If the sender is not in memory we can't be sure the shape is in memory as well, so we don't
        // process this stim
        return;
    }

#ifndef ITF_FINAL
    if ( !_stim->getShape() || !_stim->getShape()->isValid() )
    {
        ITF_WARNING(obj, 0, "Trying to process a stim with an invalid shape! Call a GPP!");
        return;
    }
#endif // ITF_FINAL


    PhysContactsContainer allContacts;
    PhysContactsContainer contacts;

    PHYSWORLD->collidePhantoms(_stim->getPrevPos(),_stim->getPos(),_stim->getAngle(),_stim->getShape(),
                               _stim->getDepth(),ECOLLISIONFILTER_ALL,allContacts);

    u32 numContacts = allContacts.size();

    // discard non-hittable polylines
    if ( numContacts )
    {
        PolyLine* poly(NULL);
        const PolyLineEdge* edge(NULL);

        for ( u32 i = 0; i < numContacts; i++ )
        {
            const SCollidableContact& contact = allContacts[i];

            AIUtils::getPolyLine(contact.m_collidableUserData, contact.m_edgeIndex, poly, edge);

            if ( poly && edge && poly->m_isCollisionActive )
            {
                const GameMaterial_Template* mat = World::getGameMaterial(edge->getGameMaterial());

                if ( mat && mat->getNoStim() )
                {
                    continue;
                }
            }

            contacts.push_back(contact);
        }
    }

    for ( u32 i = 0; i < contacts.size(); i++ )
    {
        SCollidableContact& contact = contacts[i];

        if ( contact.m_collidableUserData == _stim->getSender() )
        {
            continue;
        }

        BaseObject* obj = GETOBJECT(contact.m_collidableUserData);

        if ( obj )
        {
            Actor* receiver;
            
            if ( ( receiver = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) ) == NULL )
            {
                PolyLine* poly = obj->DynamicCast<PolyLine>(ITF_GET_STRINGID_CRC(PolyLine,1932163747));

                if ( poly )
                {
                    receiver = poly->getOwnerActor();
                }
            }

            ITF_ASSERT(receiver);

            if ( !receiver )
            {
                continue;
            }


            if ( _processed.find(contact.m_collidable) != -1 )
            {
                continue;
            }

            _processed.push_back(contact.m_collidable);


            // Send only one stim with all the contacts to the receiver
            _stim->resetContacts();
            _stim->addContact(contact);
            _stim->setPhantomRef(contact.m_collidable);

            for ( u32 j = i + 1; j < contacts.size(); j++ )
            {
                SCollidableContact& newContact = contacts[j];
                BaseObject* obj = GETOBJECT(newContact.m_collidableUserData);

                if ( obj )
                {
                    Actor* newReceiver = obj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));

                    if ( newReceiver == receiver )
                    {
                        _stim->addContact(newContact);
                    }
                }
            }

            receiver->onEvent(_stim);
            
            BaseObject* senderObj = GETOBJECT(_stim->getSender());
            ITF_ASSERT(senderObj);
            Actor* sender = senderObj->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115));
            ITF_ASSERT(sender);
            EventStimNotify eventForSender;
            eventForSender.setSender(receiver->getRef());
            eventForSender.setContact(contact);
            sender->onEvent(&eventForSender);

            if (_stim->getStopProcessing())
            {
                break;
            }
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    if ( m_drawMode )
    {
        f32 duration = (m_drawMode == 1) ? 1.f : 0.f; // 1 second or 1 frame

        DebugDraw::shape(_stim->getPos(),0.f,_stim->getAngle(),_stim->getShape(),Color::white(), duration);

        for ( u32 i = 0; i < contacts.size(); i++ )
        {
            SCollidableContact& contact = contacts[i];

            GFX_ADAPTER->drawDBGCircle(contact.m_contactPoint, 0.1f, 0.f, 0.f, 0.3f, duration);
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE
}

void StimsManager::recycleStim( EventStim* _stim )
{
    StringID::StringIdValueType stimCrc = _stim->GetObjectClassCRC();
    StimsRecyclingMap::iterator it = m_recyclingStims.find(stimCrc);

    // reset the shape to be safe
    _stim->setShape(NULL);

    if ( it != m_recyclingStims.end() )
    {
        it->second.push(_stim);
    }
    else
    {
        StimsQueue newQueue;

        newQueue.push(_stim);

        m_recyclingStims[stimCrc] = newQueue;
    }
}

bbool StimsManager::prepareStim( EventStim* _stim, const PhysShape* _shape )
{
    _stim->reset();

    if ( _shape )
    {
        ShapesList* shapeList = getShapePoolForShape(_shape);

        if ( shapeList && !shapeList->empty() )
        {
            PhysShape* shapeCopy = shapeList->back();
            PhysShape::CopyShape(_shape,shapeCopy);
            _stim->setShape(shapeCopy);
            shapeList->pop_back();
            m_usedShapes.push_back(shapeCopy);
        }
        else
        {
            return bfalse;
        }
    }

    return btrue;
}

void StimsManager::createPools()
{
    for ( u32 i = 0; i < s_PoolSize; i++ )
    {
        m_circlePool.push_back(newAlloc(mId_Phys,PhysShapeCircle()));
        m_boxPool.push_back(newAlloc(mId_Phys, PhysShapeBox()));
        m_polygonPool.push_back(newAlloc(mId_Phys, PhysShapePolygon()));
    }
}

void StimsManager::destroyPools()
{
    restorePools();

    destroyPool(&m_circlePool);
    destroyPool(&m_boxPool);
    destroyPool(&m_polygonPool);
}

void StimsManager::destroyPool( ShapesList* _list )
{
    for ( ShapesList::iterator it = _list->begin(); it != _list->end(); ++it )
    {
        delete *it;
    }

    _list->clear();
}

void StimsManager::restorePools()
{
    for ( ShapesArray::iterator it = m_usedShapes.begin(); it != m_usedShapes.end(); ++it )
    {
        PhysShape* shape = *it;
        ShapesList* shapeList = getShapePoolForShape(shape);

        ITF_ASSERT(shapeList);

        if ( shapeList )
        {
            shapeList->push_back(shape);
        }
    }

    m_usedShapes.clear();
}

StimsManager::ShapesList* StimsManager::getShapePoolForShape( const PhysShape* _shape )
{
    StringID::StringIdValueType className = _shape->GetObjectClassCRC();

    if ( className == ITF_GET_STRINGID_CRC(PhysShapeCircle,3922519168) )
    {
        return &m_circlePool;
    }
    else if ( className == ITF_GET_STRINGID_CRC(PhysShapeBox,3520962473) )
    {
        return &m_boxPool;
    }
    else if ( className == ITF_GET_STRINGID_CRC(PhysShapePolygon,3261252112) )
    {
        return &m_polygonPool;
    }
    else
    {
        ITF_ASSERT(0);
        return NULL;
    }
}

#ifndef ITF_FINAL
void StimsManager::dumpStimsDebugInfoAndCrash()
{
    String dumpData = "-----------------------------\r\nStimsManager Debug Info\r\n-----------------------------\r\n";
    String tmp;
    u32 numPending = m_pendingStims.size();

    for ( u32 i = 0; i < numPending; i++ )
    {
        EventStim* stim = m_pendingStims[i];
        
        tmp.setTextFormat("Stim %u (%s)\r\n",i,StringID(stim->GetObjectClassCRC()).getDebugString());
        dumpData += tmp;

        Actor* sender = AIUtils::getActor(stim->getSender());

        if ( sender )
        {
            StringConverter lua(sender->getLua().getString().cStr());
            tmp.setTextFormat("Sender: %s\r\n",lua.getChar());
            dumpData += tmp;
        }

        const PhysShape* shape = stim->getShape();

        if ( shape )
        {
            tmp.setTextFormat("Shape: %s\r\n",StringID(shape->GetObjectClassCRC()).getDebugString());
            dumpData += tmp;
        }

        dumpData += "-----------------------------\r\n";
    }

    if ( m_circlePool.empty() )
    {
        dumpData += "Circle Pool is empty\r\n";
    }
    else
    {
        tmp.setTextFormat("Circle Pool elements: %d\r\n",m_circlePool.size());
        dumpData += tmp;
    }

    if ( m_polygonPool.empty() )
    {
        dumpData += "Polygon Pool is empty\r\n";
    }
    else
    {
        tmp.setTextFormat("Polygon Pool elements: %d\r\n",m_polygonPool.size());
        dumpData += tmp;
    }

    if ( m_boxPool.empty() )
    {
        dumpData += "Box Pool is empty\r\n";
    }
    else
    {
        tmp.setTextFormat("Box Pool elements: %d\r\n",m_boxPool.size());
        dumpData += tmp;
    }
#ifdef ITF_SUPPORT_DEBUGFEATURE
    DEBUGINFO->addGenericInfo(dumpData);
#endif //ITF_SUPPORT_DEBUGFEATURE
    ITF_ASSERT_MSG(0, "Your using too many stims, maybe you should increase s_PoolSize ?");
}
#endif //!ITF_FINAL

#ifdef ITF_SUPPORT_DEBUGFEATURE
void StimsManager::toggleDrawStims()
{
    m_drawMode++;
    if (m_drawMode > 2)
        m_drawMode = 0;
}
#endif // ITF_SUPPORT_DEBUGFEATURE

}
