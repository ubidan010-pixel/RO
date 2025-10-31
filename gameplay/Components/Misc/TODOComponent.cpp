#include "precompiled_gameplay.h"

#ifndef _ITF_TODOCOMPONENT_H_
#include "gameplay/Components/Misc/TODOComponent.h"
#endif //_ITF_TODOCOMPONENT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(TODOComponent)

BEGIN_SERIALIZATION_CHILD(TODOComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("TextLabel", m_TextLabel);
        SERIALIZE_MEMBER("drawUsingEngine", m_drawUsingEngine);
        SERIALIZE_MEMBER("drawBoxWidth", m_drawBoxWidth);
        SERIALIZE_MEMBER("drawBoxHeight", m_drawBoxHeight);
        SERIALIZE_MEMBER("textSize", m_textSize);
        SERIALIZE_MEMBER("textAlpha", m_textAlpha);
        SERIALIZE_MEMBER("textRed", m_textRed);
        SERIALIZE_MEMBER("textGreen", m_textGreen);
        SERIALIZE_MEMBER("textBlue", m_textBlue);
        SERIALIZE_MEMBER("backgroundAlpha", m_backgroundAlpha);
        SERIALIZE_MEMBER("backgroundRed", m_backgroundRed);
        SERIALIZE_MEMBER("backgroundGreen", m_backgroundGreen);
        SERIALIZE_MEMBER("backgroundBlue", m_backgroundBlue);
        SERIALIZE_MEMBER("centerText", m_center);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(TODOComponent)

    VALIDATE_COMPONENT_PARAM("textPath", !getTemplate()->getTextPath().isEmpty(), "You need to specify which actor to use for the text");

END_VALIDATE_COMPONENT()

TODOComponent::TODOComponent()
: Super()
, m_textRef(ITF_INVALID_OBJREF)
, m_drawUsingEngine(btrue)
, m_drawBoxWidth(200.f)
, m_drawBoxHeight(100.f)
, m_textAlpha(1.0f)
, m_textRed(0.0f)
, m_textGreen(0.0f)
, m_textBlue(0.0f)
, m_textSize(25.0f)
, m_backgroundAlpha(0.0f)
, m_backgroundRed(1.0f)
, m_backgroundGreen(1.0f)
, m_backgroundBlue(1.0f)
, m_center(btrue)
, m_activated(btrue)
{
}


TODOComponent::~TODOComponent()
{
}

void TODOComponent::onActorClearComponents()
{
    Actor* textActor = m_textRef.getActor();

    if ( textActor && !textActor->isDestructionRequested() )
    {
        Scene* textScene = textActor->getScene();

        if ( textScene )
        {
            textScene->unregisterPickable(textActor);
        }
    }
}

void TODOComponent::onBecomeActive()
{
    Super::onBecomeActive();

    Actor* textActor = m_textRef.getActor();

    if ( textActor )
    {
        textActor->enable();
    }
}

void TODOComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    Actor* textActor = m_textRef.getActor();

    if ( textActor )
    {
        textActor->disable();
    }
}

void TODOComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    if ( !getTemplate()->getTextPath().isEmpty() )
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_spawner, getTemplate()->getTextPath());

        Actor* textActor = m_spawner.getSpawnee(m_actor->getScene(),m_actor->getPos(),m_actor->getAngle());

        if ( textActor )
        {
            m_textRef = textActor->getRef();
        }
    }

    // register activation event
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventActivate,2639596100),this);
}

void TODOComponent::Update( f32 _dt )
{
    if (m_drawUsingEngine && m_activated )
    {
        Actor* textActor = m_textRef.getActor();

        if ( textActor )
        {
            UITextBox* textComponent = textActor->GetComponent<UITextBox>();

            if ( textComponent )
            {
                TextInfo textInfo;
                textInfo.m_color = Color(m_textAlpha, m_textRed, m_textGreen, m_textBlue).getAsU32();
                textInfo.m_size = m_textSize;
                textInfo.m_text.setText(m_TextLabel.cStr());

                UITextBox::BoxPosition alignement = UITextBox::BoxPosition_TopLeft;
                if(m_center)
                    alignement = UITextBox::BoxPosition_Center;

                Vec3d pos;

                GFX_ADAPTER->compute3DTo2D(m_actor->getPos(), pos);

                textComponent->setData(pos,m_textSize,textInfo,m_drawBoxWidth,m_drawBoxHeight,UI_TEXTMANAGER->getTextsDepthRank(),Vec2d::Zero,Vec2d::One,Color(m_backgroundAlpha, m_backgroundRed, m_backgroundGreen, m_backgroundBlue).getAsU32(), btrue, alignement);      
                textActor->setPos(pos);

                const AABB & screenAABB = GFX_ADAPTER->getScreenAABB();

                if ( !textActor->getAABB().checkOverlap(screenAABB) )
                {
                    textActor->disable();
                }
                else
                {
                    textActor->enable();
                }
            }
        }
    }
}

void TODOComponent::onEvent(Event * _event)
{
    if (EventActivate * eventActivate = _event->DynamicCast<EventActivate>(ITF_GET_STRINGID_CRC(EventActivate,2639596100)))
    {
        m_activated = eventActivate->getActivated();
    }
}

void TODOComponent::setTextColor(f32 _alpha, f32 _red, f32 _green, f32 _blue)
{
	m_textAlpha		= _alpha;
	m_textRed		= _red;
	m_textGreen		= _green;
	m_textBlue		= _blue;
}

//-------------------------------------------------------------------------------------

IMPLEMENT_OBJECT_RTTI(TODOComponent_Template)
BEGIN_SERIALIZATION(TODOComponent_Template)

    SERIALIZE_MEMBER("textPath",m_textPath);

END_SERIALIZATION()

}

