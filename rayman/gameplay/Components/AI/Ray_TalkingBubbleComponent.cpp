#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_TALKINGBUBBLECOMPONENT_H_
#include "rayman/gameplay/Components/ai/Ray_TalkingBubbleComponent.h"
#endif //_ITF_RAY_TALKINGBUBBLECOMPONENT_H_

#ifndef _ITF_RAY_ANIMATIONMARKERS_H_
#include "rayman/gameplay/Ray_AnimationMarkers.h"
#endif //_ITF_RAY_ANIMATIONMARKERS_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_FONTRESOURCE_H_
#include "engine/display/FontResource.h"
#endif //_ITF_FONTRESOURCE_H_

#ifndef _ITF_FONT_H_
#include "engine/display/font.h"
#endif //_ITF_FONT_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

namespace ITF
{

IMPLEMENT_OBJECT_RTTI(Ray_TalkingBubbleComponent)
BEGIN_SERIALIZATION_CHILD(Ray_TalkingBubbleComponent)

END_SERIALIZATION()

Ray_TalkingBubbleComponent::Ray_TalkingBubbleComponent()
: m_animatedComponent(NULL)
, m_sentenceIndex(0)
, m_textBoneIndex(U32_INVALID)
, m_state(STATE_HIDDEN)
, m_currentScaleX(0.f)
, m_currentScaleY(0.f)
, m_targetScaleX(0.f)
, m_targetScaleY(0.f)
, m_scaleXSpeed(0.f)
, m_scaleYSpeed(0.f)
, m_boneHeight(0.f)
, m_shake(bfalse)
, m_showSentence(bfalse)
, m_textVisibleMarker(bfalse)
, m_receivedHide(bfalse)
, m_receivedShow(bfalse)
, m_done(bfalse)
, m_firstLineHasPlayed(bfalse)
{
}

Ray_TalkingBubbleComponent::~Ray_TalkingBubbleComponent()
{
}

void Ray_TalkingBubbleComponent::onActorClearComponents()
{
    clearActors();
}

void Ray_TalkingBubbleComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    u32 numSentences = m_sentences.size();

    for ( u32 i = 0; i < numSentences; i++ )
    {
        SentenceInfo& sentenceInfo = m_sentences[i];

        Actor* textActor = sentenceInfo.m_textActorRef.getActor();

        if ( textActor )
        {
            textActor->disable();
        }
    }
}

void Ray_TalkingBubbleComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

    if (m_animatedComponent)
    {
        m_animatedComponent->enableDraw(bfalse);
        m_animatedComponent->setUpdateAnimInput(this);
    }

    if (!getTemplate()->getTextPath().isEmpty())
    {
        SPAWNER->declareNeedsSpawnee(m_actor, &m_spawner,getTemplate()->getTextPath());
    }

    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301),this);
    ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374),this);
}

void Ray_TalkingBubbleComponent::onResourceReady()
{
    Super::onResourceReady();

    if ( getTemplate()->getTextBone().isValid() )
    {
        m_textBoneIndex = m_animatedComponent->getBoneIndex(getTemplate()->getTextBone());
    }
}

void Ray_TalkingBubbleComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_receivedShow )
    {
        m_receivedShow = bfalse;
        m_animatedComponent->enableDraw(btrue);
        if (getTemplate()->getResetOnAppear())
        {
            setCurrentSentence(0);
            m_firstLineHasPlayed = bfalse;
        }
        m_state = STATE_IDLE;
    }
    
    if ( m_receivedHide )
    {
        m_state = STATE_HIDDEN;
        m_textVisibleMarker = bfalse;
        m_showSentence = bfalse;
        m_receivedHide = bfalse;
    }
    else
    {
        if ( !m_animatedComponent->isInTransition() )
        {
            if ( m_state == STATE_HIDDEN )
            {
                m_animatedComponent->enableDraw(bfalse);
                return;
            }
        }
    }

    if ( m_showSentence )
    {
        if ( fabs(m_targetScaleX-m_currentScaleX) < 0.01f && fabs(m_targetScaleY-m_currentScaleY) < 0.01f )
        {
            m_showSentence = bfalse;
        }
    }
    else
    {
        if (getTemplate()->getAutoPlayOnEndDialog() && m_firstLineHasPlayed)
        {
            if (!isDialogPlaying())
            {
                showNextSentence();
            }
        }
    }

    updateSentences();

    if ( m_state != STATE_HIDDEN )
    {
        updateCameraZoom();
    }

    updateScale(_dt,m_targetScaleX,m_currentScaleX,m_scaleXSpeed);
    updateScale(_dt,m_targetScaleY,m_currentScaleY,m_scaleYSpeed);


}

