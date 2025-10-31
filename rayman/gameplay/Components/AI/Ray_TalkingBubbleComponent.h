
#ifndef _ITF_RAY_TALKINGBUBBLECOMPONENT_H_
#define _ITF_RAY_TALKINGBUBBLECOMPONENT_H_

#ifndef _ITF_ACTORCOMPONENT_H_
#include "engine/actors/actorcomponent.h"
#endif //_ITF_ACTORCOMPONENT_H_

#ifndef _ITF_SPAWNER_H_
#include "engine/actors/managers/Spawner.h"
#endif


namespace ITF
{

class Ray_TalkingBubbleComponent : public ActorComponent, IUpdateAnimInputCriteria
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TalkingBubbleComponent,ActorComponent,2023492281)
    DECLARE_SERIALIZE()

    enum State
    {
        STATE_HIDDEN = 0,
        STATE_IDLE = 1,
    };

public:

    Ray_TalkingBubbleComponent();
    virtual ~Ray_TalkingBubbleComponent();

    virtual bbool                       needsUpdate() const { return btrue; }
    virtual bbool                       needsDraw() const { return bfalse; }
    virtual bbool                       needsDraw2D() const { return bfalse; }

    virtual void                        onActorClearComponents();
    virtual void                        onActorLoaded(Pickable::HotReloadType /*_hotReload*/);
    virtual void                        onBecomeInactive();
    virtual void                        onResourceReady();
    virtual void                        Update( f32 _dt );
    virtual void                        onEvent( Event* _event );
    virtual void                        updateAnimInput();

    bbool                               isHidden() const { return m_state == STATE_HIDDEN; }
    bbool                               isDialogPlaying() const;
    bbool                               isDoneTalking() const;

    void                                setSentences( const SafeArray<LocalisationId>& _list );
    void                                setSentencesStr( const ITF_VECTOR <TextInfo>& _list );
    void                                setAppear();
    void                                setDisappear();
    void                                showNextSentence();
    void                                showSentence(u32 _index);

    void                                setBubbleHeight( f32 _height ) { m_boneHeight = _height; }

protected:

    struct SentenceInfo
    {
        ActorRef                    m_textActorRef;
        u32                         m_numCharacters;
        f32                         m_scale;
    };

    typedef ITF_VECTOR <SentenceInfo> SentenceList;

    const class Ray_TalkingBubbleComponent_Template* getTemplate() const;

    void                                updateSentences();
    void                                updateCameraZoom();
    void                                parseAndConvertText( SentenceInfo& _sentenceInfo );
    void                                updateScale( f32 _dt, f32 _targetScale, f32& _currentScale, f32& _currentSpeed ) const;
    
    Vec3d                               getTextPos() const;

    void                                clearActors();

    void                                setSentenceCleanup();
    void                                setSentenceFinish();
    void                                setSentencePrepareTextSize( class UITextBox* _textComponent, SentenceInfo& _sentenceInfo ) const;

    void                                setCurrentSentence( u32 _sentenceIndex );

    class AnimatedComponent*            m_animatedComponent;
    u32                                 m_sentenceIndex;
    u32                                 m_textBoneIndex;
    State                               m_state;
    SentenceList                        m_sentences;
    ITF_VECTOR <TextInfo>               m_sentencesStr;
    f32                                 m_currentScaleX;
    f32                                 m_currentScaleY;
    f32                                 m_targetScaleX;
    f32                                 m_targetScaleY;
    f32                                 m_scaleXSpeed;
    f32                                 m_scaleYSpeed;
    f32                                 m_boneHeight;
    bbool                               m_shake;
    bbool                               m_showSentence;
    bbool                               m_textVisibleMarker;
    bbool                               m_receivedHide;
    bbool                               m_receivedShow;
    bbool                               m_done;
    bbool                               m_firstLineHasPlayed;

    SpawneeGenerator                    m_spawner;
};

class Ray_TalkingBubbleComponent_Template : public TemplateActorComponent
{
    DECLARE_OBJECT_CHILD_RTTI(Ray_TalkingBubbleComponent_Template,TemplateActorComponent,3566362660);
    DECLARE_SERIALIZE();
    DECLARE_ACTORCOMPONENT_TEMPLATE(Ray_TalkingBubbleComponent);

public:

    Ray_TalkingBubbleComponent_Template();
    ~Ray_TalkingBubbleComponent_Template();

    
    const StringID&                         getTextBone() const { return m_textBone; }
    f32                                     getMaxZoom() const { return m_maxZoom; }
    f32                                     getMaxRatio() const { return m_maxRatio; }
    f32                                     getScaleK() const { return m_scaleK; }
    f32                                     getScaleD() const { return m_scaleD; }
    f32                                     getTextDepth() const { return m_textDepth; }
    u32                                     getCharsMinSize() const { return m_charsMinSize; }
    u32                                     getCharsMaxSize() const { return m_charsMaxSize; }
    f32                                     getWidthMinSize() const { return m_widthMinSize; }
    f32                                     getWidthMaxSize() const { return m_widthMaxSize; }
    f32                                     getHeightMinSize() const { return m_heightMinSize; }
    f32                                     getHeightMaxSize() const { return m_heightMaxSize; }
    const Path&                             getTextPath() const { return m_textPath; }
    const Color&                            getTextColor() const { return m_textColor; }
    bbool                                   getResetOnAppear() const { return m_resetOnAppear; }
    bbool                                   getAutoPlayOnEndDialog() const { return m_autoPlayOnEndDialog; }

private:

    StringID                                m_textBone;
    f32                                     m_maxZoom;
    f32                                     m_maxRatio;
    f32                                     m_scaleK;
    f32                                     m_scaleD;
    f32                                     m_textDepth;
    u32                                     m_charsMinSize;
    u32                                     m_charsMaxSize;
    f32                                     m_widthMinSize;
    f32                                     m_widthMaxSize;
    f32                                     m_heightMinSize;
    f32                                     m_heightMaxSize;
    Path                                    m_textPath;
    Color                                   m_textColor;
    bbool                                   m_resetOnAppear;
    bbool                                   m_autoPlayOnEndDialog;
};

ITF_INLINE const class Ray_TalkingBubbleComponent_Template* Ray_TalkingBubbleComponent::getTemplate() const
{
    return static_cast<const class Ray_TalkingBubbleComponent_Template*>(m_template);
}

}

#endif // _ITF_RAY_TALKINGBUBBLECOMPONENT_H_

