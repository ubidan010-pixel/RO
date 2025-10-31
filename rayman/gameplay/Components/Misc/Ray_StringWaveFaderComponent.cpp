#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_STRINGWAVEFADERCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_StringWaveFaderComponent.h"
#endif //_ITF_RAY_STRINGWAVEFADERCOMPONENT_H_

#ifndef _ITF_ANIMATEDCOMPONENT_H_
#include "engine/actors/components/animatedcomponent.h"
#endif //_ITF_ANIMATEDCOMPONENT_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_FRISE_H_
#include "engine/display/Frieze/Frieze.h"
#endif //_ITF_FRISE_H_

#ifndef _ITF_POLYLINECOMPONENT_H_
#include "gameplay/Components/Environment/PolylineComponent.h"
#endif //_ITF_POLYLINECOMPONENT_H_

#ifndef _ITF_FXCONTROLLERCOMPONENT_H_
#include "gameplay/components/Misc/FXControllerComponent.h"
#endif //_ITF_FXCONTROLLERCOMPONENT_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_StringWaveFaderComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_StringWaveFaderComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_StringWaveFaderComponent)
        VALIDATE_COMPONENT_PARAM("", m_animatedComponent, "no animated component");
        VALIDATE_COMPONENT_PARAM("", m_linkComponent, "no animated component");
    END_VALIDATE_COMPONENT()

    #define _WAVEFADER_USE_ON_OFF

    ///////////////////////////////////////////////////////////////////////////////////////////

    Ray_StringWaveFaderComponent::Ray_StringWaveFaderComponent()
        : Super()
        , m_pos(1)
        , m_speed(0)
        , m_animatedComponent(NULL)
        , m_linkComponent(NULL)
        , m_isOn(btrue)
        , m_fxControllerComponent(NULL)
        , m_downwardFX(U32_INVALID)
    {
    }

    Ray_StringWaveFaderComponent::~Ray_StringWaveFaderComponent()
    {
    }

    void Ray_StringWaveFaderComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();
        m_linkComponent = m_actor->GetComponent<LinkComponent>();
        m_fxControllerComponent = m_actor->GetComponent<FXControllerComponent>();

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076), this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(PunchStim,200533519), this);

        resetState();
    }

    void Ray_StringWaveFaderComponent::resetState()
    {
        m_pos = 1;
        m_speed = 0;

        updateOnOff(bfalse);
        updateLinkeeScale(0);
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
#if 0
    void Ray_StringWaveFaderComponent::sendOnOffToLinkee(bbool _on)
    {
        //linkees
        EventTrigger triggerEvent;
        triggerEvent.setActivated(_on);
        triggerEvent.setSender(m_actor->getRef());
        triggerEvent.setActivator(m_actor->getRef());

        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList &children = m_linkComponent->getChildren();
            for (u32 i=0; i<children.size(); i++)
            {
                Pickable *p= SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());

                if (!p)
                {
                    continue;
                }

                //2 cases : actor/friezes
                switch(p->getObjectType())
                {
                case BaseObject::eActor:
                    {
                        ((Actor*)p)->onEvent(&triggerEvent);
                        Vec2d scale=p->getScale();
                        scale.m_y = newScale;
                        p->setScale(scale);
                        break;
                    }
                case BaseObject::eFrise:
                    {
                        Frise *frieze=(Frise*)p;
                        const SafeArray<ObjectRef> &generators = frieze->getStringWaveGenerators();
                        u32 count = generators.size();
                        for (u32 i=0; i<count; i++)
                        {
                            Actor *act = IRTTIObject::SafeDynamicCast<Actor>(generators[i].getObject(),ITF_GET_STRINGID_CRC(Actor,2546623115));
                            if (act)
                            {
                                act->onEvent(&triggerEvent);
                            }
                        }
                        break;
                    }

                }
            }
        }
    }