void Ray_TalkingBubbleComponent::setSentenceCleanup()
{
    clearActors();
    m_sentences.clear();
    m_sentencesStr.clear();
    setCurrentSentence(0);
}

void Ray_TalkingBubbleComponent::setSentenceFinish()
{
    if ( m_sentences.size() > 0 )
    {
        setCurrentSentence(m_sentenceIndex);
        m_currentScaleX = m_targetScaleX;
        m_currentScaleY = m_targetScaleY;
        m_scaleXSpeed = 0.f;
        m_scaleYSpeed = 0.f;
    }
}

void Ray_TalkingBubbleComponent::setCurrentSentence( u32 _sentenceIndex )
{
    m_sentenceIndex = _sentenceIndex;

    if ( m_sentenceIndex < m_sentences.size() )
    {
        f32 scale = m_sentences[m_sentenceIndex].m_scale;

        m_targetScaleX = scale;
        m_targetScaleY = scale;
    }
}

void Ray_TalkingBubbleComponent::setSentencePrepareTextSize( UITextBox* _textComponent, SentenceInfo& _sentenceInfo ) const
{
    f32 minChars = static_cast<f32>(getTemplate()->getCharsMinSize());
    f32 maxChars = static_cast<f32>(getTemplate()->getCharsMaxSize());
    f32 numChars = Clamp(static_cast<f32>(_sentenceInfo.m_numCharacters),minChars,maxChars);
    f32 t = ( numChars - minChars ) / ( maxChars - minChars );
    f32 width = Interpolate(getTemplate()->getWidthMinSize(),getTemplate()->getWidthMaxSize(),t);
    f32 height = Interpolate(getTemplate()->getHeightMinSize(),getTemplate()->getHeightMaxSize(),t);

    _textComponent->setBoxWidth(width);
    _textComponent->setBoxHeight(height);

    _sentenceInfo.m_scale = t;
}

void Ray_TalkingBubbleComponent::setSentences( const SafeArray <LocalisationId>& _list )
{
    setSentenceCleanup();

    u32 numSentences = _list.size();
    
    for ( u32 i = 0; i < numSentences; i++ )
    {
        Actor* bubbleText = m_spawner.getSpawnee(m_actor->getScene(),getTextPos(),m_actor->getAngle());

        if ( bubbleText )
        {
            SentenceInfo sentenceInfo;

            sentenceInfo.m_textActorRef = bubbleText->getRef();
            sentenceInfo.m_numCharacters = UITextBox::calculateNumOfCharactersFromLoc(_list[i]);
            sentenceInfo.m_scale = 0.f;

            UITextBox* textComponent = bubbleText->GetComponent<UITextBox>();

            if ( textComponent )
            {
                setSentencePrepareTextSize(textComponent,sentenceInfo);

                textComponent->addTextFromLineId(_list[i],getTemplate()->getTextColor());
            }

            bubbleText->disable();

            m_sentences.push_back(sentenceInfo);

            m_actor->getBinding()->bindChild(bubbleText->getRef());
        }
    }

    setSentenceFinish();
}

void Ray_TalkingBubbleComponent::setSentencesStr( const ITF_VECTOR <TextInfo>& _list )
{
    setSentenceCleanup();

    u32 numSentences = _list.size();

    for ( u32 i = 0; i < numSentences; i++ )
    {
        Actor* bubbleText = m_spawner.getSpawnee(m_actor->getScene(),getTextPos(),m_actor->getAngle());

        if ( bubbleText )
        {
            SentenceInfo sentenceInfo;

            sentenceInfo.m_textActorRef = bubbleText->getRef();
            sentenceInfo.m_numCharacters = UITextBox::calculateNumOfCharactersFromStr(_list[i].m_text.cStr());
            sentenceInfo.m_scale = 0.f;

            UITextBox* textComponent = bubbleText->GetComponent<UITextBox>();

            if ( textComponent )
            {
                setSentencePrepareTextSize(textComponent,sentenceInfo);

                textComponent->addTextFromStr(_list[i]);
            }

            bubbleText->disable();

            m_sentences.push_back(sentenceInfo);
        }
    }

    setSentenceFinish();
}

void Ray_TalkingBubbleComponent::clearActors()
{
    u32 numSentences = m_sentences.size();

    for ( u32 i = 0; i < numSentences; i++ )
    {
        Actor* bubble = m_sentences[i].m_textActorRef.getActor();

        if ( bubble )
        {
            m_actor->getBinding()->unbindChild(bubble->getRef());
            bubble->requestDestruction();
        }
    }

    m_sentences.clear();
}

