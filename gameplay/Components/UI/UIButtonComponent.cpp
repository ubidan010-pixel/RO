#include "precompiled_gameplay.h"

#ifndef _ITF_UIBUTTONCOMPONENT_H_
#include "gameplay/components/UI/UIButtonComponent.h"
#endif //_ITF_UIBUTTONCOMPONENT_H_

#ifndef _ITF_UIMENUMANAGER_H_
#include "engine/actors/managers/UIMenuManager.h"
#endif //_ITF_UIMENUMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_LINKCOMPONENT_H_
#include "gameplay/Components/Misc/LinkComponent.h"
#endif //_ITF_LINKCOMPONENT_H_

#ifndef _ITF_SCENEOBJECTPATH_H_
#include "engine/scene/SceneObjectPath.h"
#endif //_ITF_SCENEOBJECTPATH_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIButtonComponent)
    BEGIN_SERIALIZATION_CHILD(UIButtonComponent)
    SERIALIZE_MEMBER("lineId",m_lineId);
    SERIALIZE_MEMBER("offset1",m_offset1);
    SERIALIZE_MEMBER("offset2",m_offset2);
    SERIALIZE_MEMBER("offset3",m_offset3);
    SERIALIZE_MEMBER("isExtremity",m_isExtremity);
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIButtonComponent::UIButtonComponent()
    : m_state(State_Idle)
    ,m_isTextBoxActive(btrue)
    ,m_textBox()
    ,m_currentAnimationIndex(0)
    ,m_linkComponent(NULL)
    ,m_firstUpdate(btrue)
    ,m_timer(0.0f)
    ,m_curScale(1.0f)
    ,m_offset2(0.0f)
    ,m_offset1(0.0f)
    ,m_offset3(0.0f)
    ,m_isExtremity(bfalse)
    ,m_curActorScale(1.0f)
    {}

    ///////////////////////////////////////////////////////////////////////////////////////////
    UIButtonComponent::~UIButtonComponent()
    {
        clear();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIButtonComponent::clear()
    {
        UI_MENUMANAGER->removeUIComponentOfTheMenu(this);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIButtonComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        m_actor->setAlwaysVisible(btrue);

        //changeState(State_Idle);

        m_linkComponent = m_actor->GetComponent<LinkComponent>();

        if (!getTemplate()->getTextPath().isEmpty())
        {
            SPAWNER->declareNeedsSpawnee(m_actor, &m_spawner,getTemplate()->getTextPath());
        }

        if(getTemplate()->getIs2dActor())
            m_actor->set2DActor(btrue);

        m_firstUpdate = btrue;

        if (m_linkComponent)
        {
            const LinkComponent::ChildrenList & children = m_linkComponent->getChildren();
            if (children.size() >= 2)
            {
                if ( Pickable* pickable1 = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[0].getPath()) )
                {
                    Actor * actor = pickable1->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) ;
                    if (actor)
                    {
                        UIButtonComponent * buttonComponent = actor->GetComponent<UIButtonComponent>();
                        if (buttonComponent)
                        {
                            m_child1 = actor->getRef();
                        }
                    }
                }
                if ( Pickable* pickable2 = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[1].getPath()) )
                {
                    Actor * actor = pickable2->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) ;
                    if (actor)
                    {
                        UIButtonComponent * buttonComponent = actor->GetComponent<UIButtonComponent>();
                        if (buttonComponent)
                        {
                            m_child2 = actor->getRef();
                        }
                    }
                }
                if (children.size() >= 3)
                {
                    if ( Pickable* pickable3 = SceneObjectPathUtils::getObjectFromRelativePath(m_actor, children[2].getPath()) )
                    {
                        Actor * actor = pickable3->DynamicCast<Actor>(ITF_GET_STRINGID_CRC(Actor,2546623115)) ;
                        if (actor)
                        {
                            UIButtonComponent * buttonComponent = actor->GetComponent<UIButtonComponent>();
                            if (buttonComponent)
                            {
                                m_child3 = actor->getRef();
                            }
                        }
                    }
                }
            }
        }

    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UIButtonComponent::Update( f32 _deltaTime )
    {
        Super::Update(_deltaTime);

        if (m_firstUpdate)
        {
            m_firstUpdate = bfalse;
            m_initPos.m_x = m_relativePosX;
            m_initPos.m_y = m_relativePosY;
            return;
        }

        if (!m_textBox.isValid() && !getTemplate()->getTextPath().isEmpty())
        {
            Actor * textBox = m_spawner.getSpawnee(m_actor->getScene(),m_actor->getPos(),m_actor->getAngle());
            if(textBox)
            {
                textBox->set2DActor(btrue);
                m_textBox = textBox->getRef();
            }
        }

        Actor * actor = m_textBox.getActor();
        UITextBox * textBox = NULL;

        if (actor && m_isTextBoxActive)
        {
            textBox = actor->GetComponent<UITextBox>();            
            if(!textBox)
                m_isTextBoxActive = bfalse;
            else
                textBox->setNeedToComputePosition(bfalse);

            TextInfo textInfo;
            textInfo.m_color = getColor();
            textInfo.m_size = getFontHeight();
            textInfo.m_text.setText(m_content.cStr());
            textInfo.m_lineId = m_lineId;

            // get actor

            f32 textAngle = GetActor()->getAngle();
            Vec3d textPos = GetActor()->getPos();
            Vec2d textScale = Vec2d::One;

            updateState(textPos, textScale, textAngle);

            if ( m_animLightComponent )
            {
                Vec2d animScale = Vec2d::One;
                updateTextFromAnim(textPos, animScale, textAngle);
                textScale *= animScale;
            }

            // Apply this on actor
            actor->setPos(textPos);
            actor->setAngle(textAngle);
            actor->setScale(textScale);

            if (textBox)
            {
                textBox->setData(textPos
                    , textInfo.m_size
                    , textInfo
                    , 0.0f
                    , 0.0f
                    , getDepthRank()
                    , Vec2d::Zero
                    , Vec2d::One
                    , COLOR_WHITE
                    , bfalse
                    , UITextBox::BoxPosition_Center
                    , getShadowOffset()
                    , getShadowColor()
                    , getTemplate()->getLineSpacingFactor()
                    , ResourceID::Invalid
                    , m_fontIndex
                    , getTemplate()->getTextMode()
                    , getTemplate()->getTextModeY());
            
                textBox->setNoScaleWithActor(bfalse);
            }
        }

        m_timer += _deltaTime;
    }

    void UIButtonComponent::changeState( ButtonState newState )
    {
        if(m_state == newState)return;

        m_state = newState;
        switch(newState)
        {
        case State_Idle:        changeAnimation(ITF_GET_STRINGID_CRC(IDLE,1633200266));          break;
        case State_onRollover:  changeAnimation(ITF_GET_STRINGID_CRC(ROLLOVER,4038735874));    break;
        case State_onRollout:   changeAnimation(ITF_GET_STRINGID_CRC(ROLLOUT,2734166257));     break;
        case State_onReleased:  changeAnimation(ITF_GET_STRINGID_CRC(RELEASE,3780715669));    break;
        case State_onPressed:   changeAnimation(ITF_GET_STRINGID_CRC(PRESS,2931858408));     break;
        case State_Idle_Selected: 
            changeAnimation(ITF_GET_STRINGID_CRC(IDLE_SELECTED,3444845800)); 
            m_timer = 0.0f;
            break;
        }
    }

    void UIButtonComponent::changeAnimation( const StringID & AnimationID )
    { 
        if(!m_animLightComponent)return;

        const SubAnim* subAnim = m_animLightComponent->getSubAnim(AnimationID);        
        if(subAnim)
        {
            m_currentAnimationName = AnimationID;
            m_animLightComponent->setAnim(subAnim->getFriendlyName());
            m_currentAnimationIndex = m_animLightComponent->getSubAnimSet()->getSubAnimIndex(m_currentAnimationName);
        }
    }

