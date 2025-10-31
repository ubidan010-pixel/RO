#include "precompiled_gameplay.h"

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_FILESERVER_H_
#include "core/file/FileServer.h"
#endif // _ITF_FILESERVER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_GAMEMANAGER_H_
#include "gameplay/Managers/GameManager.h"
#endif //_ITF_GAMEMANAGER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_DEBUGDRAW_H_
#include "engine/debug/DebugDraw.h"
#endif //_ITF_DEBUGDRAW_H_

#ifndef _ITF_SCENE_H_
#include "engine/scene/scene.h"
#endif //_ITF_SCENE_H_

#ifndef _ITF_MUSICMANAGER_H_
#include "engine/gameplay/musicmanager.h"
#endif //_ITF_MUSICMANAGER_H_

#ifndef _ITF_RESOURCEMANAGER_H_
#include "engine/resources/ResourceManager.h"
#endif //_ITF_RESOURCEMANAGER_H_

#ifndef _ITF_AIUTILS_H_
#include "gameplay/AI/Utils/AIUtils.h"
#endif //_ITF_AIUTILS_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_EVENTMANAGER_H_
#include "engine/events/EventManager.h"
#endif //_ITF_EVENTMANAGER_H_

#ifndef _ITF_EVENTS_H_
#include "engine/events/Events.h"
#endif //_ITF_EVENTS_H_

#include "engine/AdaptersInterfaces/AudioMiddlewareAdapter.h"

#define JAPANESE_SCALE 0.8f

namespace ITF
{
    ///////////////////////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(UITextBox)
    BEGIN_SERIALIZATION_CHILD(UITextBox)
        BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        END_CONDITION_BLOCK()
    END_SERIALIZATION()

    ///////////////////////////////////////////////////////////////////////////////////////////
    UITextBox::UITextBox()
    : Super()
    , m_UIref(ITF_INVALID_OBJREF)
    , m_fontIndex(U32_INVALID)
    , m_boxWidth(0.f)
    , m_boxHeight(0.f)
    , m_highestSize (0.f)
    , m_textBoxScale (1.f)
    , m_currentPage(0)
    , m_display(btrue)
    , m_depthRank(0)
    , m_sender(ITF_INVALID_OBJREF)
    , m_backgroundColour(COLOR_WHITE)
    , m_startUV(Vec2d::Zero)
    , m_endUV(Vec2d::One)
    , m_useBackground(bfalse)
    , m_boxPosition(BoxPosition_TopLeft)
    , m_mode(FONT_ALIGN_LEFT)
    , m_modeY(0)
    , m_needToComptutePosition(btrue)
    , m_textChanged(bfalse)
    , m_actorsReadyToDraw(btrue)
    , m_textScale(Vec3d::One)
    , m_textRotation(0.f)
    , m_textAlpha(1.0f)
    , m_shadowOffset(Vec2d(3.4f,3.4f))
    , m_shadowColor(COLOR_BLACK)
    , m_stopAudio(bfalse)
    , m_launchAudio(bfalse)
    , m_restart(btrue)
    , m_lineSpacingFactor(1.f)
    , m_noScaleWithActor(btrue)
	{
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    UITextBox::~UITextBox()
    {
        if (EVENTMANAGER)
        {
            EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(EventLanguageChanged,1029567175), this);
        }
    }