Vec3d Ray_TalkingBubbleComponent::getTextPos() const
{
    Vec3d retPos = m_actor->getPos();

    if ( m_textBoneIndex != U32_INVALID )
    {
        Vec3d bonePos;

        m_animatedComponent->getBonePos(m_textBoneIndex,retPos);
    }

    retPos.m_z = m_actor->getPos().m_z + getTemplate()->getTextDepth();

    return retPos;
}

void Ray_TalkingBubbleComponent::onEvent( Event * _event)
{
    Super::onEvent(_event);

    if ( AnimGameplayEvent* animEvent = _event->DynamicCast<AnimGameplayEvent>(ITF_GET_STRINGID_CRC(AnimGameplayEvent,2720277301)) )
    {
        if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Bounce_Finish,1890149707) )
        {
            m_shake = bfalse;
        }
        else if ( animEvent->getName() == ITF_GET_STRINGID_CRC(MRK_Show_Text,3496325768) )
        {
            if ( !m_textVisibleMarker && m_state == STATE_IDLE )
            {
                m_textVisibleMarker = btrue;
            }
        }
    }
    else if ( EventScaleChanged* scaleChanged = _event->DynamicCast<EventScaleChanged>(ITF_GET_STRINGID_CRC(EventScaleChanged,3630354374)) )
    {
        f32 t = m_actor->getScale().m_x / m_actor->getWorldInitialScale().m_x;
        u32 numSentences = m_sentences.size();

        for ( u32 i = 0; i < numSentences; i++ )
        {
            Actor* actor = m_sentences[i].m_textActorRef.getActor();
            Vec2d scale = actor->getWorldInitialScale()*t;
            actor->setScale(scale);
        }
    }
}

void Ray_TalkingBubbleComponent::setAppear()
{
    if ( m_state == STATE_HIDDEN )
    {
        m_receivedShow = btrue;
    }
}

void Ray_TalkingBubbleComponent::setDisappear()
{
    if ( m_state != STATE_HIDDEN )
    {
        m_receivedHide = btrue;
    }
}

void Ray_TalkingBubbleComponent::showNextSentence()
{
    u32 numSentences = m_sentences.size();

    if (getTemplate()->getAutoPlayOnEndDialog() && m_done)
        return;

    if ( numSentences > 1 )
    {
        if ( m_sentenceIndex < numSentences-1 )
        {
            setCurrentSentence(m_sentenceIndex+1);
        }
        else
        {
            if (getTemplate()->getAutoPlayOnEndDialog())
            {
                m_done = btrue;
            }
            else
            {
                setCurrentSentence(0);
            }
        }

        m_showSentence = btrue;
        m_shake = btrue;
    }
    else
    {
        if (getTemplate()->getAutoPlayOnEndDialog())
            m_done = btrue;
    }
}

void Ray_TalkingBubbleComponent::showSentence(u32 _index)
{
    u32 numSentences = m_sentences.size();

    if ( numSentences > 1  && _index < m_sentences.size())
    {
        setCurrentSentence(_index);

        m_showSentence = btrue;
        m_shake = btrue;
    }
}

void Ray_TalkingBubbleComponent::updateSentences()
{
    Vec3d pos = getTextPos();
    u32 numSentences = m_sentences.size();
    bbool showSentences = !m_animatedComponent->isInTransition() && m_state != STATE_HIDDEN;

    for ( u32 i = 0; i < numSentences; i++ )
    {
        SentenceInfo& sentenceInfo = m_sentences[i];

        Actor* textActor = sentenceInfo.m_textActorRef.getActor();

        if ( textActor )
        {
            bbool active = showSentences && i == m_sentenceIndex;

            if ( active )
            {
                if ( !textActor->isEnabled() )
                {
                    textActor->enable();
                    
                    UITextBox* textBox = textActor->GetComponent<UITextBox>();

                    if ( textBox )
                    {
                        textBox->setLaunchAudio();
                    }
                }

                m_firstLineHasPlayed = btrue;
            }
            else
            {
                if ( textActor->isEnabled() )
                {
                    textActor->disable();
                }

                continue;
            }

            Vec3d textPos = pos;

            if ( textActor->getTemplate()->isZForced() )
            {
                f32 sceneZ = AIUtils::getSceneZFromPickable(textActor);

                textPos.m_z = sceneZ + textActor->getTemplate()->getForcedZ() + getTemplate()->getTextDepth();
            }
            else
            {
                textPos.m_z = m_actor->getPos().m_z + getTemplate()->getTextDepth();
            }

            textActor->setPos(textPos);
        }
    }
}