#ifdef ITF_SUPPORT_EDITOR
    void UIButtonComponent::drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const
    {
    }

    void UIButtonComponent::onEditorSelected()
    {
        changeState(State_onRollover);
    }
#endif // ITF_SUPPORT_EDITOR

    void UIButtonComponent::onPressed()
    {
        changeState(State_onPressed);
    }

    void UIButtonComponent::onReleased()
    {
        changeState(State_onReleased);
    }

    void UIButtonComponent::onRollover()
    {
        changeState(State_onRollover);
    }   

    void UIButtonComponent::onRollout()
    {
        changeState(State_onRollout);
    }

    void UIButtonComponent::updateState(Vec3d &textPos, Vec2d &textScale, f32 &textAngle)
    {
        f32 targetScale = 1.0f;
        switch(m_state)
        {
        case State_Idle:
            {
            updateRelativePosFromTarget(getTargetPos());
                targetScale = updateMinActorScale();
            }
            break;
        case State_onPressed:
            updateRelativePosFromTarget(0.5f);
            if (m_animLightComponent)
            {
                if (m_animLightComponent->isSubAnimFinished())
                    changeState(State_Idle_Selected);
            }
            else
            {
                m_curScale = 1.0f + getTemplate()->getIdleSelectedScale() * f32_Sin(MTH_2PI * getTemplate()->getIdleSelectedPulseFrequency() * m_timer);
                textScale = Vec2d::One  * m_curScale;
                changeState(State_Idle_Selected);
            }
            break;
        case State_onReleased:
            updateRelativePosFromTarget(0.5f);
            if (m_animLightComponent)
            {
                if (m_animLightComponent->isSubAnimFinished())
                    changeState(State_Idle_Selected);
            }
            else
            {
                m_curScale = 1.0f + getTemplate()->getIdleSelectedScale() * f32_Sin(MTH_2PI * getTemplate()->getIdleSelectedPulseFrequency() * m_timer);
                textScale = Vec2d::One  * m_curScale;
                changeState(State_Idle_Selected);
            }
            break;
        case State_onRollout:   
            if (m_animLightComponent)
            {
                targetScale = updateMinActorScale();
                if (m_animLightComponent->isSubAnimFinished())
                    changeState(State_Idle);
            }
            else
            {
                changeState(State_Idle);
            }
            updateRelativePosFromTarget(getTargetPos());
            break;
        case State_onRollover:
            if (m_animLightComponent)
            {
                if (m_animLightComponent->isSubAnimFinished())
                    changeState(State_Idle_Selected);
            }
            else
            {
                changeState(State_Idle_Selected);
            }
            updateRelativePosFromTarget(0.5f);
            break;
        case State_Idle_Selected:
            if (!m_animLightComponent)
            {
                m_curScale = 1.0f + getTemplate()->getIdleSelectedScale() * f32_Sin(MTH_2PI * getTemplate()->getIdleSelectedPulseFrequency() * m_timer);
                textScale = Vec2d::One  * m_curScale;
            }
            updateRelativePosFromTarget(0.5f);
            break;
        }
        m_curActorScale = m_curActorScale + (targetScale - m_curActorScale) * getTemplate()->getActorScaleSmoothFactor();
        m_actor->setScale(m_actor->getScale() * m_curActorScale);
        //textScale *= m_curActorScale;
    }

    f32 UIButtonComponent::getTargetPos()
    {
        Actor * actor = m_child1.getActor();
                    if (actor)
                    {
                        UIButtonComponent * buttonComponent = actor->GetComponent<UIButtonComponent>();
                        if (buttonComponent)
                        {
                            if (buttonComponent->getState() != State_Idle && 
                                buttonComponent->getState() != State_onRollout)
                            {
                                return 0.5f + m_offset1;
                            }
                        }
                    }
        actor = m_child2.getActor();
                    if (actor)
                    {
                        UIButtonComponent * buttonComponent = actor->GetComponent<UIButtonComponent>();
                        if (buttonComponent)
                        {
                            if (buttonComponent->getState() != State_Idle && 
                                buttonComponent->getState() != State_onRollout)
                            {
                                return  0.5f + m_offset2;
                            }
                        }
                    }
        actor = m_child3.getActor();
                    if (actor)
                    {
                        UIButtonComponent * buttonComponent = actor->GetComponent<UIButtonComponent>();
                        if (buttonComponent)
                        {
                            if (buttonComponent->getState() != State_Idle && 
                                buttonComponent->getState() != State_onRollout)
                            {
                                return  0.5f + m_offset3;
                            }
                        }
                    }
        return m_initPos.m_y;
    }

    void UIButtonComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();
        if (m_textBox.isValid())
        {
            Actor * actor = m_textBox.getActor();
            if (actor)
                actor->requestDestruction();
            m_textBox.invalidate();
        }
    }

    void UIButtonComponent::updateTextFromAnim( Vec3d &textPos, Vec2d &textScale, f32 &textAngle )
    {
        m_animLightComponent->setColor(getActorColor());
        // Get bone info
        u32 boneIndex = U32_INVALID;
        boneIndex = m_animLightComponent->getBoneIndex("b_text01");

        if ( boneIndex != U32_INVALID )
        {
            Vec3d bonePosEnd    = Vec3d(1.0f,0.0f,0.0f);

            m_animLightComponent->getBonePos(boneIndex,textPos);
            m_animLightComponent->getBonePos(boneIndex,bonePosEnd,btrue);

            m_animLightComponent->getBoneScale(boneIndex,textScale);

            if ( m_actor->is2DActor() )
            {
                transform2d xf;

                xf.m_pos = m_actor->get2DPos();
                xf.m_rot.m_col1 = Vec2d::Right.Rotate(m_actor->getAngle());
                xf.m_rot.m_col2 = xf.m_rot.m_col1.getPerpendicular();

                textPos.m_y *= -1.f;
                textPos = mulTransform2d(xf,textPos.truncateTo2D()*m_actor->getScale()).to3d(textPos.m_z);
                bonePosEnd.m_y *= -1.f;
                bonePosEnd = mulTransform2d(xf,bonePosEnd.truncateTo2D()*m_actor->getScale()).to3d(bonePosEnd.m_z);
            }

            Vec3d boneDir = bonePosEnd - textPos;
            textAngle = Vec3d_getAngle(boneDir);
        }
    }

    void UIButtonComponent::updateRelativePosFromTarget(f32 _pos)
    {
        if (m_linkComponent)
        {
            f32 currentPos = m_relativePosY;
            f32 newPos = currentPos + (_pos - currentPos) * getTemplate()->getSpeed();
            m_relativePosY = newPos;
        }
    }

    f32 UIButtonComponent::updateMinActorScale(  )
    {
        if (m_isExtremity)
        {
            Actor * actor = m_child1.getActor();
            if (actor)
            {
                UIButtonComponent * buttonComponent = actor->GetComponent<UIButtonComponent>();
                if (buttonComponent)
                {
                    if (buttonComponent->getState() != State_Idle && 
                        buttonComponent->getState() != State_onRollout)
                    {
                        if (buttonComponent->getExtremity())
                            return getTemplate()->getMinActorScale();
                    }
                }
            }
            actor = m_child2.getActor();
            if (actor)
            {
                UIButtonComponent * buttonComponent = actor->GetComponent<UIButtonComponent>();
                if (buttonComponent)
                {
                    if (buttonComponent->getState() != State_Idle && 
                        buttonComponent->getState() != State_onRollout)
                    {
                        if (buttonComponent->getExtremity())
                            return getTemplate()->getMinActorScale();
                    }
                }
            }

        }
        return 1.0f;
    }

    void UIButtonComponent::onBecomeActive()
    {
        Super::onBecomeActive();
        if (this->getIsSelected())
            changeState(State_Idle_Selected);
        else
            changeState(State_Idle);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UIButtonComponent_Template)
    BEGIN_SERIALIZATION_CHILD(UIButtonComponent_Template)
        SERIALIZE_MEMBER("speed", m_speed);
        SERIALIZE_MEMBER("path", m_textPath);
        SERIALIZE_MEMBER("idleSelectedScale", m_idleSelectedScale);
        SERIALIZE_MEMBER("idleSelectedPulseFrequency", m_idleSelectedPulseFrequency);
        SERIALIZE_MEMBER("is2dActor", m_is2dActor);
        SERIALIZE_MEMBER("actorScaleSmoothFactor", m_actorScaleSmoothFactor);
        SERIALIZE_MEMBER("minActorScale", m_minActorScale);
    END_SERIALIZATION()

    UIButtonComponent_Template::UIButtonComponent_Template() 
    : m_speed(0.1f)
    , m_idleSelectedScale(1.5f)
    , m_idleSelectedPulseFrequency(2.0f)
    , m_is2dActor(bfalse)
    , m_actorScaleSmoothFactor(1.0f)
    , m_minActorScale(0.5f)
    {

    }

}
