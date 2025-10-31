#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_STIMCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StimComponent.h"
#endif //_ITF_RAY_STIMCOMPONENT_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_STIMSMANAGER_H_
#include "gameplay/managers/stimsmanager.h"
#endif //_ITF_STIMSMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_AIMANAGER_H_
#include "gameplay/managers/AIManager.h"
#endif //_ITF_AIMANAGER_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_StimComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_StimComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(Ray_StimComponent)
        // warn but don't invalidate
        ITF_WARNING_PARAM(m_actor, "useFixedAngle", !getTemplate()->getHitRadial() || !getTemplate()->getUseFixedAngle(), "fixedAngle will be ignored if the hit is radial");
    END_VALIDATE_COMPONENT()

    const Vec2d k_shapeAABBScale( Vec2d( 2.0f, 2.0f ) );

    Ray_StimComponent::Ray_StimComponent()
        : Super()
        , m_disabled(bfalse)
        , m_fixedAngle(bfalse, 0)
        , m_registered(bfalse)
        , m_previousShapePos(Vec2d::Zero)
    {
    }

    Ray_StimComponent::~Ray_StimComponent()
    {
        unregisterObjects();
    }

    void Ray_StimComponent::setDisabled( bbool _value ) 
    { 
        if ( m_disabled && !_value )
        {
            m_previousShapePos = Vec2d::Zero;
        }
        m_disabled = _value; 
    }

    void Ray_StimComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (!isValid() || m_disabled)
        {
            return;
        }

        const Vec2d& shapePos = getShapePos();
        
        if ( getTemplate()->getUseOutOfScreenOptim() )
        {
            getShape()->calculateAABB( shapePos, m_previousShapePos.IsEqual( Vec2d::Zero ) ? shapePos : m_previousShapePos, m_actor->getAngle(), m_shapeAABB );
            m_shapeAABB.ScaleFromCenter( k_shapeAABBScale );
            //GFX_ADAPTER->drawDBGAABB( m_shapeAABB, Color::yellow() );

            if ( !CAMERA->isRectVisible( m_shapeAABB, m_actor->getDepth() ) )
            {
                m_previousShapePos = shapePos;
                return;
            }
        }
        
        const f32 stimAngle = getTemplate()->getUseFixedAngle() ? m_fixedAngle.ToRadians() :
                                                                  m_actor->getAngle() + getTemplate()->getLocalAngleOffset().ToRadians();
        const Vec2d stimDir = Vec2d::Right.Rotate(stimAngle);

        PunchStim* stim = STIMSMANAGER->requestStim<PunchStim>(getShape());
#ifdef ITF_FINAL
        if ( stim )
