#include "precompiled_gameplay.h"

#ifndef _ITF_AFTERFXCONTROLLERCOMPONENT_H_
#include "gameplay/Components/Misc/AfterFxControllerComponent.h"
#endif //_ITF_AFTERFXCONTROLLERCOMPONENT_H_

#ifndef _ITF_AFTERFXCOMPONENT_H_
#include "engine/actors/components/AfterFxComponent.h"
#endif //_ITF_AFTERFXCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(AfterFxControllerComponent_Template)
    BEGIN_SERIALIZATION(AfterFxControllerComponent_Template)
    SERIALIZE_MEMBER("useCircle", m_useCircle);
    SERIALIZE_MEMBER("useBox", m_useBox);
    SERIALIZE_MEMBER("minRange", m_minRange);
    SERIALIZE_MEMBER("maxRange", m_maxRange);
    SERIALIZE_MEMBER("minColor", m_minColor);
    SERIALIZE_MEMBER("maxColor", m_maxColor);
    SERIALIZE_OBJECT("boxShape", m_boxShape);
    SERIALIZE_MEMBER("inactiveWhenOutOfRange", m_inactiveWhenOutOfRange);

    END_SERIALIZATION()

    AfterFxControllerComponent_Template::AfterFxControllerComponent_Template()
        : m_minRange(0.0f)
        , m_maxRange(0.0f)
        , m_useCircle(btrue)
        , m_useBox(bfalse)
        , m_inactiveWhenOutOfRange(bfalse)
    {
    }

    AfterFxControllerComponent_Template::~AfterFxControllerComponent_Template()
    {
    }

    IMPLEMENT_OBJECT_RTTI(AfterFxControllerComponent)

    BEGIN_SERIALIZATION_CHILD(AfterFxControllerComponent)
    END_SERIALIZATION()


    BEGIN_VALIDATE_COMPONENT(AfterFxControllerComponent)
        VALIDATE_COMPONENT_PARAM("", m_afterFxComponent, "AfterFxControllerComponent requires an AfterFxComponent");
    END_VALIDATE_COMPONENT()


    AfterFxControllerComponent::AfterFxControllerComponent()
        : Super()
        , m_afterFxComponent(NULL)
        , m_outOfRange(bfalse)
    {
    }

    AfterFxControllerComponent::~AfterFxControllerComponent()
    {
    }

    void AfterFxControllerComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_afterFxComponent = m_actor->GetComponent<AfterFxComponent>();
    
        m_afterFxComponent->setColorRTarget( getTemplate()->getMaxColor() );
    } 

    void AfterFxControllerComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        Actor* player = AIUtils::getLeadLivePlayer(m_actor);
        if (!player)
        {
            return;
        }

        f32 distance = F32_INFINITY;
        if( getTemplate()->useCircle() )
        {

            distance = (player->get2DPos() - m_actor->get2DPos()).norm();
        }
        else if( getTemplate()->useBox() )
        {
            AABB box = getTemplate()->getBoxShape();
            box.Scale( m_actor->getScale() );
            box.moveCenter( m_actor->get2DPos() );
            distance = box.getDistMinPosAABB( player->get2DPos() );
        }
        else
        {
            ITF_ASSERT(0);
        }

        const f32 minRange = getTemplate()->getMinRange();
        const f32 maxRange = getTemplate()->getMaxRange();
        f32 ratio = 0.0f;
        if( maxRange - minRange > 0.0f )
        {
            ratio = InverseInterpolate(getTemplate()->getMinRange(), getTemplate()->getMaxRange(), Clamp(distance, getTemplate()->getMinRange(), getTemplate()->getMaxRange()));
        }
        else
        {
            ratio = distance <= minRange ? 0.0f : 1.0f;
        }

        //check after fx activation depending on current range
        if( getTemplate()->inactiveWhenOutOfRange() )
        {
            manageActivationFromRangeRatio(ratio);
        }

        m_afterFxComponent->setColorRTarget( Color::Interpolate(getTemplate()->getMinColor(), getTemplate()->getMaxColor(), ratio) );

        AABB aabb(Vec2d::Zero);
        if( getTemplate()->useCircle() )
        {
            aabb.grow(AABB( Vec2d(-getTemplate()->getMaxRange(), -getTemplate()->getMaxRange()), Vec2d(getTemplate()->getMaxRange(), getTemplate()->getMaxRange()) ));
        }
        else if( getTemplate()->useBox() )
        {
            const f32 range = getTemplate()->getMaxRange();
            const Vec2d & actorScale = m_actor->getScale();

            Vec2d min;
            min = getTemplate()->getBoxShape().getMin() * actorScale - range;

            Vec2d max;
            max = getTemplate()->getBoxShape().getMax() * actorScale + range;

            aabb.setMin(min);
            aabb.setMax(max);
        }
        m_actor->growRelativeAABB(aabb);

        //LOG("Ratio : %f", ratio);
    }

    void AfterFxControllerComponent::manageActivationFromRangeRatio(const f32 _rangeRatio)
    {
        const bbool outOfRange = _rangeRatio == 1.0f;
        if( m_outOfRange && !outOfRange )
        {
            EventActivate activateEvent;
            activateEvent.setActivated(btrue);
            m_afterFxComponent->onEvent(&activateEvent);                
        }
        else if( !m_outOfRange && outOfRange )
        {
            EventActivate activateEvent;
            activateEvent.setActivated(bfalse);
            m_afterFxComponent->onEvent(&activateEvent);
        }
        m_outOfRange = outOfRange;
    }

#ifdef ITF_SUPPORT_EDITOR
    void AfterFxControllerComponent::drawEdit( ActorDrawEditInterface* _drawInterface, u32 _flags ) const
    {
        Super::drawEdit(_drawInterface, _flags);

        if (_flags != ActorComponent::DrawEditFlag_All)
            return;

        if( getTemplate()->useCircle() )
        {
            GFX_ADAPTER->drawDBGCircle(m_actor->getPos(), getTemplate()->getMinRange(), getTemplate()->getMinColor());
            GFX_ADAPTER->drawDBGCircle(m_actor->getPos(), getTemplate()->getMaxRange(), getTemplate()->getMaxColor());
        }
        else if( getTemplate()->useBox() )
        {
            const Vec2d & actorPos = m_actor->get2DPos();

            AABB box = getTemplate()->getBoxShape();
            box.Scale(m_actor->getScale());
            box.moveCenter( actorPos );
            GFX_ADAPTER->drawDBGAABB(box, getTemplate()->getMinColor(), 0.f, 1.0f);

            
            const f32 range = getTemplate()->getMaxRange();
            const Vec2d & actorScale = m_actor->getScale();

            Vec2d min;
            min.m_x = getTemplate()->getBoxShape().getMin().m_x * actorScale.m_x - range;
            min.m_y = getTemplate()->getBoxShape().getMin().m_y * actorScale.m_y - range;


            Vec2d max;
            max.m_x = getTemplate()->getBoxShape().getMax().m_x * actorScale.m_x + range;
            max.m_y = getTemplate()->getBoxShape().getMax().m_y * actorScale.m_y + range;

            box.setMin(min);
            box.setMax(max);
            box.moveCenter( actorPos );
            GFX_ADAPTER->drawDBGAABB(box, getTemplate()->getMaxColor(), 0.f, 1.0f);
        }
    }
#endif // ITF_SUPPORT_EDITOR

};