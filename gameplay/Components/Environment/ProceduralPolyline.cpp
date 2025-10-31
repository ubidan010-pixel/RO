#include "precompiled_gameplay.h"

#ifndef _ITF_PROCEDURALPOLYLINE_H_
#include "gameplay/Components/Environment/ProceduralPolyline.h"
#endif //_ITF_PROCEDURALPOLYLINE_H_

#ifndef _ITF_PHYSWORLD_H_
#include "engine/physics/PhysWorld.h"
#endif //_ITF_PHYSWORLD_H_

#ifndef _ITF_PHYSBODY_H_
#include "engine/physics/PhysBody.h"
#endif //_ITF_PHYSBODY_H_

#ifndef _ITF_PHYSPHANTOM
#include "engine/physics/PhysPhantom.h"
#endif //_ITF_PHYSPHANTOM

#ifndef _ITF_GAMEMATERIAL_H_
#include "engine/gameplay/GameMaterial.h"
#endif //_ITF_GAMEMATERIAL_H_

namespace ITF
{
    ProceduralPolyline::ProceduralPolyline()
        : m_polyline( NULL )
        , m_phantom( NULL )
        , m_material(GameMaterial_Template::nullMatName)
        , m_active(bfalse)
        , m_phantomRegistered( bfalse )
    {
    }

    ProceduralPolyline::~ProceduralPolyline()
    {
        clear();
    }


    void ProceduralPolyline::clear()
    {
        deletePolylinePhantom();

        if (m_polyline)
        {
            deactivate();
            SF_DEL(m_polyline->m_physShape);
            SF_DEL(m_polyline);
        }
    }

    void ProceduralPolyline::init( Actor* _owner, IEventListener* _eventListener, bbool _usePhantom /* = bfalse*/ )
    {
        ITF_ASSERT(!m_polyline);
        m_polyline = newAlloc(mId_Gameplay, PolyLine);

        ACTOR_REGISTER_EVENT_COMPONENT(_owner,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076),_eventListener);
        ACTOR_REGISTER_EVENT_COMPONENT(_owner,ITF_GET_STRINGID_CRC(EventHanging,2826410601),_eventListener);

        m_polyline->setOwnerActor(_owner);
        m_polyline->setSerializable(bfalse);

        PhysBodyInitCommonInfo bodyInit;
        SET_OWNER(bodyInit, _owner->getRef());
        bodyInit.m_static = btrue;
        bodyInit.m_pos = m_polyline->get2DPos();
        bodyInit.m_angle = m_polyline->getAngle();
        bodyInit.m_userData = m_polyline->getRef();
        bodyInit.m_weight = 1.f;
        bodyInit.m_collisionGroup = ECOLLISIONGROUP_POLYLINE;
        bodyInit.m_collisionFilter = ECOLLISIONFILTER_CHARACTERS;

        m_polyline->m_physShape = newAlloc(mId_Phys,PhysShapePolyline(m_polyline));
        m_polyline->m_physBody = PHYSWORLD->allocBody(m_polyline->m_physShape,bodyInit);
        m_polyline->setGameMaterial(m_material.getStringID());

