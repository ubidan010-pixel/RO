#include "precompiled_gameplay.h"

#ifndef _ITF_UITEXTBOXESCOMPONENT_H_
#include "gameplay/components/UI/UITextBoxesComponent.h"
#endif //_ITF_UITEXTBOXESCOMPONENT_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_TRANSFORM_H_
#include "core/math/transform.h"
#endif //_ITF_TRANSFORM_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

namespace ITF
{
    BEGIN_SERIALIZATION(UITextField)
        SERIALIZE_MEMBER("textActorPath",m_textActorPath);
        SERIALIZE_MEMBER("fontHeight",m_fontHeight);
        SERIALIZE_MEMBER("color",m_color);
        SERIALIZE_MEMBER("boneName",m_boneName);
        SERIALIZE_MEMBER("lineId",m_lineId);
        SERIALIZE_MEMBER("depthRank",m_depthRank);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(UITextBoxesComponent_Template)

    BEGIN_SERIALIZATION(UITextBoxesComponent_Template)
        SERIALIZE_MEMBER("isDraw2d",m_isDraw2d);
        SERIALIZE_MEMBER("useBoneAngle",m_usBoneAngle);
        SERIALIZE_MEMBER("useBoneScale",m_useBoneScale);
        SERIALIZE_MEMBER("useScreenRatio",m_useScreenRatio);
        SERIALIZE_MEMBER("disableActiveSync",m_disableActiveSync);
        SERIALIZE_CONTAINER_OBJECT("textFields",m_textFields);
    END_SERIALIZATION()

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(UITextBoxesComponent)