void Ray_TalkingBubbleComponent::updateCameraZoom()
{
    f32 defaultDepth = CAMERACONTROLLERMANAGER->getDefaultDepth();
    f32 currentDepth = CAMERACONTROLLERMANAGER->getDepth();
    f32 ratio = currentDepth/defaultDepth;
    f32 maxRatio = Max(1.f,getTemplate()->getMaxRatio());

    ratio = Clamp(ratio,1.f,maxRatio);

    f32 t = (ratio-1.f) / (maxRatio-1.f);

    f32 scaleValue = Interpolate(1.f,getTemplate()->getMaxZoom(),t);

    Vec2d initialScale = m_actor->getWorldInitialScale();

    m_actor->setScale(initialScale*scaleValue);
}

void Ray_TalkingBubbleComponent::updateScale( f32 _dt, f32 _targetScale, f32& _currentScale, f32& _currentSpeed ) const
{
    f32 dif = _targetScale - _currentScale;
    f32 f = (dif*getTemplate()->getScaleK()) - (_currentSpeed*getTemplate()->getScaleD());
    
    _currentSpeed += f*_dt;
    _currentScale += _currentSpeed*_dt;
}

void Ray_TalkingBubbleComponent::updateAnimInput()
{
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(bubbleState,524321424),m_state);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(scaleX,1292250801),m_currentScaleX);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(scaleY,3650005007),m_currentScaleY);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(shake,3500416584),m_shake);
    m_animatedComponent->setInput(ITF_GET_STRINGID_CRC(boneHeight,2751424114),m_boneHeight);
}

bbool Ray_TalkingBubbleComponent::isDialogPlaying() const
{
    ITF_ASSERT(m_sentenceIndex < m_sentences.size());

    if ( m_sentenceIndex >= m_sentences.size() )
    {
        return bfalse;
    }

    const SentenceInfo& info = m_sentences[m_sentenceIndex];

    const Actor* actor = info.m_textActorRef.getActor();

    if ( actor )
    {
        const UITextBox* textComponent = actor->GetComponent<UITextBox>();

        if ( textComponent )
        {
            return textComponent->isDialogPlaying() || !textComponent->isTextUpdated();
        }
    }

    return bfalse;
}

ITF::bbool Ray_TalkingBubbleComponent::isDoneTalking() const
{
    return m_done && m_textVisibleMarker &&!isDialogPlaying();
}

///////////////////////////////////////////////////////////////////////////////////////////
IMPLEMENT_OBJECT_RTTI(Ray_TalkingBubbleComponent_Template)
BEGIN_SERIALIZATION(Ray_TalkingBubbleComponent_Template)

    SERIALIZE_MEMBER("textBone",m_textBone);
    SERIALIZE_MEMBER("maxZoom",m_maxZoom);
    SERIALIZE_MEMBER("maxRatio",m_maxRatio);
    SERIALIZE_MEMBER("scaleK",m_scaleK);
    SERIALIZE_MEMBER("scaleD",m_scaleD);
    SERIALIZE_MEMBER("textDepth",m_textDepth);
    SERIALIZE_MEMBER("charsMinSize",m_charsMinSize);
    SERIALIZE_MEMBER("charsMaxSize",m_charsMaxSize);
    SERIALIZE_MEMBER("widthMinSize",m_widthMinSize);
    SERIALIZE_MEMBER("widthMaxSize",m_widthMaxSize);
    SERIALIZE_MEMBER("heightMinSize",m_heightMinSize);
    SERIALIZE_MEMBER("heightMaxSize",m_heightMaxSize);
    SERIALIZE_MEMBER("textActor",m_textPath);
    SERIALIZE_MEMBER("textColor",m_textColor);
    SERIALIZE_MEMBER("resetOnAppear",m_resetOnAppear);
    SERIALIZE_MEMBER("autoPlayOnEndDialog",m_autoPlayOnEndDialog);

END_SERIALIZATION()

Ray_TalkingBubbleComponent_Template::Ray_TalkingBubbleComponent_Template()
: m_maxZoom(2.f)
, m_maxRatio(1.5f)
, m_scaleK(10.f)
, m_scaleD(2.f)
, m_textDepth(0.05f)
, m_charsMinSize(20)
, m_charsMaxSize(100)
, m_widthMinSize(5.f)
, m_widthMaxSize(10.f)
, m_heightMinSize(5.f)
, m_heightMaxSize(10.f)
, m_textColor(Color::white())
, m_resetOnAppear(btrue)
, m_autoPlayOnEndDialog(bfalse)
{
}

Ray_TalkingBubbleComponent_Template::~Ray_TalkingBubbleComponent_Template()
{
}

}