        if ( _usePhantom )
        {
            createPolylinePhantom();
        }
    }

    void ProceduralPolyline::createPolylinePhantom()
    {
        ITF_ASSERT( m_polyline != NULL );
        ITF_ASSERT( m_phantom == NULL );
        if ( m_phantom != NULL )
        {
            return;        
        }
        PhysPhantomInitCommonInfo initInfo;

        SET_OWNER(initInfo, m_polyline->getRef() );
        initInfo.m_shape = m_polyline->m_physShape;
        initInfo.m_pos = m_polyline->get2DPos();
        initInfo.m_angle = m_polyline->getAngle();
        initInfo.m_userData = m_polyline->getRef();
        initInfo.m_collisionGroup = ECOLLISIONGROUP_POLYLINE;

        m_phantom = PHYSWORLD->allocPhantom(initInfo);
        m_phantom->updateAABB();
        m_phantomRegistered = bfalse;
    }

    void ProceduralPolyline::deletePolylinePhantom()
    {
        if ( m_phantom != NULL )
        {
            if ( m_phantomRegistered )
            {
                PHYSWORLD->removePhantom( m_phantom );
                m_phantomRegistered = bfalse;
            }

            PHYSWORLD->deallocPhantom(m_phantom);
        }
    }

    void ProceduralPolyline::setGameMaterial( const Path& _value )
    {
        m_material = _value;

        if ( m_polyline )
        {
            m_polyline->setGameMaterial(_value.getStringID());
        }
    }

    void ProceduralPolyline::activate()
    {
        ITF_ASSERT(m_polyline);

        if (!m_active)
        {
            m_active = btrue;
            PHYSWORLD->insertBody(m_polyline->m_physBody, m_polyline->getOwnerActor()->getDepth());

            if ( ( m_phantom != NULL ) && !m_phantomRegistered )
            {
                PHYSWORLD->insertPhantom( m_phantom, m_polyline->getOwnerActor()->getDepth() );
                m_phantomRegistered = btrue;
            }
        }
    }

    void ProceduralPolyline::deactivate()
    {
        ITF_ASSERT(m_polyline);

        if (m_active)
        {
            m_active = bfalse;
            PHYSWORLD->removeBody(m_polyline->m_physBody);

            if ( ( m_phantom != NULL ) && m_phantomRegistered )
            {
                PHYSWORLD->removePhantom( m_phantom );
                m_phantomRegistered = bfalse;
            }

            EventDetach eventDetach(m_polyline->getRef(), U32_INVALID);
            for (u32 i = 0; i < m_objectsOnPolyline.size(); ++i)
            {
                Actor * actor = static_cast<Actor*>(GETOBJECT(m_objectsOnPolyline[i]));
                if (actor)
                {
                    actor->onEvent(&eventDetach);
                }
            }
        }
    }

    void ProceduralPolyline::setPoints( const SafeArray<Vec2d>& _points )
    {
        ITF_ASSERT(m_polyline);

        m_polyline->clear();

        for (u32 i = 0; i < _points.size(); i++)
        {
            m_polyline->addPoint(_points[i], i);
        }

        m_polyline->setGameMaterial(m_material.getStringID());
    }

    void ProceduralPolyline::update( f32 _dt )
    {
        ITF_ASSERT(m_polyline);

        ITF_ASSERT(m_active);
        if (!m_active)
            return;

        m_polyline->setGameMaterial(m_material.getStringID());
        m_polyline->setDepth(m_polyline->getOwnerActor()->getDepth());
        m_polyline->update(_dt);

        if ( ( m_phantom != NULL ) && m_phantomRegistered )
        {
            //m_phantom->setPos3D( m_polyline->getPos());
            m_phantom->setRecomputeAABB();
            m_phantom->updateAABB();
        }
    }

    void ProceduralPolyline::onEvent( Event* _event )
    {
        if ( EventStickOnPolyline* eventStick = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)) )
        {
            processStickToPolyline(eventStick);
        }
        else if ( EventHanging* eventHang = _event->DynamicCast<EventHanging>(ITF_GET_STRINGID_CRC(EventHanging,2826410601)) )
        {
            processHangOnPolyline(eventHang);
        }
    }

    void ProceduralPolyline::processStickToPolyline( EventStickOnPolyline* _eventStick )
    {
        ITF_ASSERT(m_polyline);

        if (_eventStick->getPolyline() == m_polyline->getRef())
        {
            if (_eventStick->isSticked())
            {
                if (m_objectsOnPolyline.find(_eventStick->getActor()) == -1)
                {
                    m_objectsOnPolyline.push_back(_eventStick->getActor());
                }
            }
            else
            {
                i32 index = m_objectsOnPolyline.find(_eventStick->getActor());
                if (index != -1)
                {
                    m_objectsOnPolyline.eraseNoOrder(index);
                }
            }
        }
    }

    void ProceduralPolyline::processHangOnPolyline( EventHanging* _eventHang )
    {
        ITF_ASSERT(m_polyline);

        if (_eventHang->getPolyline() == m_polyline->getRef())
        {
            if (_eventHang->isHanging())
            {
                if (m_objectsOnPolyline.find(_eventHang->getActor()) == -1)
                {
                    m_objectsOnPolyline.push_back(_eventHang->getActor());
                }
            }
            else
            {
                i32 index = m_objectsOnPolyline.find(_eventHang->getActor());
                if (index != -1)
                {
                    m_objectsOnPolyline.eraseNoOrder(index);
                }
            }
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void ProceduralPolyline::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        ITF_ASSERT(m_polyline || !m_active);

        if (!(_flags & ActorComponent::DrawEditFlag_Collisions))
            return;

        if (m_active)
        {
            _drawInterface->drawPolyLine(m_polyline, btrue);
        }
    }
#endif // ITF_SUPPORT_EDITOR

} // namespace ITF