#endif

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_StringWaveFaderComponent::updateLinkeeScale(f32 _dt)
    {
        f32 scalePercent = m_pos;
        scalePercent = std::min(std::max(scalePercent,0.f),1.f);

        EventTrigger triggerEvent;
        triggerEvent.setActivated(m_isOn);
        triggerEvent.setSender(m_actor->getRef());
        triggerEvent.setActivator(m_actor->getRef());


        //send scale to children
        f32 newScale = getTemplate()->getLinkeeScaleForOff()*(1-scalePercent)
            + getTemplate()->getLinkeeScaleForOn()*scalePercent;

        //linkees
        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList &children = m_linkComponent->getChildren();
            for (u32 i=0; i<children.size(); i++)
            {
                Pickable *p= SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[i].getPath());

                if (!p)
                {
                    continue;
                }

                //2 cases : actor/friezes
                switch(p->getObjectType())
                {
                case BaseObject::eActor:
                    {
                        Vec2d scale=p->getScale();
                        scale.m_y = newScale;
                        p->setScale(scale);
                        ((Actor*)p)->onEvent(&triggerEvent);
                        break;
                    }
                case BaseObject::eFrise:
                    {
                        Frise *frieze=(Frise*)p;
                        const SafeArray<ObjectRef> *pGenerators = frieze->getStringWaveGenerators();

                        if ( !pGenerators )
                            break;

                        const SafeArray<ObjectRef>& generators  = *pGenerators;

                        u32 count = generators.size();
                        for (u32 i=0; i<count; i++)
                        {
                            Actor *act = IRTTIObject::SafeDynamicCast<Actor>(generators[i].getObject(),ITF_GET_STRINGID_CRC(Actor,2546623115));
                            if (act)
                            {
                                Vec2d scale=act->getScale();
                                scale.m_y = newScale;
                                act->setScale(scale);
                                act->onEvent(&triggerEvent);
                            }
                        }
                        break;
                    }

                }
            }
        }
        

    }

    void Ray_StringWaveFaderComponent::updateOnOff(bbool _fxOnChange)
    {
#ifdef _WAVEFADER_USE_ON_OFF
        bbool oldIsOn = m_isOn;
        if (getTemplate()->getLinkeeScaleForOff()<getTemplate()->getLinkeeScaleForOn())
        {
            m_isOn = m_pos>getTemplate()->getThresholdPosition();
        }        
        else
        {
            m_isOn = m_pos<getTemplate()->getThresholdPosition();
        }
        if (m_isOn != oldIsOn)
        {
            if (_fxOnChange)
            {
                //todo : fx
            }
           
        }
#endif
    }

    bbool Ray_StringWaveFaderComponent::hasCollision(PunchStim *_hit, const StringID &_polyline)
    {
        const PolylineComponent *comp = (PolylineComponent*)m_actor->GetComponent<PolylineComponent>();
        if (comp)
        {
            for (u32 contactIndex=0; contactIndex<_hit->getContacts().size(); contactIndex++)
            {
                const SCollidableContact &contact = _hit->getContacts()[contactIndex];
                ObjectRef polylineRef = contact.m_collidableUserData;

                const PolylineComponent::ProceduralPolyline *line = comp->getProcPolylineFromObjRef(polylineRef);
                if (line && line->m_animRef == _polyline)
                {
                    return btrue;
                }
            }
        }
        return bfalse;
    }

    void Ray_StringWaveFaderComponent::onEvent( Event* _event )
    {
        Super::onEvent(_event);

        //Detect objects on top
        if (EventStickOnPolyline * stickOnPolyEvent = _event->DynamicCast<EventStickOnPolyline>(ITF_GET_STRINGID_CRC(EventStickOnPolyline,471231076)))
        {
            ActorRef obj = stickOnPolyEvent->getActor();
            if (stickOnPolyEvent->isSticked())
            {
                if (m_actorsOnTop.find(obj)<0)
                {
                    m_actorsOnTop.push_back(obj);
                }
            }
            else
            {
                i32 pos = m_actorsOnTop.find(obj);
                if (pos>=0)
                {
                    m_actorsOnTop.eraseNoOrder((u32)pos);
                }
            }
        }
        else if (PunchStim* hit = _event->DynamicCast<PunchStim>(ITF_GET_STRINGID_CRC(PunchStim,200533519)) )
        {
            if (hit->getReceivedHitType()==RECEIVEDHITTYPE_EARTHQUAKE)
            {
                if (hasCollision(hit, ITF_GET_STRINGID_CRC(L_Col_Fader,2061499968)))
                {
                    m_speed = std::min(m_speed,-getTemplate()->getCrushSpeed());
                }
            }
        }

    }

    void Ray_StringWaveFaderComponent::Update( f32 _dt )
    {
        Super::Update(_dt);
        
        f32 massOnTop = (f32)m_actorsOnTop.size();
        f32 acceleration;
        if (massOnTop>0)
        {
            acceleration = -getTemplate()->getDownwardAcceleration();
        }
        else
        {
            acceleration = getTemplate()->getUpwardAcceleration();
        }

        //limit speed
        f32 oldSpeed = m_speed;
        bbool oldWasAtEnd = (m_pos==0) || (m_pos==1);
        m_speed += acceleration*_dt;
        if (m_speed>0.f)
        {
            m_speed = std::min(m_speed, getTemplate()->getMaxUpwardSpeed());
        }
        else if (m_speed<0.f)
        {
            m_speed = -std::min(-m_speed, getTemplate()->getMaxDownwardSpeed());
        }

        //limitate pos
        bbool bounceOnEnd = bfalse;
        m_pos += m_speed*_dt;
        if (m_speed>0.f)
        {
            if (m_pos>=1.f)
            {
                m_pos = 1.f;
                bounceOnEnd = btrue;
            }
        }
        else if (m_speed<0.f)
        {
            if (m_pos<=0.f)
            {
                m_pos = 0.f;
                bounceOnEnd = btrue;
            }
        }
        m_pos = std::max(std::min(m_pos,1.f),0.f);


        //bouncing
        if (bounceOnEnd)
        {
            if (f32_Abs(m_speed)<getTemplate()->getMinBounceSpeed())
            {
                m_speed = 0.f;
            }
            else
            {
                m_speed = -m_speed*getTemplate()->getBounceFactor();
            }
        }

        //
        if (m_fxControllerComponent)
        {
            bbool killDownwardFX = bfalse;

            bbool isFastEnough = f32_Abs(oldSpeed)>getTemplate()->getMinSpeedForSound();
            if (bounceOnEnd)
            {
                killDownwardFX = btrue;
                if (!oldWasAtEnd && isFastEnough)
                {
                    m_fxControllerComponent->playFX(getTemplate()->getBounceOnEndFX());
                }
            }
            else
            {
                if (m_downwardFX==U32_INVALID && isFastEnough)
                {
                    m_downwardFX = m_fxControllerComponent->playFX(getTemplate()->getGoDownwardFX());
                }
            }
            if (killDownwardFX)
            {
                m_fxControllerComponent->stopFX(m_downwardFX);
                m_downwardFX = U32_INVALID;
            }

        }
        //
        m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(platformPos,4157288625), 1-m_pos);

        updateOnOff(btrue);
        updateLinkeeScale(_dt);
    }


    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_StringWaveFaderComponent_Template)
    BEGIN_SERIALIZATION(Ray_StringWaveFaderComponent_Template)
        SERIALIZE_MEMBER("minBounceSpeed", m_minBounceSpeed);
        SERIALIZE_MEMBER("maxDownwardSpeed", m_maxDownwardSpeed);
        SERIALIZE_MEMBER("maxUpwardSpeed", m_maxUpwardSpeed);
        SERIALIZE_MEMBER("upwardAcceleration", m_upwardAcceleration);
        SERIALIZE_MEMBER("downwardAcceleration", m_downwardAcceleration);
        SERIALIZE_MEMBER("bounceFactor", m_bounceFactor);
        SERIALIZE_MEMBER("thresholdPosition", m_thresholdPosition);
        SERIALIZE_MEMBER("linkeeScaleForOff", m_linkeeScaleForOff);
        SERIALIZE_MEMBER("linkeeScaleForOn", m_linkeeScaleForOn);
        SERIALIZE_MEMBER("crushSpeed", m_crushSpeed);
        SERIALIZE_MEMBER("bounceOnEndFX", m_bounceOnEndFX);
        SERIALIZE_MEMBER("goDownwardFX", m_goDownwardFX);
        SERIALIZE_MEMBER("minSpeedForSound", m_minSpeedForSound);
    END_SERIALIZATION()

    Ray_StringWaveFaderComponent_Template::Ray_StringWaveFaderComponent_Template()
    : m_minBounceSpeed(0.1f)
    , m_maxDownwardSpeed(1.f)
    , m_maxUpwardSpeed(0.2f)
    , m_upwardAcceleration(0.2f)
    , m_downwardAcceleration(1.f)
    , m_bounceFactor(0.25f)
    , m_thresholdPosition(0.25f)
    , m_linkeeScaleForOn(1.0f)
    , m_linkeeScaleForOff(0.0f)
    , m_crushSpeed(1.f)
    , m_minSpeedForSound(0.1f)
    , m_bounceOnEndFX("Fader_Clic")
    , m_goDownwardFX("Fader_Move")
    {
    }


}