#endif // ITF_FINAL
        {
            stim->setPos(shapePos);
            stim->setPrevPos( m_previousShapePos.IsEqual( Vec2d::Zero ) ? shapePos : m_previousShapePos );
            stim->setSender(m_actor->getRef());
            stim->setDepth(m_actor->getDepth());
            stim->setAngle(m_actor->getAngle());

            stim->setFaction(getFaction());
            stim->setReceivedHitType(getTemplate()->getHitType());
            stim->setLevel(getTemplate()->getHitLevel());
            stim->setIsRadial(getTemplate()->getHitRadial());
            stim->setDirection(stimDir);
            stim->setFxPos(shapePos.to3d(m_actor->getPos().m_z));

            stim->setHitEnemiesOnce(getTemplate()->getHitEnemiesOnce());

            STIMSMANAGER->sendStim(stim);
        }
        
        m_previousShapePos = shapePos;
    }


    void Ray_StimComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_fixedAngle = getTemplate()->getFixedAngle();
    }

    ITF_INLINE Faction Ray_StimComponent::getFaction() const
    {
        return getTemplate()->getFaction();
    }

    ITF_INLINE EReceivedHitType Ray_StimComponent::getHitType() const
    {
        return getTemplate()->getHitType();
    }

    ITF_INLINE bbool Ray_StimComponent::getUseFixedAngle() const
    {
        return getTemplate()->getUseFixedAngle();
    }

    void Ray_StimComponent::onBecomeActive()
    {
        Super::onBecomeActive(  );
        registerObjects();
    }

    void Ray_StimComponent::onBecomeInactive()
    {
        Super::onBecomeInactive(  );unregisterObjects();
    }

    void Ray_StimComponent::onDepthChanged( f32 _oldZ, f32 _newZ )
    {
        Super::onDepthChanged( _oldZ,_newZ );
        
        if (m_registered)
        {
            if (getTemplate()->getRegisterToAIManager())
            {
                AI_MANAGER->changeDepth(m_actor->getRef(),_oldZ,_newZ);

            }
        }
    }

    void Ray_StimComponent::registerObjects()
    {
        if (!m_registered)
        {
            m_registered = btrue;
            if (getTemplate()->getRegisterToAIManager())
            {
                AIData * data = AI_MANAGER->addActor(m_actor->getRef(),m_actor->getDepth());
                if (data)
                    data->m_faction = getTemplate()->getFaction();
            }
        }
    }

    void Ray_StimComponent::unregisterObjects()
    {
        if (m_registered)
        {
            m_registered = bfalse;
            if ( getTemplate()->getRegisterToAIManager())
            {
                AI_MANAGER->removeActor(m_actor->getRef(),m_actor->getDepth());
            }
        }
    }

#ifdef ITF_SUPPORT_DEBUGFEATURE
    void Ray_StimComponent::drawDebug() const
    {
        if (!m_disabled)
        {
            //Super::drawDebug();

            DebugDraw::shape(getShapePos(), m_actor->getPos().m_z, m_actor->getAngle(), getShape(),
                Color::white(), 0.f, "stim", 25 * MTH_DEGTORAD);


            f32 angle = getTemplate()->getUseFixedAngle() ?
                m_fixedAngle.ToRadians() :
            m_actor->getAngle() + getTemplate()->getLocalAngleOffset().ToRadians();
            Vec2d dir = Vec2d::Right.Rotate(angle);
            DebugDraw::arrow2D(m_actor->get2DPos(), m_actor->get2DPos() + dir, m_actor->getDepth());
        }
    }
#endif // ITF_SUPPORT_DEBUGFEATURE

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_StimComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_StimComponent_Template)
        SERIALIZE_RAY_FACTION("faction", m_faction);
        SERIALIZE_MEMBER("useFixedAngle", m_useFixedAngle);
        SERIALIZE_MEMBER("fixedAngle", m_fixedAngle);
        SERIALIZE_MEMBER("localAngleOffset", m_localAngleOffset);
        SERIALIZE_RECEIVEDHITTYPE("hitType", m_hitType);
        SERIALIZE_MEMBER("hitLevel", m_hitLevel);
        SERIALIZE_MEMBER("hitRadial", m_hitRadial);
        SERIALIZE_MEMBER("useNormal", m_useNormal);
        SERIALIZE_MEMBER("registerToAIManager", m_registerToAIManager);
        SERIALIZE_MEMBER("useOutOfScreenOptim", m_useOutOfScreenOptim );
        SERIALIZE_MEMBER("hitEnemiesOnce", m_hitEnemiesOnce);
    END_SERIALIZATION()

    Ray_StimComponent_Template::Ray_StimComponent_Template()
    :Super()
    , m_faction(RAY_FACTION_NEUTRAL)
    , m_useFixedAngle(bfalse)
    , m_fixedAngle(bfalse, 0)
    , m_localAngleOffset(bfalse, 0)
    , m_hitType(RECEIVEDHITTYPE_FRONTPUNCH)
    , m_hitLevel(0)
    , m_hitRadial(bfalse)
    , m_useNormal(bfalse)
    , m_registerToAIManager(btrue)
    , m_useOutOfScreenOptim( btrue )
    , m_hitEnemiesOnce(bfalse)
    {
    }
};