    void UITextBox::onActorClearComponents()
    {
        u32 numSpawnedActor = m_spawnedActors.size();

        for ( u32 i = 0; i < numSpawnedActor; i++ )
        {
            Actor* spawned = m_spawnedActors[i].m_ref.getActor();

            if ( spawned )
            {
                spawned->requestDestruction();
            }
        }

        m_spawnedActors.clear();

        stopAudio();
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventTrigger,1343042510),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventWaitForActor,866051242),this);
        ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699),this);

        m_textTabWithVarTags.clear();
        m_textTab.clear();
        m_blockData.clear();

        m_UIref = m_actor->getRef();

        const u32 BlocksFriendlySize=getTemplate()->getBlocksFriendly().size();
        m_blockData.reserve(BlocksFriendlySize);
        
        for (u32 index = 0;index<BlocksFriendlySize;++index)
        {
            BlockData data;
            data.m_friendly = getTemplate()->getBlocksFriendly()[index];
            m_blockData.push_back(data);
        }
        
        m_mode = getTemplate()->getTextMode();
        m_modeY = getTemplate()->getTextModeY();
        m_highestSize = LOCALISATIONMANAGER->getCurrentLanguage() == ITF_LANGUAGE_JAPANESE ? getTemplate()->getTextHeight() * JAPANESE_SCALE : getTemplate()->getTextHeight();
        m_boxPosition = BoxPosition(getTemplate()->getBoxPosition());

        // Load the font
        if (!getTemplate()->getFontName().isEmpty())
            m_fontIndex = GFX_ADAPTER->loadFont(getTemplate()->getFontName());

        fillTextTab();
        if( m_textTabWithVarTags.size() > 0 )
        {
        m_highestSize = UI_TEXTMANAGER->getHighestSize(m_textTabWithVarTags);
        }

        // Scale
        m_textBoxScale = UI_TEXTMANAGER->getTextBoxScale();

        const Vec2d& initTextureOffset2D =getTemplate()->getInitTextureOffset2D();
        const Vec2d& initAnchorOffset2D =getTemplate()->getInitAnchorOffset2D();

        m_textureOffset2D = Vec2d(initTextureOffset2D.m_x * m_textBoxScale, initTextureOffset2D.m_y * m_textBoxScale);
        m_anchorOffset2D = Vec2d(initAnchorOffset2D.m_x * m_textBoxScale, initAnchorOffset2D.m_y * m_textBoxScale);
        scaleBox();

        // Background
        if (!getTemplate()->getPath().isEmpty())
            m_backTexture = m_actor->addResource(Resource::ResourceType_Texture, getTemplate()->getPath());

        // Set Shadow params
        m_shadowColor = getTemplate()->getTextShadowColor().getAsU32();
        m_shadowOffset = getTemplate()->getShadowOffset();

        m_lineSpacingFactor = getTemplate()->getlineSpacingFactor();

        if (EVENTMANAGER)
        {
            EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(EventLanguageChanged,1029567175), this);
        }
 }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::onEvent( Event * _evt)
    {
        if (EventTrigger * onTrigger = _evt->DynamicCast<EventTrigger>(ITF_GET_STRINGID_CRC(EventTrigger,1343042510)))
        {
            if (onTrigger->getActivated())
            {
                show();
            }
            else
            {
                hide();               
            }
        }
        else if (EventWaitForActor * onWaitForActor = _evt->DynamicCast<EventWaitForActor>(ITF_GET_STRINGID_CRC(EventWaitForActor,866051242)))
        {
            m_sender = onWaitForActor->getSender();
            if (!m_display)
                show();
        }
        else if (EventSequenceActorActivate * onSequenceActivate = _evt->DynamicCast<EventSequenceActorActivate>(ITF_GET_STRINGID_CRC(EventSequenceActorActivate,1119571699)))
        {
            if (onSequenceActivate->getActivate())
            {
                show();
            }
            else
            {
                hide();
            }
        }
        else if (EventLanguageChanged* languageChanged = _evt->DynamicCast<EventLanguageChanged>(ITF_GET_STRINGID_CRC(EventLanguageChanged,1029567175)))
        {
            onLanguageChanged(languageChanged->getOldLanguage(), languageChanged->getNewLanguage());
        }
    }

    void UITextBox::show()
    {
        if (getTemplate()->getUsePages()) 
        {
            INPUT_ADAPTER->disableEnvironment(InputAdapter::EnvironmentEngine);
        }

        if ( m_actorsReadyToDraw )
        {
            showActors(btrue);
        }

        m_display = btrue;
    }

    void UITextBox::hide()
    {
        if (getTemplate()->getUsePages()) 
        {
            if (INPUT_ADAPTER) // otherwise crashes when closing the app
                INPUT_ADAPTER->enableEnvironment(InputAdapter::EnvironmentEngine);
        }

        showActors(bfalse);

        m_display = bfalse;
        m_stopAudio = btrue;
    }

    void UITextBox::onLanguageChanged(ITF_LANGUAGE _oldLanguage, ITF_LANGUAGE _newLanguage)
    {
        if (m_textTabWithVarTags.empty())
        {
            return;
        }

        const bbool wasJapanese = (_oldLanguage == ITF_LANGUAGE_JAPANESE);
        const bbool isJapanese = (_newLanguage == ITF_LANGUAGE_JAPANESE);

        if (LOCALISATIONMANAGER)
        {
            for (u32 i = 0; i < m_textTabWithVarTags.size(); ++i)
            {
                TextInfo& textInfo = m_textTabWithVarTags[i];
                if (textInfo.m_lineId != U32_INVALID)
                {
                    textInfo.m_text = LOCALISATIONMANAGER->getText(textInfo.m_lineId).cStr();
                }

                if (wasJapanese && !isJapanese)
                {
                    textInfo.m_size /= JAPANESE_SCALE;
                }
                else if (!wasJapanese && isJapanese)
                {
                    textInfo.m_size *= JAPANESE_SCALE;
                }

                if (!textInfo.m_text.isEmpty())
                {
                    parseActorTag(textInfo, textInfo.m_text, bfalse);
                }
            }
        }

        if (wasJapanese && !isJapanese)
        {
            m_highestSize /= JAPANESE_SCALE;
        }
        else if (!wasJapanese && isJapanese)
        {
            m_highestSize *= JAPANESE_SCALE;
        }

        m_textChanged = btrue;
        m_stopAudio = btrue;
        m_launchAudio = btrue;
    }

    void UITextBox::showActors( bbool _show )
    {
        u32 numActors = m_spawnedActors.size();

        if ( numActors )
        {
            EventShow showEvent(_show?1.f:0.f);

            for ( u32 i = 0; i < numActors; i++ )
            {
                Actor* actor = m_spawnedActors[i].m_ref.getActor();

                if ( actor )
                {
                    actor->onEvent(&showEvent);
                }
            }
        }
    }

    void UITextBox::onBecomeActive() 
    {
        Super::onBecomeActive();

        if (m_display)
        {
            show();
        }
        m_restart = btrue;
        m_launchAudio = btrue;
    }

    void UITextBox::onBecomeInactive()
    {
        Super::onBecomeInactive();

        showActors(bfalse);
        stopAudio();
    }

    ///////////////////////////////////////////////////////////////
    void UITextBox::launchAudio()
    {
        u32 numTabs = m_textTabWithVarTags.size();

        for ( u32 i = 0;  i < numTabs; i++ )
        {
            const TextInfo& textInfo = m_textTabWithVarTags[i];

            if ( textInfo.m_lineId != U32_INVALID )
            {
#ifdef  ITF_SUPPORT_WWISE
                const StringID wwiseEventGuid = AUDIO_ADAPTER->getLocalizationWWiseGUIdFromLocId(textInfo.m_lineId.value);
                if (wwiseEventGuid.isValid())
                {
                    const Path audioPath = LOCALISATIONMANAGER->getAudioPath(textInfo.m_lineId);
                    const f32 audioVolume = AUDIO_ADAPTER->getLocalizationVolumeFromLocId(textInfo.m_lineId.value);
                    StringID id = StringID(textInfo.m_lineId.value);
                    MUSICMANAGER->addMusic(id, audioPath, ITF_GET_STRINGID_CRC(DIALS, 3177981930), wwiseEventGuid);
                    MUSICMANAGER->play(id, 0, Volume::DecibelsToAmplitudeRatio(audioVolume), bfalse);
                    m_playingSounds.push_back(id);
                }

#else
                const Path audioPath = LOCALISATIONMANAGER->getAudioPath(textInfo.m_lineId);
                const f32 audioVolume = LOCALISATIONMANAGER->getAudioVolume(textInfo.m_lineId);

                if ( !audioPath.isEmpty() )
                {

                    StringID id = StringID(textInfo.m_lineId.value);
                    StringID wwiseEventGuid{};

                    MUSICMANAGER->addMusic(id, audioPath, ITF_GET_STRINGID_CRC(DIALS,3177981930), wwiseEventGuid);
                    MUSICMANAGER->play(id, 0, Volume::DecibelsToAmplitudeRatio(audioVolume), bfalse);

                    m_playingSounds.push_back(id);

                }
#endif
            }

        }
    }

    ///////////////////////////////////////////////////////////////
    void UITextBox::stopAudio()
    {
        u32 numSounds = m_playingSounds.size();

        for ( u32 i = 0;  i < numSounds; i++ )
        {
            if ( MUSICMANAGER ) // because stop Audio gets called by Actor::onBecomeInactive in Ray_GameInterface singleton destructor, after MUSICMANAGER destructor 
            {
                MUSICMANAGER->stop(m_playingSounds[i]);
                MUSICMANAGER->removeMusic(m_playingSounds[i]);
            }
        }

        m_playingSounds.clear();
    }

    bbool UITextBox::isDialogPlaying() const
    {
        for (u32 i = 0; i < m_playingSounds.size(); ++i)
        {
            if (MUSICMANAGER->isMusicPlaying(m_playingSounds[i]))
            {
                return btrue;
            }
        }
        return bfalse;
    }
    ///////////////////////////////////////////////////////////////
    void UITextBox::Update( f32 _deltaTime )
    {
        if ( !m_actorsReadyToDraw )
        {
            updateReadyToDraw();
        }
        else
        {
            updateActors();
        }

        if ( m_stopAudio )
        {
            stopAudio();
            m_stopAudio = bfalse;
        }

        if (m_launchAudio)
        {
            launchAudio();
            m_launchAudio = bfalse;
        }

        if ( m_textChanged && m_actorsReadyToDraw && m_display )
        {
            updateVariables();
            m_textChanged = bfalse;
        }
        else
        {
            for (u32 i = 0; i < m_playingSounds.size(); ++i)
            {
                if (!MUSICMANAGER->isMusicPlaying(m_playingSounds[i]))
                {
                    MUSICMANAGER->removeMusic(m_playingSounds[i]);
                    m_playingSounds.eraseKeepOrder(i);
                    i--;
                }
            }
        }

        scaleBox(btrue);

        // Scale the font and the box to fit with the screen (screen resolution of reference: 800x600) 
        if (UI_TEXTMANAGER->needsScale() || m_boxWidth == 0)
        {
            m_textBoxScale = UI_TEXTMANAGER->getTextBoxScale();
            // This scalebox is resetting the boxwidth and boxheight, overriding the value entered from the setdata, and it's also making the text
            // draw in a different position unaligning it with the rest. I need to review what's wrong with it
            //scaleBox();

            const Vec2d& initTextureOffset2D =getTemplate()->getInitTextureOffset2D();
            const Vec2d& initAnchorOffset2D =getTemplate()->getInitAnchorOffset2D();
            m_textureOffset2D = Vec2d(initTextureOffset2D.m_x * m_textBoxScale, initTextureOffset2D.m_y * m_textBoxScale);
            m_anchorOffset2D = Vec2d(initAnchorOffset2D.m_x * m_textBoxScale, initAnchorOffset2D.m_y * m_textBoxScale);
        }

        updateAABB();

        if(getTemplate()->getUsePages() && m_display)
        {
            InputAdapter::PressStatus buttons[JOY_MAX_BUT];
            INPUT_ADAPTER->getGamePadButtons(InputAdapter::EnvironmentLua,0, buttons, JOY_MAX_BUT);
            if (buttons[m_joyButton_A]  == InputAdapter::JustPressed)
            {
                changePage();
            }
        }

        m_restart = bfalse;
    } 

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::updateAABB()
    {
        Vec2d minBound = Vec2d::Zero;
        Vec2d maxBound = Vec2d::Zero;

        if( getTemplate()->getIs2DText() || m_actor->is2DActor() )
        {
            getBoxBounds2D(m_actor->getPos(), minBound, maxBound);
        }
        else
        {
            getBoxBounds3D(m_actor->getPos(),minBound,maxBound);
        }

        AABB aabb(minBound);
        aabb.grow(maxBound);
        GetActor()->growRelativeAABBFromAbsolute(aabb);
    }

    f32 UITextBox::getScreenRatio()
    {
        return GFX_ADAPTER->getScreenWidth() / UI2D_WidthRef;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::setData(const Vec3d& _pos, f32 _highestSize, const TextInfo& _textInfo, f32 _width, f32 _height,
            i32 _depthRank, const Vec2d& _startUV, const Vec2d& _endUV, u32 _colour, bbool _useBackground, 
            const BoxPosition& _boxPosition, const Vec2d& _shadowOffset, u32 _shadowColor, f32 _lineFactor, const ResourceID& _backTexture, u32 _fontIndex, u32 _mode, u32 _modeY)
    {
        m_actor->setPos(_pos);

        m_textTabWithVarTags.resize(1);

        // Using localized text ?
        if ( _textInfo.m_lineId != U32_INVALID )
        {
            // Text has changed ?
            if ( m_textTabWithVarTags[0].m_lineId != _textInfo.m_lineId )
            {
                m_textTabWithVarTags[0] = _textInfo;
                // get the localized text
                m_textTabWithVarTags[0].m_text = LOCALISATIONMANAGER->getText(_textInfo.m_lineId).cStr();
            }
            // Keep the same translated text -> set everything else to the new value
            else
            {
                String previousText;
                previousText.setText(m_textTabWithVarTags[0].m_text);
                m_textTabWithVarTags[0] = _textInfo;
                m_textTabWithVarTags[0].m_text = previousText;
            }
        }
        // Set new text
        else
        {
            m_textTabWithVarTags[0] = _textInfo;
        }

        if ( LOCALISATIONMANAGER->getCurrentLanguage() == ITF_LANGUAGE_JAPANESE )
            m_textTabWithVarTags[0].m_size *= JAPANESE_SCALE;
        m_highestSize = LOCALISATIONMANAGER->getCurrentLanguage() == ITF_LANGUAGE_JAPANESE ? _highestSize * JAPANESE_SCALE : _highestSize;
        m_display = btrue;
        if(_width > 0.0f)   m_boxWidth = _width;
        if(_height > 0.0f)  m_boxHeight = _height;
        m_boxPosition = _boxPosition;
        m_depthRank = _depthRank;
        m_backTexture = _backTexture;
        m_startUV = _startUV;
        m_endUV = _endUV;
        m_backgroundColour = _colour;
        m_useBackground = _useBackground;
        if(_fontIndex!=U32_INVALID) m_fontIndex = _fontIndex;
        if(_mode != U32_INVALID)
        {
            m_mode = _mode;
        }

        m_modeY = _modeY;

        m_shadowOffset = _shadowOffset;
        m_shadowColor = _shadowColor;

        m_textChanged = btrue;
        m_stopAudio = btrue;

        m_lineSpacingFactor = _lineFactor;
    }

    void UITextBox::setTextAt(const TextInfo& _textInfo, u32 _index )
    {
        ITF_ASSERT(_index < m_textTabWithVarTags.size() );

        if ( _index < m_textTabWithVarTags.size() )
        {
            // Using localized text ?
            bbool textChanged = bfalse;

            if ( _textInfo.m_lineId != U32_INVALID )
            {
                // Text has changed ?
                if ( m_textTabWithVarTags[_index].m_lineId != _textInfo.m_lineId )
                {
                    m_textTabWithVarTags[_index] = _textInfo;
                    // get the localized text
                    m_textTabWithVarTags[_index].m_text = LOCALISATIONMANAGER->getText(_textInfo.m_lineId).cStr();
                    textChanged = btrue;
                }
                // Keep the same translated text -> set everything else to the new value
                else
                {
                    String previousText;
                    previousText.setText(m_textTabWithVarTags[0].m_text);
                    m_textTabWithVarTags[_index] = _textInfo;
                    m_textTabWithVarTags[_index].m_text = previousText;
                }
            }
            // Set new text
            else
            {
                if ( m_textTabWithVarTags[_index].m_text.isEmpty() != _textInfo.m_text.isEmpty() ||
                     m_textTabWithVarTags[_index].m_text != _textInfo.m_text )
                {
                    textChanged = btrue;
                    m_textTabWithVarTags[_index].m_text = _textInfo.m_text;
                }

                m_textTabWithVarTags[_index].m_friendly = _textInfo.m_friendly;
                m_textTabWithVarTags[_index].m_lineId = _textInfo.m_lineId;
                m_textTabWithVarTags[_index].m_size = _textInfo.m_size;
            }

            m_textTabWithVarTags[_index].m_color = _textInfo.m_color;

            if ( LOCALISATIONMANAGER->getCurrentLanguage() == ITF_LANGUAGE_JAPANESE )
                m_textTabWithVarTags[_index].m_size *= JAPANESE_SCALE;

            if ( textChanged || m_restart )
            {
                parseActorTag(m_textTabWithVarTags[_index], m_textTabWithVarTags[_index].m_text,bfalse);

                if ( m_textTabWithVarTags[_index].m_actors.size() > 0 )
                {
                    showActors(bfalse);
                }

                m_stopAudio     = btrue;
                m_launchAudio   = btrue;
            }

            m_textChanged = btrue;
        }
    }

    void UITextBox::addTextFromLineId(const LocalisationId& _lineId, const Color& _color )
    {
        TextInfo textInfo;

        textInfo.m_color = _color.getAsU32();
        textInfo.m_size = m_highestSize;
        textInfo.m_lineId = _lineId;
        textInfo.m_text = _lineId != LocalisationId::Invalid ? LOCALISATIONMANAGER->getText(_lineId) : String::emptyString;

        parseActorTag(textInfo, textInfo.m_text,bfalse);

        showActors(bfalse);

        m_textTabWithVarTags.push_back(textInfo);

        m_textChanged   = btrue;
        m_stopAudio     = btrue;
        m_launchAudio   = btrue;
    }

    void UITextBox::setTextFromLineId(const LocalisationId& _lineId, const Color& _color )
    {
        TextInfo textInfo;

        textInfo.m_color = _color.getAsU32();
        textInfo.m_size = m_highestSize;
        textInfo.m_lineId = _lineId;
        textInfo.m_text = _lineId != U32_INVALID ? LOCALISATIONMANAGER->getText(_lineId).cStr() : String8::emptyString;

        parseActorTag(textInfo, textInfo.m_text,bfalse);

        showActors(bfalse);

        m_textTabWithVarTags.resize(1);
        m_textTabWithVarTags[0] = textInfo;

        m_textChanged   = btrue;
        m_stopAudio     = btrue;
        m_launchAudio   = btrue;
    }

    void UITextBox::addTextFromStr( const TextInfo& _textInfo )
    {
        m_textTabWithVarTags.push_back(_textInfo);
        if ( LOCALISATIONMANAGER->getCurrentLanguage() == ITF_LANGUAGE_JAPANESE )
            m_textTabWithVarTags.back().m_size *= JAPANESE_SCALE;

        TextInfo& textInfo = m_textTabWithVarTags[m_textTabWithVarTags.size()-1];

        if ( textInfo.m_lineId != U32_INVALID && textInfo.m_text.isEmpty() )
        {
            textInfo.m_text = LOCALISATIONMANAGER->getText(textInfo.m_lineId).cStr();
        }

        parseActorTag(textInfo, textInfo.m_text,bfalse);

        showActors(bfalse);

        m_textChanged = btrue;
    }

    void UITextBox::clearText()
    {
        m_textTabWithVarTags.clear();
    }

#ifdef ITF_SUPPORT_EDITOR
    ////////////////////////////////////////////////////////////////////////////
    void UITextBox::drawBox(const f32 _border /*= 1.0f*/) const
    {
        Vec2d boxMinPosition = Vec2d::Zero;
        Vec2d boxMaxPosition = Vec2d::Zero;
        if( getTemplate()->getIs2DText() || m_actor->is2DActor() )
        {
            getBoxBounds2D(m_actor->getPos(), boxMinPosition, boxMaxPosition);
            drawBox2D(boxMinPosition, boxMaxPosition, _border);
        }
        else
        {
            getBoxBounds3D(m_actor->getPos(), boxMinPosition, boxMaxPosition);
            drawBox3D(boxMinPosition, boxMaxPosition, _border);
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    void UITextBox::drawBox2D(const Vec2d & _minBound, const Vec2d & _maxBound, const f32 _border) const
    {
        Vec2d bottomRight(_maxBound.m_x, _minBound.m_y);
        Vec2d topLeft(_minBound.m_x, _maxBound.m_y);
        GFX_ADAPTER->draw2dLine(_minBound, topLeft, _border, COLOR_RED);
        GFX_ADAPTER->draw2dLine(topLeft, _maxBound, _border, COLOR_RED);
        GFX_ADAPTER->draw2dLine(_maxBound, bottomRight, _border, COLOR_RED);
        GFX_ADAPTER->draw2dLine(bottomRight, _minBound, _border, COLOR_RED);
    }


    ////////////////////////////////////////////////////////////////////////////
    void UITextBox::drawBox3D(const Vec2d & _minBound, const Vec2d & _maxBound, const f32 _border) const
    {
        Vec2d bottomRight(_maxBound.m_x, _minBound.m_y);
        Vec2d topLeft(_minBound.m_x, _maxBound.m_y);
        DebugDraw::line2D(_minBound, topLeft, m_actor->getDepth(), Color::red(), _border);
        DebugDraw::line2D(topLeft, _maxBound, m_actor->getDepth(), Color::red(), _border);
        DebugDraw::line2D(_maxBound, bottomRight, m_actor->getDepth(), Color::red(), _border);
        DebugDraw::line2D(bottomRight, _minBound, m_actor->getDepth(), Color::red(), _border);
    }
#endif //ITF_SUPPORT_EDITOR

    ////////////////////////////////////////////////////////////////////////////
    void UITextBox::getBoxBounds2D(const Vec3d & _refPosition, Vec2d & _minBound, Vec2d & _maxBound) const
    {
        const f32 halfBoxWidth = m_boxWidth * 0.5f;
        const f32 halfBoxHeight = m_boxHeight * 0.5f;

        switch (m_boxPosition)
        {
        case BoxPosition_Center:
            {
                _minBound.set(_refPosition.m_x - halfBoxWidth, _refPosition.m_y - halfBoxHeight);
                _maxBound.set(_refPosition.m_x + halfBoxWidth, _refPosition.m_y + halfBoxHeight);
            }
            break;
        case BoxPosition_TopLeft:
            {
                _minBound.set(_refPosition.m_x, _refPosition.m_y);
                _maxBound.set(_refPosition.m_x + m_boxWidth, _refPosition.m_y + m_boxHeight);
            }
            break;
        case BoxPosition_Left:
            {
                _minBound.set(_refPosition.m_x, _refPosition.m_y - halfBoxHeight);
                _maxBound.set(_refPosition.m_x + m_boxWidth, _refPosition.m_y + halfBoxHeight);
            }
            break;
        default:
            ITF_ASSERT(0);
            break;
        }
    }

    void UITextBox::getBoxBounds3D(const Vec3d & _refPosition, Vec2d & _minBound, Vec2d & _maxBound) const
    {
        const f32 halfBoxWidth = m_boxWidth * 0.5f;
        const f32 halfBoxHeight = m_boxHeight * 0.5f;

        switch (m_boxPosition)
        {
        case BoxPosition_Center:
            {
                _minBound.set(_refPosition.m_x - halfBoxWidth, _refPosition.m_y + halfBoxHeight);
                _maxBound.set(_refPosition.m_x + halfBoxWidth, _refPosition.m_y - halfBoxHeight);
            }
            break;
        case BoxPosition_TopLeft:
            {
                _minBound.set(_refPosition.m_x, _refPosition.m_y);
                _maxBound.set(_refPosition.m_x + m_boxWidth, _refPosition.m_y - m_boxHeight);
            }
            break;
        case BoxPosition_Left:
            {
                _minBound.set(_refPosition.m_x, _refPosition.m_y + halfBoxHeight);
                _maxBound.set(_refPosition.m_x + m_boxWidth, _refPosition.m_y - halfBoxHeight);
            }
            break;
        default:
            ITF_ASSERT(0);
            break;
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    void UITextBox::Draw ()
    {
        if ( !m_actorsReadyToDraw )
        {
            return;
        }

        if ( getTemplate()->getIs2DText() )
        {
            Draw2D();
            return;
        }

        if (m_display)
		{
            const f32 halfBoxWidth = m_boxWidth * 0.5f;
            const f32 halfBoxHeight = m_boxHeight * 0.5f;

#ifdef ITF_SUPPORT_DEBUGFEATURE
        	if (getTemplate()->getIsDrawBox())
            {
               drawBox();
        	}
#endif //ITF_SUPPORT_DEBUGFEATURE

            // Draw the text in the box
            if (m_fontIndex!= U32_INVALID)
            {
                Vec3d refPosition = m_actor->getPos();
				
                switch(m_boxPosition)
                {
                case BoxPosition_Center:
                    {
                        refPosition.m_x -= halfBoxWidth;
                        refPosition.m_y += halfBoxHeight;
                    }
                    break;
                case BoxPosition_TopLeft:
                    break;
                case BoxPosition_Left:
                    {

                        refPosition.m_y += halfBoxHeight;
                    }
                    break;
                default:
                    ITF_ASSERT(0);
                    break;
                }

                refPosition.m_z += getTemplate()->getZOffset();

				drawFontAtPos(refPosition);
            }
        }
    }

    void UITextBox::Draw2D()
    {
        if (m_display)
        {
            const f32 halfBoxWidth = m_boxWidth * 0.5f;
            const f32 halfBoxHeight = m_boxHeight * 0.5f;

#ifdef ITF_SUPPORT_DEBUGFEATURE
            if (getTemplate()->getIsDrawBox())
            {
                drawBox();
            }
#endif //ITF_SUPPORT_DEBUGFEATURE

            // Draw the text in the box
            if (m_fontIndex!= U32_INVALID)
            {
                Vec3d refPosition = m_actor->getPos();

                switch(m_boxPosition)
                {
                case BoxPosition_Center:
                    {
                        refPosition.m_x -= halfBoxWidth;
                        refPosition.m_y -= halfBoxHeight;
                    }
                    break;
                case BoxPosition_TopLeft:
                    break;
                case BoxPosition_Left:
                    {

                        refPosition.m_y -= halfBoxHeight;
                    }
                    break;
                default:
                    ITF_ASSERT(0);
                    break;
                }

                drawFontAtPos(refPosition);
            }
        }
    }

    void UITextBox::drawFontAtPos(const Vec3d& _pos)
    {
        Texture* tex = static_cast<Texture*>(m_backTexture.getResource());
        FontCall fontcall;
        fontcall.setFont(GFX_ADAPTER->getFontByIndex(m_fontIndex));
        fontcall.setPosition(_pos);
        u32 color = COLOR_WHITE & (u32)(m_textAlpha * 0xFF);
        fontcall.setColor(color);

#ifdef demotest
        //demo ici.
        static f32 time = 0;
        time += LOGICDT;
        static f32 r = 0.f;
        r += 0.01f;

        fontcall.setBoxRotation(r);
        fontcall.setBoxScale(Vec3d(1.f * sinf(time), 2.f * cosf(time), 1.f));
        fontcall.setBoxPivot(Vec3d(100.f,0.f,0.f));
#else
        if( getTemplate()->getUseActorRotation() )
        {
            m_textRotation = m_actor->getAngle();
        }
        fontcall.setBoxRotation(m_textRotation);

        fontcall.setBoxPivot(Vec3d(0.f,0.f,0.f));
        
        if ( (getTemplate()->getUseActorScale() && !m_actor->is2DActor()) || !m_noScaleWithActor )
        {
            m_textScale.m_x = m_actor->getScale().m_x;
            m_textScale.m_y = m_actor->getScale().m_y;
        }
        fontcall.setBoxScale(m_textScale);
#endif
        if( getTemplate()->getUseActorPosition() )
        {
            fontcall.setBoxPosition(m_actor->getPos());
        }
        else
        {
            fontcall.setBoxPosition(_pos);
        }
        
        fontcall.setUseTransBox(btrue); //btrue

        fontcall.setShadowOffset(m_shadowOffset);
        fontcall.setlineSpacingFactor(m_lineSpacingFactor);
        fontcall.setShadowColor(m_shadowColor);

        fontcall.setCount(0);
        fontcall.setMode(m_mode);
        fontcall.setModeY(m_modeY);
        fontcall.setAppend(bfalse);

        fontcall.setUseStyle(btrue);
        fontcall.setHeight(m_highestSize);
        fontcall.setBoxWidth(m_boxWidth);
        fontcall.setBoxHeight(m_boxHeight);
        
        fontcall.setTextTab(&m_textTab);
        fontcall.setTextCroppingMode(getTemplate()->getCroppingMode());
        fontcall.setBackGroundTexture(m_backTexture);
        fontcall.setBackGroundTextureStartUV(m_startUV);
        fontcall.setBackGroundTextureEndUV(m_endUV);
        fontcall.setBackGroundColour(m_backgroundColour);
        fontcall.setTextureOffset2D(m_textureOffset2D);
        fontcall.setScale(m_textBoxScale);
        fontcall.setIs2D(getTemplate()->getIs2DText() || m_actor->is2DActor());
        fontcall.setWrite(m_actorsReadyToDraw);
        if (tex)
        {
            fontcall.setUseBackground(btrue);
        }
        else 
        {
            fontcall.setUseBackground(m_useBackground);
        }

        if (getTemplate()->getUsePages())
        {
            ITF_ASSERT(m_currentPage < m_blockData.size());
            u32 textTabStart = m_blockData[m_currentPage].m_index;
            u32 textTabStop = m_textTab.size();
            if (m_currentPage + 1 < m_blockData.size())
                textTabStop = m_blockData[m_currentPage+1].m_index;

            fontcall.setTextTabStart(textTabStart);
            fontcall.setTextTabStop(textTabStop);
        }
        else
        {
            fontcall.setTextTabStart(0);
            fontcall.setTextTabStop(U32_INVALID);
        }

        GFX_ADAPTER->drawTextWithStyle(&fontcall, m_depthRank);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::fillTextTab()
    {       
        u32 nbBlock = 0;
        for (ITF_VECTOR<BlockData>::iterator iter = m_blockData.begin(); iter!=m_blockData.end(); iter++)
        {
            if (UI_TEXTMANAGER->isReady())
            {
                (*iter).m_index = m_textTabWithVarTags.size();
                TextInfo textInfo;
                u32 indexStart = 0;
                while (UI_TEXTMANAGER->getTextByFriendly((*iter).m_friendly, textInfo, indexStart))
                    m_textTabWithVarTags.push_back(textInfo);
                nbBlock++;
            }
        }
    }
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    String UITextBox::parseVariableTag (const String &_text)
    {
        static const String varTag = "[var:";
        static const u32 varTagBeginSize = varTag.getLen();
        i32 indexVarBegin, indexVarEnd;
        String textWithoutTag = "";

        _text.strstr(varTag.cStr(), bfalse, &indexVarBegin);

        if ( indexVarBegin != -1 )
        {
            String varText = _text.substr(indexVarBegin, _text.getLen() - indexVarBegin);

            varText.strstr(']',bfalse,&indexVarEnd);

            if ( indexVarEnd != -1 )
            {
                const String variableName = varText.substr(varTagBeginSize, indexVarEnd - varTagBeginSize);
                const u32 varTagSize = varTagBeginSize + variableName.getLen() + 1;

                textWithoutTag += _text.substr(0, indexVarBegin);
                textWithoutTag += UI_TEXTMANAGER->getVariableValue(variableName);
                textWithoutTag += parseVariableTag(_text.substr(indexVarBegin + varTagSize, _text.getLen() - (indexVarBegin + varTagSize)));
            }
            else
            {
                textWithoutTag += _text;
            }
        }
        else
        {
            textWithoutTag += _text;
        }
        
        return textWithoutTag;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::updateActors()
    {
        if ( !m_actor->isVisible(CAMERA) )
        {
            return;
        }

        u32 numTabs = m_textTab.size();
        Vec2d scale = m_actor->getScale()/m_actor->getWorldInitialScale();

        for ( u32 tabIndex = 0; tabIndex < numTabs; tabIndex++ )
        {
            TextInfo& textInfo = m_textTab[tabIndex];
            u32 numActors = textInfo.m_actors.size();

            for ( u32 actorIndex = 0; actorIndex < numActors; actorIndex++ )
            {
                TextActorInfo& actorInfo = textInfo.m_actors[actorIndex];

                u32 numSpawnedActors = m_spawnedActors.size();
                bbool found = bfalse;

                for ( u32 spawnIndex = 0; spawnIndex < numSpawnedActors; spawnIndex++ )
                {
                    if ( m_spawnedActors[spawnIndex].m_pathId == actorInfo.m_path )
                    {
                        Actor* actor = m_spawnedActors[spawnIndex].m_ref.getActor();

                        if ( actor )
                        {
                            actorInfo.m_pos.m_z = m_actor->getPos().m_z;
                            actorInfo.m_pos.m_z += getTemplate()->getActorsZOffset();

                            actor->setPos(actorInfo.m_pos);
                            actor->setScale(actor->getWorldInitialScale()*scale);
                        }
                        found = btrue;
                        break;
                    }
                }
                }
            }
        }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::updateReadyToDraw()
    {
        m_actorsReadyToDraw = btrue;

        u32 numActors = m_spawnedActors.size();
        u32 numTextInfo = m_textTab.size();
        u32 numTextInfoVars = m_textTabWithVarTags.size();

        for ( u32 i = 0; i < numActors; i++ )
        {
            ActorInfo& actorInfo = m_spawnedActors[i];
            Actor* actor = actorInfo.m_ref.getActor();

            if ( actor && actor->isActive() )
            {
                const AABB& aabb = actor->getAABB();

                if ( aabb.getMin() != aabb.getMax() )
                {
                    f32 width = aabb.getMax().m_x - aabb.getMin().m_x;
                    f32 height = aabb.getMax().m_y - aabb.getMin().m_y;

                    for ( u32 j = 0; j < numTextInfo; j++ )
                    {
                        TextInfo& textInfo = m_textTab[j];
                        u32 numActors = textInfo.m_actors.size();
                        
                        for ( u32 actorIndex = 0; actorIndex < numActors; actorIndex++ )
                        {
                            TextActorInfo& textActorInfo = textInfo.m_actors[actorIndex];

                            if ( textActorInfo.m_ref == actorInfo.m_ref )
                            {
                                textActorInfo.m_width = width;
                                textActorInfo.m_height = height;
                            }
                        }
                    }

                    for ( u32 j = 0; j < numTextInfoVars; j++ )
                    {
                        TextInfo& textInfo = m_textTabWithVarTags[j];
                        u32 numActors = textInfo.m_actors.size();

                        for ( u32 actorIndex = 0; actorIndex < numActors; actorIndex++ )
                        {
                            TextActorInfo& textActorInfo = textInfo.m_actors[actorIndex];

                            if ( textActorInfo.m_ref == actorInfo.m_ref )
                            {
                                textActorInfo.m_width = width;
                                textActorInfo.m_height = height;
                            }
                        }
                    }
                }
            }
            else
            {
                m_actorsReadyToDraw = bfalse;
                break;
            }
        }

        if ( m_actorsReadyToDraw && m_display )
        {
            showActors(btrue);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::parseActorTag ( TextInfo& _textInfo, const String& _text, bbool _recurse )
    {
        // First remove the current actors
        if ( !_recurse && _textInfo.m_actors.size() > 0 )
        {
            u32 numActors = _textInfo.m_actors.size();

            for ( u32 i = 0; i < numActors; i++ )
            {
                ObjectRef refActor = _textInfo.m_actors[i].m_ref;
                Actor* actor = AIUtils::getActor(refActor);

                if ( actor )
                {
                    actor->requestDestruction();
                }

                

                for ( ITF_VECTOR <ActorInfo>::iterator it = m_spawnedActors.begin(); it != m_spawnedActors.end(); ++it )
                {
                    if ( it->m_ref == refActor )
                    {
                        m_spawnedActors.erase(it);
                        break;
                    }
                }
            }

            _textInfo.m_actors.clear();
        }

        static const String actorTag = "[actor:";
        static const u32 varTagBeginSize = actorTag.getLen();
        i32 indexActorBegin, indexActorEnd;

        _text.strstr(actorTag.cStr(), bfalse, &indexActorBegin);

        if ( indexActorBegin != -1 )
        {
            String varText = _text.substr(indexActorBegin, _text.getLen() - indexActorBegin);

            varText.strstr(']',bfalse,&indexActorEnd);

            if ( indexActorEnd != -1 )
            {
                const String actorName = varText.substr(varTagBeginSize, indexActorEnd - varTagBeginSize);
                const String8 actorName8(actorName.cStr());
                const u32 actorTagSize = varTagBeginSize + actorName.getLen() + 1;

                Path filePath = actorName8.cStr();

                Actor* regActor = ACTORSMANAGER->spawnActor(m_actor->getPos(),m_actor->getScene(),filePath,btrue);

                if ( regActor )
                {
                    regActor->setAngle(m_actor->getAngle());
                    regActor->setIsSpawned(btrue);

                    ActorInfo actorInfo;

                    actorInfo.m_pathId = actorName8;
                    actorInfo.m_ref = regActor->getRef();

                    m_spawnedActors.push_back(actorInfo);

                    TextActorInfo textActorInfo;

                    textActorInfo.m_path = actorName8;
                    textActorInfo.m_height = 0.f;
                    textActorInfo.m_width = 0.f;
                    textActorInfo.m_pos = m_actor->getPos(); // NB[LaurentCou]: fix for actors spawned at origin preventing unload of WM level selections
                    textActorInfo.m_ref = regActor->getRef();

                    _textInfo.m_actors.push_back(textActorInfo);

                    m_actorsReadyToDraw = bfalse;
                }

                parseActorTag(_textInfo, _text.substr(indexActorBegin + actorTagSize, _text.getLen() - (indexActorBegin + actorTagSize)),btrue);
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::updateVariables ()
    {
        m_textTab.clear();
        for (ITF_VECTOR<TextInfo>::iterator iter = m_textTabWithVarTags.begin(); iter!=m_textTabWithVarTags.end(); iter++)
        {
            // use localized text if available
            const TextInfo& srcTextInfo = *iter;
            TextInfo curText (parseVariableTag( srcTextInfo.m_text), srcTextInfo.m_color, srcTextInfo.m_size, srcTextInfo.m_friendly, srcTextInfo.m_lineId, srcTextInfo.m_actors );
            m_textTab.push_back(curText);
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::scaleBox(bbool _only2d)
    {
        if(getTemplate()->getIs2DText())
        {
            int Screen_W = 0, Screen_H = 0;
            SYSTEM_ADAPTER->getWindowSize(Screen_W, Screen_H);
            Vec2d scale = m_actor->getScale();

            // If the actor is a 2D actor, dont get his scale (too large)
            if(m_actor->is2DActor())
                scale = Vec2d::One;

            f32 m_screenPourcentX = (100.0f * getTemplate()->getInitialBoxWidth() * scale.m_x) / UI2D_WidthRef;
            f32 m_screenPourcentY = (100.0f * getTemplate()->getInitialBoxHeight() * scale.m_y) / UI2D_HeightRef;

            m_boxWidth  = Screen_W * m_screenPourcentX * 0.01f;
            m_boxHeight = Screen_H * m_screenPourcentY * 0.01f;
        }
        else if (!_only2d)
        {
            m_boxHeight = getTemplate()->getInitialBoxHeight() * m_textBoxScale;
            if(!m_boxHeight)
            {
                m_boxWidth = 0.0f;
            }
            else
            {
                m_boxWidth = m_boxHeight * getTemplate()->getInitialBoxWidth() / getTemplate()->getInitialBoxHeight();
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::changePage()
    {
        m_currentPage++;
        if (m_currentPage >= m_blockData.size())
        {
            m_display = bfalse;
            m_currentPage = 0;
            INPUT_ADAPTER->enableEnvironment(InputAdapter::EnvironmentEngine);
            Actor * sender = static_cast<Actor*>(GETOBJECT(m_sender));
            if (sender)
            {
                hide();
                m_actor->disable();
                EventEndWait onEndWait;
                sender->onEvent(&onEndWait);
            }
        }
    }


    bbool UITextBox::needsDraw() const
    {
        return  !m_actor->is2DActor() && !getTemplate()->getIs2DText();
    }

    bbool UITextBox::needsDraw2D() const
    {
        return m_actor->is2DActor() || getTemplate()->getIs2DText();
    }

    u32 UITextBox::calculateNumOfCharactersFromLoc( const LocalisationId& _locId )
    {
        String text = LOCALISATIONMANAGER->getText(_locId);

        return calculateNumOfCharactersFromStr(text);
    }

    u32 UITextBox::calculateNumOfCharactersFromStr( const String& _str )
    {
        u32 count = 0;

        u32 len = _str.getLen();
        u32 currentChar = 0;

        static const u16 strTagOpen = '[';
        static const u16 strTagClose = ']';
        bbool inTag = bfalse;

        while ( currentChar < len )
        {
            u16 ch = _str.cStr()[currentChar];
            if ( ch == strTagOpen )
            {
                inTag = btrue;
            }

            if ( ch == strTagClose && inTag)
            {
                inTag = bfalse;
            }

            if(!inTag)
            {
                count++;
            }

            currentChar++;
        }

        return count;
    }

#ifdef ITF_SUPPORT_EDITOR
    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::drawEdit( class ActorDrawEditInterface* /*drawInterface*/, u32 _flags ) const 
    {
        drawBox(3.0f);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void UITextBox::onEditorMove(bbool _modifyInitialPos)
    {
        //scaleBox();
    }
#endif // ITF_SUPPORT_EDITOR

    //-------------------------------------------------------------------------------------

    IMPLEMENT_OBJECT_RTTI(UITextBox_Template)
    BEGIN_SERIALIZATION(UITextBox_Template)

        SERIALIZE_MEMBER("fontName",m_fontName); 
        SERIALIZE_MEMBER("textMode",m_textMode); //FONT_ALIGN_LEFT = 0,  FONT_ALIGN_CENTER = 1, FONT_ALIGN_RIGHT = 2, FONT_ALIGN_JUSTIFY = 3
        SERIALIZE_MEMBER("textModeY",m_textModeY); //FONT_ALIGNY_TOP = 0,  FONT_ALIGNY_CENTER = 1
        SERIALIZE_MEMBER("textHeight", m_textHeight);
        SERIALIZE_MEMBER("boxWidth",m_initialBoxWidth);
        SERIALIZE_MEMBER("boxHeight",m_initialBoxHeight);
        SERIALIZE_MEMBER("isDrawBox",m_isDrawBox);
        SERIALIZE_ENUM_BEGIN("croppingMode",m_croppingMode);
            SERIALIZE_ENUM_VAR(Cropping_Scale);
            SERIALIZE_ENUM_VAR(Cropping_Cut);
        SERIALIZE_ENUM_END();
        SERIALIZE_MEMBER("usePages",m_usePages);
        SERIALIZE_ENUM_BEGIN("boxPosition",m_boxPosition);
            SERIALIZE_ENUM_VAR(BoxPosition_TopLeft);
            SERIALIZE_ENUM_VAR(BoxPosition_Center);
            SERIALIZE_ENUM_VAR(BoxPosition_Left);
        SERIALIZE_ENUM_END();
        SERIALIZE_CONTAINER("textBlock",m_blocksFriendly);
        SERIALIZE_MEMBER("texture",m_textureFile);
        SERIALIZE_MEMBER("textureOffset2D",m_initTextureOffset2D);
        SERIALIZE_MEMBER("anchorOffset2D",m_initAnchorOffset2D);
        SERIALIZE_MEMBER("zOffset", m_zOffset);
        SERIALIZE_MEMBER("is2DText",m_is2DText);
        SERIALIZE_MEMBER("zOffsetActors",m_actorsZOffset);
        SERIALIZE_MEMBER("useActorPosition", m_useActorPosition);
        SERIALIZE_MEMBER("useActorScale",m_useActorScale);
        SERIALIZE_MEMBER("useActorRotation", m_useActorRotation);
        SERIALIZE_MEMBER("textShadowOffset",m_textShadowOffset);  
        SERIALIZE_MEMBER("textShadowColor",m_textShadowColor);
        SERIALIZE_MEMBER("lineSpacingFactor",m_lineSpacingFactor);
    END_SERIALIZATION()

    UITextBox_Template::UITextBox_Template()
    : m_textMode(FONT_ALIGN_LEFT)
    , m_textModeY(0)
    , m_textHeight(0.f)
    , m_initialBoxHeight(0.f)
    , m_initialBoxWidth(0.f)
    , m_usePages(bfalse)
    , m_croppingMode(Cropping_Cut)
    , m_boxPosition(BoxPosition_TopLeft)
    , m_isDrawBox(bfalse)
    , m_is2DText(btrue)
    , m_initTextureOffset2D(Vec2d::Zero)  
    , m_initAnchorOffset2D(Vec2d::Zero)
    , m_zOffset(0.05f)
    , m_actorsZOffset(0.05f)
    , m_useActorPosition(btrue)
    , m_useActorScale(bfalse)
    , m_useActorRotation(btrue)
    , m_textShadowOffset(Vec2d(3.4f,3.4f))
    , m_textShadowColor(COLOR_BLACK)
    , m_lineSpacingFactor(1.f)
    {
    }
}