    BEGIN_SERIALIZATION_CHILD(UITextBoxesComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(UITextBoxesComponent)
        VALIDATE_COMPONENT_PARAM("", m_animLightComponent, "AnimLightComponent mandatory");
    END_VALIDATE_COMPONENT()

    UITextBoxesComponent::UITextBoxesComponent()
        : m_animLightComponent(NULL)
        , m_enableRender(btrue)
    {
    }

    UITextBoxesComponent::~UITextBoxesComponent()
    {
        for ( u32 i = 0 ; i < m_textFields.size() ; ++i )
        {
            if( m_textFields[i].m_textRef.isValid() )
            {
                m_actor->getBinding()->unbindChild(m_textFields[i].m_textRef);
                //Actor * textBoxAct = m_textFields[i].m_textActor.getActor();
                //if ( textBoxAct )
                //    textBoxAct->requestDestruction();
            }
        }
        m_textFields.clear();
    }

    void UITextBoxesComponent::prepareSpawneeGenerators()
    {
        const ITF_VECTOR<UITextField>& textFieldsTemplate = getTemplate()->getTextFields();

        for ( u32 i = 0 ; i < textFieldsTemplate.size() ; ++i )
        {
            UITextField pNewTextfield = textFieldsTemplate[i];

            if ( !pNewTextfield.m_textActorPath.isEmpty() )
            {
                m_spawneeGenerators[pNewTextfield.m_textActorPath.getStringID()] = SpawneeGenerator();
            }
        }
    }

    void UITextBoxesComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        // register to EventShow
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventShow,111532112),this);

        // Get animation component
        m_animLightComponent = m_actor->GetComponent<AnimLightComponent>();

        // Allocate spawnee generator map to have them all ready and to avoid reallocating during spawning
        // because it will destroy previously created actors
        prepareSpawneeGenerators();

        // Fill in array of text fields based on templates
        const ITF_VECTOR<UITextField>& textFieldsTemplate = getTemplate()->getTextFields();
        m_textFields.reserve(textFieldsTemplate.size());

        for ( u32 i = 0 ; i < textFieldsTemplate.size() ; ++i )
        {
            m_textFields.push_back(textFieldsTemplate[i]);
            UITextField& pNewTextfield = m_textFields.back();


            ITF_WARNING_CATEGORY(GPP,NULL,pNewTextfield.m_boneName != StringID::Invalid,"Need a bone name to work");
            ITF_WARNING_CATEGORY(GPP,NULL,!pNewTextfield.m_textActorPath.isEmpty(),"A text actor must be specified to work properly");
            if ( pNewTextfield.m_boneName != StringID::Invalid && !pNewTextfield.m_textActorPath.isEmpty() )
            {
                const StringID& pathID = pNewTextfield.m_textActorPath.getStringID();
                ITF_MAP<StringID, SpawneeGenerator>::iterator spawnee = m_spawneeGenerators.find(pathID);

                ITF_WARNING_CATEGORY(GPP,NULL,spawnee != m_spawneeGenerators.end(),"Not able to get a spawnee in the spawnee map");
                if ( spawnee == m_spawneeGenerators.end() )
                    continue;

                SPAWNER->declareNeedsSpawnee(m_actor, &(spawnee->second), pNewTextfield.m_textActorPath);

                Actor * textAct = spawnee->second.getSpawnee( m_actor->getScene(),m_actor->getPos());
                if ( !textAct )
                    continue;

                pNewTextfield.m_textRef = textAct->getRef();
                m_actor->getBinding()->bindChild(pNewTextfield.m_textRef);

                UITextBox* uiTextBox = textAct->GetComponent<UITextBox>();

                if ( !uiTextBox )
                    continue;

                pNewTextfield.m_textInfo.m_color = pNewTextfield.m_color.getAsU32();
                pNewTextfield.m_textInfo.m_size = pNewTextfield.m_fontHeight;
                pNewTextfield.m_textInfo.m_lineId = pNewTextfield.m_lineId;
                pNewTextfield.m_textInfo.m_text.setText("");

                // store newly created textfield
                m_textFieldsMap[pNewTextfield.m_boneName] = i;

                uiTextBox->addTextFromLineId(pNewTextfield.m_lineId,Color::black());
                
                if ( pNewTextfield.m_depthRank != -1 )
                {
                    uiTextBox->setDepthRank(pNewTextfield.m_depthRank);
                }
                else
                {
                    uiTextBox->setDepthRank(UI_TEXTMANAGER->getTextsDepthRank());
                }

                textAct->disable();
            }
        }
    }

    void UITextBoxesComponent::onBecomeActive()
    {
        Super::onBecomeActive();

        // NB[LaurentCou]: disable this for world map spots, we can have active
        // text boxes with hidden text
        if (!getTemplate()->getDisableActiveSync())
        {
            u32 sizeCount = m_textFields.size();
            for ( u32 i = 0 ; i < sizeCount ; ++i )
            {
                if ( m_textFields[i].m_textRef.isValid() )
                {
                    // get actor
                    Actor * textAct = m_textFields[i].m_textRef.getActor();
                    if ( !textAct )
                        continue;

                    textAct->enable();

                    if ( !m_actor->getBinding()->isDescendant(textAct->getRef()) )
                    {
                        m_actor->getBinding()->bindChild(textAct->getRef());
                    }
                }
            }
        }
    }

    void UITextBoxesComponent::onBecomeInactive()
    {
        Super::onBecomeInactive();

        // NB[LaurentCou]: disable this for world map spots, we can have active
        // text boxes with hidden text
        if (!getTemplate()->getDisableActiveSync())
        {
            u32 sizeCount = m_textFields.size();
            for ( u32 i = 0 ; i < sizeCount ; ++i )
            {
                if ( m_textFields[i].m_textRef.isValid() )
                {
                    // get actor
                    Actor * textAct = m_textFields[i].m_textRef.getActor();
                    if ( !textAct )
                        continue;

                    textAct->disable();
                }
            }
        }
    }

    void UITextBoxesComponent::Update( f32 _dt )
    {
        if ( !m_enableRender || !m_actor->isEnabled() )
        {
            return;
        }

        u32 sizeCount = m_textFields.size();
        for ( u32 i = 0 ; i < sizeCount ; ++i )
        {
            UITextField& textField = m_textFields[i];

            if ( textField.m_textRef.isValid() )
            {
                // get actor
                Actor * textAct = textField.m_textRef.getActor();

                if ( !textAct )
                    continue;

                if ( textField.m_lineId == LocalisationId::Invalid && textField.m_textInfo.m_lineId == LocalisationId::Invalid &&
                     textField.m_textInfo.m_text.isEmpty() )
                {
                    if ( textAct->isEnabled() )
                    {
                        textAct->disable();
                    }

                    continue;
                }

                if ( !textAct->isEnabled() )
                {
                    textAct->enable();
                }

                f32 textAngle = GetActor()->getAngle();
                Vec3d textPos = GetActor()->getPos();
                Vec2d textScale = textAct->getWorldInitialScale();
                u32 boneIndex = U32_INVALID;

                if ( m_animLightComponent )
                {
                    // Get bone info
                    boneIndex = m_animLightComponent->getBoneIndex(textField.m_boneName);

                    if ( boneIndex != U32_INVALID )
                    {
                        Vec3d bonePosEnd    = Vec3d(1.0f,0.0f,0.0f);

                        m_animLightComponent->getBonePos(boneIndex,textPos);
                        m_animLightComponent->getBonePos(boneIndex,bonePosEnd,btrue);

                        // NB[LaurentCou]: if this happens, spawned actors are
                        // moved on the origin, preventing unload of WM level
                        // selections as their bbox gets uber-large
                        ITF_ASSERT_MSG(textPos!=Vec3d::Zero, "Invalid bone position");
                        if (textPos==Vec3d::Zero)
                        {
                            textPos = GetActor()->getPos(); // hack
                        }

                        if ( getTemplate()->getUseBoneScale() )
                        {
                            m_animLightComponent->getBoneScale(boneIndex,textScale);
                        }

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

                // Apply this on actor
                textAct->setPos(textPos);

                if (getTemplate()->getUseBoneAngle())
                    textAct->setAngle(textAngle);
                
                if ( getTemplate()->getUseScreenRatio() )
                {
                    f32 screenWidth = static_cast<f32>(GFX_ADAPTER->getScreenWidth());
                    f32 screenHeight = static_cast<f32>(GFX_ADAPTER->getScreenHeight());
                    f32 ratioX = screenWidth/UI2D_WidthRef;
                    f32 ratioY = screenHeight/UI2D_HeightRef;

                    textScale.m_x *= ratioX;
                    textScale.m_y *= ratioY;
                }

                textAct->setScale(textScale);

                UITextBox* uiTextBox = textAct->GetComponent<UITextBox>();
                if ( uiTextBox != NULL )
                {
                    Color newColor = textField.m_color;
                    if ( m_animLightComponent && boneIndex != U32_INVALID )
                    {
                        AnimBoneDyn * bone = m_animLightComponent->getBone(boneIndex);
                        newColor.m_a = bone->m_Alpha;
                    }
                    textField.m_textInfo.m_color = newColor.getAsU32();
                    uiTextBox->setTextAt(textField.m_textInfo, 0);
                }
            }
        }
    }

    void UITextBoxesComponent::onEvent( Event * _event)
    {
        Super::onEvent(_event);

        // EventShow enables/disables text fields (no alpha fading available on text boxes...)
        if ( EventShow* eventShow = _event->DynamicCast<EventShow>(ITF_GET_STRINGID_CRC(EventShow,111532112)) )
        {
            enableRender(eventShow->getAlpha() != 0.0f);
        }
    }

    void UITextBoxesComponent::setText( const StringID& _textFieldName, const String8& _text)
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_textFieldsMap.find(_textFieldName);

        if ( it != m_textFieldsMap.end() )
        {
            m_textFields[it->second].m_textInfo.m_text.setText(_text);
        }
    }

    void UITextBoxesComponent::setLineId( const StringID& _textFieldName, const LocalisationId& _lineId)
    {
        ITF_MAP<StringID, u32>::const_iterator it = m_textFieldsMap.find(_textFieldName);

        if ( it != m_textFieldsMap.end() )
        {
            m_textFields[it->second].m_textInfo.m_lineId = _lineId;
        }
    }

    void UITextBoxesComponent::enableRender( bbool _val )
    {
        if ( m_enableRender != _val )
        {
            m_enableRender = _val;

            u32 numTextFields = m_textFields.size();

            for ( u32 i = 0; i < numTextFields; i++ )
            {
                UITextField& field = m_textFields[i];
                Actor* actor = field.m_textRef.getActor();

                if ( actor )
                {
                    if ( m_enableRender )
                    {
                        actor->enable();
                    }
                    else
                    {
                        actor->disable();
                    }
                }
            }
        }
    }
};
