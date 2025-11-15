#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_PLAYERHUDSCORECOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerHudScoreComponent.h"
#endif //_ITF_RAY_PLAYERHUDSCORECOMPONENT_H_

#ifndef _ITF_ANIMATION_H_
#include "engine/animation/Animation.h"
#endif //_ITF_ANIMATION_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_

#ifndef _ITF_UITEXTBOX_H_
#include "gameplay/components/UI/UITextBox.h"
#endif //_ITF_UITEXTBOX_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_FXBANKCOMPONENT_H_
#include "engine/actors/components/FxBankComponent.h"
#endif //_ITF_FXBANKCOMPONENT_H_

#ifndef _ITF_CINEMATICMANAGER_H_
#include "gameplay/managers/CinematicManager.h"
#endif //_ITF_CINEMATICMANAGER_H_

#ifndef _ITF_RAY_JOINLEAVEGAMEHANDLER_H_
#include "rayman/gameplay/GameScreens/Ray_JoinLeaveGameHandler.h"
#endif //_ITF_RAY_JOINLEAVEGAMEHANDLER_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_RAY_GAMESCREEN_WORLDMAP_H_
#include "rayman/gameplay/GameScreens/Ray_GameScreen_WorldMap.h"
#endif //_ITF_RAY_GAMESCREEN_WORLDMAP_H_

namespace ITF
{
    IMPLEMENT_OBJECT_RTTI(Ray_PlayerHudScoreComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_PlayerHudScoreComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_PlayerHudScoreComponent)
        VALIDATE_COMPONENT_PARAM("", (getTemplate()->getPulseIncreaseDuration()+getTemplate()->getPulseDecreaseDuration()+getTemplate()->getPulseSustainDuration())<=getTemplate()->getPulseCycleDuration_SingleLum(), "pulse increase+sustain+decrease durations are greater than total cycle");
        VALIDATE_COMPONENT_PARAM("", (getTemplate()->getPulseIncreaseDuration()+getTemplate()->getPulseDecreaseDuration()+getTemplate()->getPulseDoublePeakDuration())<=getTemplate()->getPulseCycleDuration_MultipleLums(), "pulse increase+double peak+decrease durations are greater than total cycle");
    END_VALIDATE_COMPONENT()

    static StringID s_lumScaleFrame("LumScaleFrame");

    #define _DONT_BUFFER_WHEN_PULSING

    u32 Ray_PlayerHudScoreComponent::m_forcedDisplayMask = 0;
    bbool Ray_PlayerHudScoreComponent::m_alwaysVisible = bfalse;
    bbool Ray_PlayerHudScoreComponent::m_lumCaptionEnabled = btrue;
    bbool Ray_PlayerHudScoreComponent::m_isDrawEnabled = btrue;
	bbool Ray_PlayerHudScoreComponent::m_relicCaptionEnabled = bfalse;
	bbool Ray_PlayerHudScoreComponent::m_isNewCaption = bfalse;
	u32	  Ray_PlayerHudScoreComponent::m_frescoId = 0;

    u32 Ray_PlayerHudScoreComponent::m_reallyPulsingType[s_maxPlayers] = {0,0,0,0}; //0 : none. 1 single 2 multiple

    FixedArray<u32,Ray_PlayerHudScoreComponent::maxPendingScoreIncrements> Ray_PlayerHudScoreComponent::m_pendingScoreIncrements[s_maxPlayers];

    Ray_PlayerHudScoreComponent *Ray_PlayerHudScoreComponent::m_instances[s_maxPlayers]={NULL,NULL,NULL,NULL};

    //


    Ray_PlayerHudScoreComponent::Ray_PlayerHudScoreComponent()
        : Super()
        , m_playerIndex(U32_INVALID)
        , m_visualOrder(0)
        , m_currentMaxHpFrame(0)
        , m_currentHpFrame(0)
        , m_lastActive(bfalse)
        , m_lastMaxHP(U32_INVALID)
        , m_lastHP(U32_INVALID)
        , m_lastLumCount(0)
        , m_stateChangeTimer(0)
        , m_growSpeed(4.f)
        , m_scoreFontIndex(0)
        , m_pressStartFontIndex(0)
        , m_pulseState(Pulse_None)
        , m_pulseTimer(0)
        , m_pulseCount(0)
        , m_pressStartPulseTimer(0)
        , m_scoreSizeMultiplier(1)
        , m_timeFromPulseStart(0)
        , m_captionType(Caption_Join)
        , m_currentAlpha(0)
        , m_currentLumScale(1)
    {
    }

    Ray_PlayerHudScoreComponent::~Ray_PlayerHudScoreComponent()
    {
    }

    void Ray_PlayerHudScoreComponent::onActorClearComponents()
    {
        Super::onActorClearComponents();

        if (m_playerIndex!=U32_INVALID && m_instances[m_playerIndex] == this)
        {
            m_instances[m_playerIndex] = NULL;
        }
    }

    Vec2d Ray_PlayerHudScoreComponent::getDefaultScale()
    {
        f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();
        return Vec2d(screenWidth * getTemplate()->getRelativeWidth(), screenWidth * getTemplate()->getRelativeWidth());
    }

    void Ray_PlayerHudScoreComponent::setForcedDisplay(u32 _playerIndex)
    {
        m_forcedDisplayMask |= 1<<_playerIndex;
    }


    void Ray_PlayerHudScoreComponent::clearSharedFlag(u32 &_mask)
    {
        if (m_playerIndex!=U32_INVALID)
        {
            _mask &= ~(1<<m_playerIndex);
        }
    }

    void  Ray_PlayerHudScoreComponent::setVisualOrder(u32 _order)
    {
        m_visualOrder=_order;
    }
    bbool Ray_PlayerHudScoreComponent::hasSharedFlag(u32 _mask)
    {
        if (m_playerIndex!=U32_INVALID)
        {
            return _mask & (1<<m_playerIndex);
        }
        return bfalse;
    }

    void Ray_PlayerHudScoreComponent::setPulseState(PulseState_t _state)
    {
        m_pulseState = _state;
        m_pulseTimer = 0;

        switch(_state)
        {
        case Pulse_Increase:
            m_timeFromPulseStart = 0;
            break;
        }
    }

    void Ray_PlayerHudScoreComponent::updatePulse(f32 _dt)
    {
        if (m_playerIndex == U32_INVALID)
            return;

        //reacting to the shared flag
        if (m_pulseState==Pulse_None)
        {
            m_pulseCount = processAddPendingLum(m_playerIndex);
            if (m_pulseCount)
            {
                setPulseState(Pulse_Increase);
            }
        }


        //updating scale
        const f32 normalScale = getTemplate()->getLumNormalScale();
        f32 targetAdditionalScale = normalScale;


        u32 reallyPulsingType = 0;

        if (m_captionType != Caption_Lum)
        {
            targetAdditionalScale = 0;
        }
        else
        {
            switch(m_pulseState)
            {
            case Pulse_Increase: 
                {
                    f32 lambda = m_pulseTimer/getTemplate()->getPulseIncreaseDuration();
                    lambda = std::min(lambda,1.f);
                    reallyPulsingType = (m_pulseCount==1)?1:2;
                    targetAdditionalScale = normalScale*(1-lambda)+lambda*getTemplate()->getPulseScale();
                    if (lambda==1)
                    {
                        if (m_pulseCount>1)
                        {
                            setPulseState(Pulse_DoublePeak);
                        }
                        else
                        {
                            setPulseState(Pulse_Sustain);
                        }
                    }
                } break;
            case Pulse_DoublePeak:
                {
                    f32 doublePeakDuration = getTemplate()->getPulseDoublePeakDuration();
                    f32 lambda = m_pulseTimer/doublePeakDuration;
                    reallyPulsingType = (m_pulseCount==1)?1:2;
                    lambda = std::min(lambda,1.f);
                    f32 scaleInterp = fabsf((lambda - 0.5f)*2.f);
                    targetAdditionalScale = getTemplate()->getPulseDoublePeakMinScale()*(1-scaleInterp)+scaleInterp*getTemplate()->getPulseScale();
                    if (lambda==1)
                    {
                        setPulseState(Pulse_Decrease);
                    }

                } break;
            case Pulse_Sustain:
                {
                    reallyPulsingType = (m_pulseCount==1)?1:2;
                    targetAdditionalScale = getTemplate()->getPulseScale(); 
                    if (m_pulseTimer>getTemplate()->getPulseSustainDuration())
                    {
                        setPulseState(Pulse_Decrease);
                    }
                } break;       
            case Pulse_Decrease:
                {
                    f32 duration;
                    duration = getTemplate()->getPulseDecreaseDuration();
                    f32 lambda = m_pulseTimer/duration;
                    lambda = std::min(lambda,1.f);
                    targetAdditionalScale = normalScale*lambda+(1-lambda)*getTemplate()->getPulseScale();
                    if (lambda==1)
                    {
                        setPulseState(Pulse_Wait);
                    }
                } break;

            case Pulse_Wait:
                {
                    f32 duration;
                    if (m_pulseCount>1)
                    {
                        duration = getTemplate()->getPulseCycleDuration_MultipleLums();
                    }
                    else
                    {
                        duration = getTemplate()->getPulseCycleDuration_SingleLum();
                    }
                    if (m_timeFromPulseStart>=duration)
                    {
                        setPulseState(Pulse_None);
                    }
                }
                break;
            }
        }

        m_reallyPulsingType[m_playerIndex] = reallyPulsingType;
        Vec2d scale = getDefaultScale();//*targetAdditionalScale;
        if(m_actor->getScale() != scale)
            m_actor->setScale(scale);
        m_scoreSizeMultiplier = targetAdditionalScale;

        m_currentLumScale = targetAdditionalScale;
        

        if (m_pulseState == Pulse_None)
        {
            return;
        }

        m_pulseTimer += _dt;
        m_timeFromPulseStart += _dt;


    }


    void Ray_PlayerHudScoreComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);
        
        m_lumAtlas.initialize(m_actor->getResourceGroup(),getTemplate()->getLumTexturePath(),1);
        m_characterAtlas.initialize(m_actor->getResourceGroup(),getTemplate()->getCharacterTexturePath(),1);
		m_relic1Atlas.initialize(m_actor->getResourceGroup(),getTemplate()->getRelic1TexturePath(),1);
		m_relic2Atlas.initialize(m_actor->getResourceGroup(),getTemplate()->getRelic2TexturePath(),1);

        TextInfo curText ("0", COLOR_WHITE, 10, StringID("score"), LocalisationId::Invalid);
        m_lumCountTextTab.clear();
        m_lumCountTextTab.push_back(curText);

		TextInfo relicText ("0", COLOR_WHITE, 10, StringID("relic"), LocalisationId::Invalid);
		m_relicTextTab.clear();
		m_relicTextTab.push_back(relicText);

        Player* myPlayer = GAMEMANAGER->getPlayerFromActor(m_actor->getRef(),bfalse);
        if ( myPlayer )
        {
            m_playerIndex = myPlayer->getIndex();
            m_visualOrder = m_playerIndex;
        }

        initPressStartText();

        m_pulseCount = 0;
        m_timeFromPulseStart = 0;

        setPulseState(Pulse_None);

        // Load the font
        if (!getTemplate()->getFontName().isEmpty())
            m_scoreFontIndex = GFX_ADAPTER->loadFont(getTemplate()->getFontName());

        if (!getTemplate()->getPressStartFontName().isEmpty())
            m_pressStartFontIndex = GFX_ADAPTER->loadFont(getTemplate()->getPressStartFontName());

         ACTOR_REGISTER_EVENT_COMPONENT(m_actor,ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152),this);
    }

    void Ray_PlayerHudScoreComponent::onEvent( Event* _event )
    {
        if ( EventPlayerIndexChange* playerIndex = _event->DynamicCast<EventPlayerIndexChange>(ITF_GET_STRINGID_CRC(EventPlayerIndexChange,2964720152)) )
        {
            m_playerIndex = playerIndex->getNewIndex();

            clearSharedFlag(m_forcedDisplayMask);
            m_instances[m_playerIndex] = this;
            ITF_ASSERT_CRASH(m_playerIndex<s_maxPlayers, "Invalid player id!");
            flushPendingScoreIncrements(m_playerIndex);

            initPressStartText();
        }
    }

    void Ray_PlayerHudScoreComponent::flushPendingScoreIncrements(u32 _playerIndex)
    {
        ITF_ASSERT_CRASH(_playerIndex<s_maxPlayers, "invalid player id");
        FixedArray<u32,maxPendingScoreIncrements> &incrementList = m_pendingScoreIncrements[_playerIndex];
        u32 listSize = incrementList.size();
        Ray_PersistentGameData_Score& score = RAY_GAMEMANAGER->getCurrentScore();
        for (u32 i=0; i<listSize; i++)
        {
            //We do not prepare the sum to avoid events based on increments
            score.setLums(_playerIndex, score.getLums(_playerIndex) + incrementList[i]);
        }
        m_reallyPulsingType[_playerIndex] = 0;
        incrementList.clear();        
    }

    void Ray_PlayerHudScoreComponent::maySendScoreMultiplierParticle(u32 _playerIndex, u32 _increment)
    {
        Ray_PlayerHudScoreComponent *instance = m_instances[_playerIndex];
        if (instance)
        {
            Actor * instanceActor= instance->GetActor();
            if (instanceActor)
            {
                FxBankComponent *bankComponent = instanceActor->GetComponent<FxBankComponent>();
                if (bankComponent)
                {
                    if (_increment == instance->getTemplate()->getScoreMultiplierValue())
                    {
                        u32 handle = bankComponent->playFx(instance->getTemplate()->getScoreMultiplierParticleID());
                        if (handle!=U32_INVALID)
                        {
                            Vec2d pos2d = getScorePosFromPlayerIndex(_playerIndex);
                            Vec3d pos3d;
                            f32 z = instanceActor->getDepth();
                            pos3d.setFromVec2d(pos2d, &z);
                            bankComponent->setFXPos(handle, pos3d);
                            bankComponent->stopFx(handle);
                        }
                    }
                }
            }
        }
    }

    void Ray_PlayerHudScoreComponent::clearAllLumScore()
    {
        for (u32 i=0; i<s_maxPlayers; i++)
        {
            Ray_PlayerHudScoreComponent *hud = m_instances[i];
            if (hud && (hud->m_playerIndex!=U32_INVALID))
            {
                hud->clearSharedFlag(m_forcedDisplayMask);
                flushPendingScoreIncrements(hud->m_playerIndex);
            }
        }
        Ray_PersistentGameData_Score& score = RAY_GAMEMANAGER->getCurrentScore();
        for (u32 i=0; i<s_maxPlayers; i++)
        {
            score.setLums(i, 0);
        }
    }

    void Ray_PlayerHudScoreComponent::addPendingScoreIncrement(u32 _playerIndex, u32 _increment)
    {
        ITF_ASSERT_CRASH(_playerIndex<s_maxPlayers, "invalid player id");
        
        FixedArray<u32,maxPendingScoreIncrements> &incrementList = m_pendingScoreIncrements[_playerIndex];

        //
        maySendScoreMultiplierParticle(_playerIndex, _increment);

        //
#ifdef _DONT_BUFFER_WHEN_PULSING
        ITF_ASSERT(maxPendingScoreIncrements==1);
        u32 incrementType = _increment==1?1:2;
        if (m_reallyPulsingType[_playerIndex] == incrementType)
        {
            processAddPendingLum(_playerIndex);
            incrementList.push_back(_increment);
            processAddPendingLum(_playerIndex);
            return; ////////////EXIT POINT
        }
        
        if (incrementList.size()>=maxPendingScoreIncrements)
        {
            processAddPendingLum(_playerIndex);
        }
#else
        if (incrementList.size()>=maxPendingScoreIncrements)
        {
            processAddPendingLum(_playerIndex);
        }
#endif
        incrementList.push_back(_increment);
    }

    //////////////////////////////////////////////////////////////////////////
    bbool Ray_PlayerHudScoreComponent::canShowLumCaption()
    {
        if (isWorldMap())
            return bfalse;
        if (!m_lumCaptionEnabled)
            return bfalse;
        if (RAY_GAMEMANAGER->getCurrentLevelName().GetValue()== ITF_GET_STRINGID_CRC(prologue,291121491))
            return bfalse;
        const StringID &mapConfigType = RAY_GAMEMANAGER->getMapConfigType(RAY_GAMEMANAGER->getCurrentLevelName());
        if (mapConfigType==s_MapConfig_Empty)
            return bfalse;
        return btrue;
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_PlayerHudScoreComponent::Draw2D()
    {
        f32 alpha = m_currentAlpha;
        if (alpha <= 0)
        {
            return;
        }


        const Vec2d& position = m_actor->get2DPos();
        const Vec2d& scale = m_actor->getScale();
        const Vec2d characterScale= scale*getTemplate()->getHeadBoxLocalSize();

		u32 fadedColor = Color(m_currentAlpha, 1,1,1).getAsU32();

		if(canShowLumCaption() || isWorldMap())
		{			
			m_characterAtlas.addQuad(0, position-Vec2d(0,characterScale.m_y), position, position+Vec2d(characterScale.m_x, 0), position+Vec2d(characterScale.m_x,-characterScale.m_y), MTH_EPSILON,fadedColor);
			m_characterAtlas.draw2d(UI_TEXTMANAGER->getScoreAnimDepthRank(), btrue);
		}
        
        Vec2d lumOffset(scale*getTemplate()->getLumBoxLocalPosition());
        Vec2d lumScale(scale*getTemplate()->getLumBoxLocalSize());

        AABB lumAABB(position+lumOffset, position+lumOffset+lumScale);
        lumAABB.ScaleFromCenter(Vec2d(m_currentLumScale,m_currentLumScale));

        if (canShowLumCaption())
        {
            m_lumAtlas.addQuad(0, lumAABB.getMin(), lumAABB.getMinXMaxY(), lumAABB.getMax(), lumAABB.getMaxXMinY(), MTH_EPSILON,fadedColor);
            m_lumAtlas.draw2d(UI_TEXTMANAGER->getScoreAnimDepthRank(), btrue);
        }

		if (canShowRelicCaption())
		{
			//Tarik::Fresco
			Vec2d relicPos = getRelicPos();
			Vec2d relicOffset(scale*getTemplate()->getRelicBoxLocalPosition());
			Vec2d relicScale(scale*getTemplate()->getRelicBoxLocalSize());
			AABB relicAABB(relicPos+relicOffset, relicPos+relicOffset+relicScale);
			u32 relicFadedColor = Color(1,1,1,1).getAsU32();

			if(!m_frescoId)
			{
				m_relic2Atlas.addQuad(0, relicAABB.getMin(), relicAABB.getMinXMaxY(), relicAABB.getMax(), relicAABB.getMaxXMinY(), MTH_EPSILON,relicFadedColor);
				m_relic2Atlas.draw2d(UI_TEXTMANAGER->getScoreAnimDepthRank(), btrue);
			}
			else
			{
				m_relic1Atlas.addQuad(0, relicAABB.getMin(), relicAABB.getMinXMaxY(), relicAABB.getMax(), relicAABB.getMaxXMinY(), MTH_EPSILON,relicFadedColor);
				m_relic1Atlas.draw2d(UI_TEXTMANAGER->getScoreAnimDepthRank(), btrue);
			}
		}
		//------

        f32 actorWidth = scale.m_x;
        f32 actorHeight = scale.m_y;

        Vec2d actorPos = position;
        actorPos.m_y -= actorHeight * 0.5f;

        //GFX_ADAPTER->draw2dBox(actorPos, actorWidth, actorHeight, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE, COLOR_WHITE);

        //compute width of the score
        f32 scoreWidth = getTemplate()->getScoreSize().m_x * actorWidth*m_scoreSizeMultiplier;
        f32 scoreHeight = getTemplate()->getScoreSize().m_y * actorHeight*m_scoreSizeMultiplier;
        f32 scoreBoxHeight = scoreHeight * 2;

        Vec2d scorePos = actorPos;
        scorePos.m_y += getTemplate()->getScorePos().m_y * actorHeight-scoreHeight*0.5f;
        scorePos.m_x += getTemplate()->getScorePos().m_x * actorWidth;

        Vec3d pos = scorePos.to3d();
        pos.m_z = 0.1f;

        FontCall fontcall;
        bbool canDrawFont = bfalse;
        

        if (alpha>0)
        {
            Vec2d size;
            Vec2d position = getTemplate()->getScorePosFromVisualOrder(m_visualOrder, getTemplate()->getPressToJoinRelativeAdditionalMargin(), bfalse, size);

            if (m_captionType==Caption_Join)
            {
                f32 pressAHeight = getTemplate()->getPressStartHeight() * actorHeight;
                m_pressStartTextTab[0].m_size = pressAHeight;
                fontcall.setAutoScale(bfalse);
                fontcall.setMode( FONT_ALIGN_LEFT );
                fontcall.setFont(GFX_ADAPTER->getFontByIndex(m_pressStartFontIndex));
                //pos.m_x -= size.m_x*0.5f;
                ((Vec2d&)pos) += getTemplate()->getPressStartPos()*size; 
                fontcall.setPosition(pos);
                fontcall.setCount(0);
                fontcall.setAppend(bfalse);
                f32 pressStartAlpha = getPressStartAlpha();
                u32 color = Color(pressStartAlpha, 1.f, 1.f,1.f).getAsU32();
                u32 shadowColor = Color(pressStartAlpha*pressStartAlpha,0,0,0).getAsU32();
                fontcall.setShadowColor(shadowColor);
                fontcall.setAutoScale(bfalse);
                fontcall.setShadowOffset(Vec2d( 3.5, 3.5 ));
                fontcall.setColor(color);
                m_pressStartTextTab[0].m_color = color;

                fontcall.setUseStyle(btrue);
                fontcall.setHeight(pressAHeight);
                fontcall.setBoxWidth((f32)GFX_ADAPTER->getScreenWidth());
                fontcall.setBoxHeight(pressAHeight*2);
                fontcall.setTextTab(&m_pressStartTextTab);
                fontcall.setTextTabStart(0);
                fontcall.setTextTabStop(U32_INVALID);
                fontcall.setBackGroundTexture(ResourceID::Invalid);
                fontcall.setBackGroundTextureStartUV(Vec2d::Zero);
                fontcall.setBackGroundTextureEndUV(Vec2d::Zero);
                fontcall.setBackGroundColour(COLOR_ZERO);
                fontcall.setTextureOffset2D(Vec2d::Zero);
                fontcall.setScale(1.f);
                fontcall.setUseBackground(bfalse);
                canDrawFont = btrue;
            }
            else if (m_captionType==Caption_Lum)
            {
                //position.m_x += size.m_;
                if (canShowLumCaption())
                {
                    m_lumCountTextTab[0].m_size = scoreHeight;

                    u32 shadowColor = Color(alpha*alpha*0.25f,0,0,0).getAsU32();                
                    fontcall.setMode( FONT_ALIGN_LEFT );
                    fontcall.setFont(GFX_ADAPTER->getFontByIndex(m_scoreFontIndex));
                    fontcall.setPosition(pos);
                    fontcall.setCount(0);
                    fontcall.setAppend(bfalse);
                    fontcall.setShadowColor(shadowColor);
                    fontcall.setShadowOffset(Vec2d( 3.5, 3.5 ));
                    fontcall.setAutoScale(bfalse);
                    u32 color = Color(alpha, getTemplate()->getScoreColor().m_r, getTemplate()->getScoreColor().m_g, getTemplate()->getScoreColor().m_b).getAsU32();
                    fontcall.setColor(color);
                    m_lumCountTextTab[0].m_color = color;

                    fontcall.setUseStyle(btrue);
                    fontcall.setHeight(scoreHeight);
                    fontcall.setBoxWidth(scoreWidth);
                    fontcall.setBoxHeight(scoreBoxHeight);
                    fontcall.setTextTab(&m_lumCountTextTab);
                    fontcall.setTextTabStart(0);
                    fontcall.setTextTabStop(U32_INVALID);
                    fontcall.setBackGroundTexture(ResourceID::Invalid);
                    fontcall.setBackGroundTextureStartUV(Vec2d::Zero);
                    fontcall.setBackGroundTextureEndUV(Vec2d::Zero);
                    fontcall.setBackGroundColour(COLOR_ZERO);
                    fontcall.setTextureOffset2D(Vec2d::Zero);
                    fontcall.setScale(1.f);
                    fontcall.setUseBackground(bfalse);
                    canDrawFont = btrue;
                }
            }
            if (canDrawFont)
            {
                i32 rank = UI_TEXTMANAGER->getScoreTextDepthRank();
                GFX_ADAPTER->drawTextWithStyle(&fontcall, rank);
            }

        }

		if (canShowRelicCaption())
		{
			initRelicText();

			fontcall.setMode( FONT_ALIGN_RIGHT );
			fontcall.setFont(GFX_ADAPTER->getFontByIndex(m_scoreFontIndex));
			//((Vec2d&)pos) += Vec2d(60.f,100.f); 
			f32 relicWidth = getTemplate()->getRelicSize().m_x * actorWidth;
			f32 relicHeight = getTemplate()->getRelicSize().m_y * actorHeight;
			f32 relicBoxHeight = relicHeight * 2;

			f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();
			f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();
			
			Vec2d relicTextPos;
			if (RAY_GAMEMANAGER->isTimeAttack())
			{
				relicTextPos = Vec2d(screenWidth*getTemplate()->getRelicTextPos().m_x, screenHeight*(getTemplate()->getRelicTextPos().m_y + 0.25f));
			}
			else
			{
				relicTextPos = Vec2d(screenWidth*getTemplate()->getRelicTextPos().m_x, screenHeight*getTemplate()->getRelicTextPos().m_y);
			}


			Vec3d relicPos3d = relicTextPos.to3d();
			relicPos3d.m_z = 0.1f;

			m_relicTextTab[0].m_size = relicHeight;
			fontcall.setPosition(relicPos3d);
			fontcall.setCount(0);
			fontcall.setAppend(bfalse);
			fontcall.setShadowColor(getTemplate()->getRelicTextShadowColor().getAsU32());
			fontcall.setShadowOffset(getTemplate()->getRelicTextShadowOffset());
			fontcall.setAutoScale(bfalse);
			f32 relicTextAlpha = 1.f;
			u32 color = Color(relicTextAlpha, getTemplate()->getRelicTextColor().m_r, getTemplate()->getRelicTextColor().m_g, getTemplate()->getRelicTextColor().m_b).getAsU32();
			fontcall.setColor(color);
			m_relicTextTab[0].m_color = color;

			fontcall.setUseStyle(btrue);
			fontcall.setHeight(relicHeight);
			fontcall.setBoxWidth(relicWidth);
			fontcall.setBoxHeight(relicBoxHeight);
			fontcall.setTextTab(&m_relicTextTab);
			fontcall.setTextTabStart(0);
			fontcall.setTextTabStop(U32_INVALID);
			fontcall.setBackGroundTexture(ResourceID::Invalid);
			fontcall.setBackGroundTextureStartUV(Vec2d::Zero);
			fontcall.setBackGroundTextureEndUV(Vec2d::Zero);
			fontcall.setBackGroundColour(COLOR_ZERO);
			fontcall.setTextureOffset2D(Vec2d::Zero);
			fontcall.setScale(1.f);
			fontcall.setUseBackground(bfalse);

			i32 rank = UI_TEXTMANAGER->getScoreTextDepthRank();
			GFX_ADAPTER->drawTextWithStyle(&fontcall, rank);
		}


    }

    Vec2d Ray_PlayerHudScoreComponent_Template::getScorePosFromVisualOrder( u32 _visualOrder, const Margin& _additionalRelativeMargin, const bbool _useWidth, Vec2d &_size ) const
    {
        f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();
        f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();

        _size.set(screenWidth * m_relativeWidth*m_actualWidthPercent, screenWidth * m_relativeWidth);
        f32 sizeWithSpacing(screenWidth * (m_relativeWidth+m_relativeSpacing)*m_actualWidthPercent);
        f32 width = _useWidth ? _size.m_x : 0.f;
        Vec2d safeMarginVec, safeSize;
        GFX_ADAPTER->computeSafeFrame(safeMarginVec, safeSize);
        Margin additionalMargin(_additionalRelativeMargin);
        additionalMargin.scale(screenWidth,screenHeight);
        Margin safeMargin(safeMarginVec.m_x, safeMarginVec.m_x, safeMarginVec.m_y, safeMarginVec.m_y);
        safeMargin.grow(additionalMargin);

        return Vec2d(safeMargin.m_left +_visualOrder*sizeWithSpacing + width, safeMargin.m_top + _size.m_y / 2);
    }

    Vec2d Ray_PlayerHudScoreComponent::getScorePosFromPlayerIndex( u32 _index )
    {
        if (_index >= s_maxPlayers)
        {
            return Vec2d::Zero;
        }

        Ray_PlayerHudScoreComponent* instance = m_instances[_index];
        if (!instance)
        {
            return Vec2d::Zero;
        }

        Vec2d dummySize;
        u32 visualOrder = instance->getVisualOrder();
        return instance->getTemplate()->getScorePosFromVisualOrder(visualOrder, instance->getTemplate()->getRelativeAdditionalMargin(), bfalse, dummySize);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    u32 Ray_PlayerHudScoreComponent::processAddPendingLum(u32 _activeIndex)
    {
        ITF_ASSERT_CRASH(_activeIndex<s_maxPlayers, "invalid index");

        FixedArray<u32,maxPendingScoreIncrements> &pendingList = m_pendingScoreIncrements[_activeIndex];
        if (pendingList.size())
        {
            u32 toAdd = pendingList[0];
            pendingList.eraseKeepOrder(0);
            Ray_PersistentGameData_Score& score = RAY_GAMEMANAGER->getCurrentScore();
            score.setLums(_activeIndex, score.getLums(_activeIndex) + toAdd);

            return toAdd;
        }
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    bbool Ray_PlayerHudScoreComponent::isWorldMap()
    {
        const StringID worldMapID(Ray_GameScreen_WorldMap::GetClassCRCStatic());
        return RAY_GAMEMANAGER->getCurrentGameScreen() == worldMapID;        
    }


    ///////////////////////////////////////////////////////////////////////////////////////////
    f32 Ray_PlayerHudScoreComponent::getPressStartAlpha()
    {
        f32 interpolator=0;
        if (m_pressStartPulseTimer>getTemplate()->getPressStartPulsePeriod())
            m_pressStartPulseTimer = 0;
        f32 t1 =getTemplate()->getPressStartFadeInDuration();
        if (m_pressStartPulseTimer<t1)
        {
            interpolator = m_pressStartPulseTimer/t1;
        }
        else
        {
            float t = m_pressStartPulseTimer-t1;
            interpolator = 1-t/getTemplate()->getPressStartFadeOutDuration();
        }

        interpolator = f32_Clamp(interpolator,0,1);
        f32 alpha = f32_Lerp(getTemplate()->getPressStartMinAlpha(), getTemplate()->getPressStartMaxAlpha(), interpolator);
        return alpha;            
    }

    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_PlayerHudScoreComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_playerIndex==U32_INVALID)
        {
            return;
        }

        Ray_Player* player = static_cast<Ray_Player*>(RAY_GAMEMANAGER->getPlayer(m_playerIndex));

        if ( !player )
        {
            return;
        }
        bbool isActivePlayer = player->getActive();
        bbool show;

        if ( player->getActive() )
        {
            show = btrue;
            m_captionType = Caption_Lum;
            
            if ( !m_lastActive )
            {
                m_stateChangeTimer = 0.f;
            }
        }
        else
        {
            if ( player->getOriginalPlayerIDInfoIndex() == GAMEMANAGER->getIndexNextAvailablePlayer() )
            {
                show = btrue;

                // no fade
                m_stateChangeTimer = getTemplate()->getHpChangeVisibleDuration() + getTemplate()->getHpChangeFadeInDuration();
            }
            else
            {
                show = bfalse;
            }

            m_captionType = Caption_Join;
        }

        if ( show )
        {
            // force-hide the hud: 
            //  - while loading / playing a cinematic
            //  - if the player cannot play this map
            //  - if the pad is disconnected
            //  - When global enabled flag is cleared
            if ( !m_isDrawEnabled || LOADINGSCREEN->isVisible() || CINEMATICMANAGER->getLockPlayers() || RAY_GAMEMANAGER->hideJoinMessage() )
            {
                show = bfalse;
            }
            else if ( RAY_GAMEMANAGER->getMaxPlayers() && m_playerIndex >= RAY_GAMEMANAGER->getMaxPlayers() )
            {
                show = bfalse;
            }
            else if ( !isActivePlayer )
            {
                if ( RAY_GAMEMANAGER->isSequencePlaying() )
                {
                    show = bfalse;
                }
                else
                {
                    // Find a pad that's connected but not playing
                    u32 numPlayers = GAMEMANAGER->getMaxPlayerCount();
                    bbool found = bfalse;

                    for ( u32 i = 0; i < numPlayers; i++ )
                    {
                        Player* p = GAMEMANAGER->getPlayer(i);

                        if ( !p->getActive() && INPUT_ADAPTER->isPadConnected(p->getIndex()) )
                        {
                            found = btrue;
                            break;
                        }
                    }

                    if ( !found )
                    {
                        show = bfalse;
                    }
                }
            }
        }

        show |= m_alwaysVisible;

        f32 alpha = m_currentAlpha;
        f32 targetAlpha;

        if (show)
        {
            if ((m_stateChangeTimer < getTemplate()->getHpChangeFadeInDuration() + getTemplate()->getHpChangeVisibleDuration()))
                targetAlpha = 1.f;
            else
                targetAlpha = getTemplate()->getIdleAlpha();
        }
        else
        {
            alpha = targetAlpha = 0;
        }

        if (targetAlpha>alpha)
        {
            alpha += _dt / getTemplate()->getHpChangeFadeInDuration();
            if (alpha>targetAlpha)
                alpha = targetAlpha;
        }
        else
        {
            alpha -= _dt / getTemplate()->getHpChangeFadeOutDuration();
            if (alpha<targetAlpha)
                alpha = targetAlpha;
        }
        alpha = Clamp(alpha, 0.f, 1.f);
        m_currentAlpha = alpha;

        updatePulse(_dt); // changes scale -> do it first

        Vec2d scoreSize;
        Vec2d position = getTemplate()->getScorePosFromVisualOrder(m_visualOrder, getTemplate()->getRelativeAdditionalMargin(), bfalse, scoreSize);
        m_actor->set2DPos(position);

        updateHpBar(_dt,player);
        updateLumCount(_dt);
        
        m_stateChangeTimer += _dt;
        m_pressStartPulseTimer += _dt;

        m_lastActive = player->getActive();



#if !defined(ITF_FINAL) && !defined(ITF_DEMO)
        if ( !show )
        {
            return;
        }

        if (player->getActive())
        {
            if ( player->getInvincible() && alpha > 0)
            {
                Vec2d scale = m_actor->getScale();
                Vec2d corner = m_actor->get2DPos();
                corner.m_x += m_actor->isFlipped() ? scale.m_x * -1.3f : scale.m_x * 0.7f;
                corner.m_y += scale.m_y * -0.4f;

                GFX_ADAPTER->drawDBGText("INVINCIBLE", corner.m_x, corner.m_y);
            }
        }

        //String s;
        //s.setTextFormat("%d / %d", m_player->getHitPoints(), m_player->getMaxHitPoints());
        //GFX_ADAPTER->drawDBGText(s);

        //const AnimFrameInfo& anim0 = m_animComponent->getAnimFrameInfo()[0];
        //const AnimFrameInfo& anim1 = m_animComponent->getAnimFrameInfo()[1];
        //s.setTextFormat("%.1f / %.1f", anim1.m_fFrame, anim0.m_fFrame);
        //GFX_ADAPTER->drawDBGText(s);
#endif
    }

    void Ray_PlayerHudScoreComponent::updateLumCount(f32 _dt)
    {
        if (m_playerIndex==U32_INVALID)
        {
            return;
        }
        f32 dest = (f32)RAY_GAMEMANAGER->getCurrentScore().getLums(m_playerIndex);

        bbool changed=bfalse;

#ifdef _INCREASE_PROGRESSIVELY
        if (m_lastLumCount<dest)
        {
            f32 progThreshold = dest-getTemplate()->getLumProgressiveBigIncreaseThreshold();
            if (m_lastLumCount<progThreshold)
            {
                m_lastLumCount = progThreshold;
            }
            else
            {
                f32 progSmallThreshold = dest-getTemplate()->getLumProgressiveSmallIncreaseThreshold();

                f32 increase = 0;
                if (m_lastLumCount<progSmallThreshold)
                {
                    increase = _dt*getTemplate()->getLumBigIncreaseSpeed();;
                }
                else
                {
                    increase = _dt*getTemplate()->getLumSmallIncreaseSpeed();
                }
                /*if ((dest-m_lastLumCount)<=1 && increase<1)
                {
                    m_lastLumCount = dest;
                }
                else
                */
                {
                    m_lastLumCount += increase;
                }
            }
            changed=btrue;
        }
        if (m_lastLumCount>dest)
        {
            m_lastLumCount = dest;
            changed=btrue;
        }
#else
        if (m_lastLumCount != dest)
        {
            m_lastLumCount = dest;
            changed = btrue;
        }
#endif

        if (changed || hasSharedFlag(m_forcedDisplayMask))
        {
            m_stateChangeTimer = 0;
            m_lumCountTextTab[0].m_text.setTextFormat("%d", (int)m_lastLumCount);
            clearSharedFlag(m_forcedDisplayMask);
        }

    }

    //-------------------------------------------------------------------------------------
    // gs_pressStartText is not really used
    constexpr const char* gs_pressStartText[] =
    {
        "<N/A>",                        // Pad_Other = 0,
        "[icon:X360_BUTTON_A]",         // Pad_X360,
        "[icon:PS3_BUTTON_CROSS]",      // Pad_PS3,
        "[icon:WII_BUTTON_2]",          // Pad_WiiSideWay,
        "[icon:WII_BUTTON_A]",          // Pad_WiiNunchuk,
        "[icon:WII_CLASSIC_BUTTON_B]",  // Pad_WiiClassic,
        "<N/A>",                        // Pad_Vita,
        "<N/A>",                        // Pad_CTR,
        "<N/A>",                        // Pad_PS5,
        "<N/A>",                        // Pad_NX_Joycon,
        "<N/A>",                        // Pad_NX_Joycon_Dual,
        "<N/A>",                        // Pad_NX_Pro,
        "<N/A>"                         // Pad_GenericXBox,
    };

    static_assert(ITF_ARRAY_SIZE(gs_pressStartText) == ITF::uSize(InputAdapter::PadType_Count));


    void Ray_PlayerHudScoreComponent::initPressStartText()
    {
        String8 pressStartText;
        if (m_playerIndex!=U32_INVALID)
        {
            InputAdapter::PadType padType = INPUT_ADAPTER->getDebugInputPadType(m_playerIndex);
            pressStartText = gs_pressStartText[padType];
        }

        LocalisationId textId = getTemplate()->getPressStartTextID();/*3873*/
        TextInfo pressStart (LOCALISATIONMANAGER->getText(textId).cStr(), COLOR_WHITE, 10, StringID("PressToStart"), LocalisationId::Invalid);
        //TextInfo pressStart (pressStartText, COLOR_WHITE, 10, StringID("PressToStart"), U32_INVALID);
        m_pressStartTextTab.clear();
        m_pressStartTextTab.push_back(pressStart);
    }

    void Ray_PlayerHudScoreComponent::updateHpBar(f32 _dt, Ray_Player* _player)
    {
        if (!_player)
            return;

        // max HP
        f32 maxHpTargetFrame = (f32)_player->getMaxHitPoints();
        if (m_currentMaxHpFrame < maxHpTargetFrame)
        {
            m_currentMaxHpFrame += _dt * m_growSpeed;
            if (m_currentMaxHpFrame > maxHpTargetFrame)
                m_currentMaxHpFrame = maxHpTargetFrame;
        }
        else if (m_currentMaxHpFrame > maxHpTargetFrame)
        {
            m_currentMaxHpFrame -= _dt * m_growSpeed;
            if (m_currentMaxHpFrame < maxHpTargetFrame)
                m_currentMaxHpFrame = maxHpTargetFrame;
        }

        // HP
        f32 hpTargetFrame = (f32)_player->getHitPoints();
        if (m_currentHpFrame < hpTargetFrame)
        {
            m_currentHpFrame += _dt * m_growSpeed;
            if (m_currentHpFrame > hpTargetFrame)
                m_currentHpFrame = hpTargetFrame;
        }
        else if (m_currentHpFrame > hpTargetFrame)
        {
            m_currentHpFrame -= _dt * m_growSpeed;
            if (m_currentHpFrame < hpTargetFrame)
                m_currentHpFrame = hpTargetFrame;
        }

        // don't fade out until we're done playing (useful for player activation: we fill a lot of HP at once)
        if (m_currentMaxHpFrame != maxHpTargetFrame ||
            m_currentHpFrame != hpTargetFrame)
        {
            m_stateChangeTimer = 0;
        }

    }

    //-------------------------------------------------------------------------------------
    IMPLEMENT_OBJECT_RTTI(Ray_PlayerHudScoreComponent_Template)
    BEGIN_SERIALIZATION_CHILD(Ray_PlayerHudScoreComponent_Template)
        SERIALIZE_MEMBER("relativeWidth", m_relativeWidth);
        SERIALIZE_MEMBER("relativeSpacing", m_relativeSpacing);
        SERIALIZE_MEMBER("actualWidthPercent", m_actualWidthPercent);
        SERIALIZE_OBJECT("relativeAdditionalMargin", m_relativeAdditionalMargin);
        SERIALIZE_OBJECT("pressToJoinRelativeAdditionalMargin", m_pressToJoinRelativeAdditionalMargin);
        SERIALIZE_MEMBER("fadeInDuration", m_hpChangeFadeInDuration);
        SERIALIZE_MEMBER("visibleDuration", m_hpChangeVisibleDuration);
        SERIALIZE_MEMBER("fadeOutDuration", m_hpChangeFadeOutDuration);
        SERIALIZE_MEMBER("fontName", m_fontName);
        SERIALIZE_MEMBER("scorePos", m_scorePos);
        SERIALIZE_MEMBER("scoreSize", m_scoreSize);
        SERIALIZE_MEMBER("scoreColor", m_scoreColor);
        SERIALIZE_MEMBER("lumBigIncreaseSpeed", m_lumBigIncreaseSpeed);
        SERIALIZE_MEMBER("lumProgressiveBigIncreaseThreshold", m_lumProgressiveBigIncreaseThreshold);
        SERIALIZE_MEMBER("lumSmallIncreaseSpeed", m_lumSmallIncreaseSpeed);
        SERIALIZE_MEMBER("lumProgressiveSmallIncreaseThreshold", m_lumProgressiveSmallIncreaseThreshold);
        SERIALIZE_MEMBER("pulseSustainDuration", m_pulseSustainDuration);
        SERIALIZE_MEMBER("lumNormalScale", m_lumNormalScale);
        SERIALIZE_MEMBER("pulseScale", m_pulseScale);
        SERIALIZE_MEMBER("pulseIncreaseDuration", m_pulseIncreaseDuration);
        SERIALIZE_MEMBER("pulseDecreaseDuration", m_pulseDecreaseDuration);
        SERIALIZE_MEMBER("pulseDoublePeakDuration", m_pulseDoublePeakDuration);
        SERIALIZE_MEMBER("pulseDoublePeakMinScale", m_pulseDoublePeakMinScale);
        SERIALIZE_MEMBER("pulseCycleDuration_SingleLum", m_pulseCycleDuration_SingleLum);
        SERIALIZE_MEMBER("pulseCycleDuration_MultipleLums", m_pulseCycleDuration_MultipleLums);
        SERIALIZE_MEMBER("scoreMultiplierParticleID", m_scoreMultiplierParticleID);
        SERIALIZE_MEMBER("scoreMultiplierValue", m_scoreMultiplierValue);
        SERIALIZE_MEMBER("pressStartHeight", m_pressStartHeight);
        SERIALIZE_MEMBER("pressStartPos", m_pressStartPos);
        SERIALIZE_MEMBER("pressStartFontName", m_pressStartFontName);
        SERIALIZE_MEMBER("grayedAlpha", m_grayedAlpha);
        SERIALIZE_MEMBER("idleAlpha", m_idleAlpha);
        SERIALIZE_MEMBER("pressStartPulsePeriod", m_pressStartPulsePeriod);
        SERIALIZE_MEMBER("pressStartFadeInDuration", m_pressStartFadeInDuration);
        SERIALIZE_MEMBER("pressStartFadeOutDuration", m_pressStartFadeOutDuration);
        SERIALIZE_MEMBER("pressStartMinAlpha", m_pressStartMinAlpha);
        SERIALIZE_MEMBER("pressStartMaxAlpha", m_pressStartMaxAlpha);
        SERIALIZE_MEMBER("pressStartTextID", m_pressStartTextID);
        SERIALIZE_MEMBER("lumTexture", m_lumTexturePath);
        SERIALIZE_MEMBER("characterTexture", m_characterTexturePath);
        SERIALIZE_MEMBER("lumBoxLocalPosition", m_lumBoxLocalPosition);
        SERIALIZE_MEMBER("lumBoxLocalSize", m_lumBoxLocalSize);
        SERIALIZE_MEMBER("headBoxLocalSize", m_headBoxLocalSize);

		SERIALIZE_MEMBER("relicPos", m_relicPos);
		SERIALIZE_MEMBER("relicTextPos", m_relicTextPos);
		SERIALIZE_MEMBER("relicSize", m_relicSize);
		SERIALIZE_MEMBER("relic1Texture", m_relic1TexturePath);
		SERIALIZE_MEMBER("relic2Texture", m_relic2TexturePath);
		SERIALIZE_MEMBER("relicBoxLocalPosition", m_relicBoxLocalPosition);
		SERIALIZE_MEMBER("relicBoxLocalSize", m_relicBoxLocalSize);
		SERIALIZE_MEMBER("newRelicTextID", m_newRelicTextID);
		SERIALIZE_MEMBER("duplicateRelicTextID", m_duplicateRelicTextID);
		SERIALIZE_MEMBER("relicTextColor", m_relicTextColor);
		SERIALIZE_MEMBER("relicTextShadowOffset", m_relicTextShadowOffset);
		SERIALIZE_MEMBER("relicTextShadowColor", m_relicTextShadowColor);
    END_SERIALIZATION()

    Ray_PlayerHudScoreComponent_Template::Ray_PlayerHudScoreComponent_Template()
    : m_relativeWidth(0.1f)
    , m_actualWidthPercent(1.f)
    , m_hpChangeFadeInDuration(1)
    , m_hpChangeVisibleDuration(1)
    , m_hpChangeFadeOutDuration(1)
    , m_lumProgressiveBigIncreaseThreshold(50)
    , m_lumProgressiveSmallIncreaseThreshold(50)
    , m_lumBigIncreaseSpeed(10)
    , m_lumSmallIncreaseSpeed(10)
    , m_scorePos(Vec2d::Zero)
    , m_scoreSize(Vec2d::One)
    , m_scoreColor(Color::white())
    , m_pulseSustainDuration(0.1f)
    , m_pulseScale(1.2f)
    , m_pulseDoublePeakMinScale(1.1f)
    , m_pulseIncreaseDuration(0.1f)
    , m_pulseDecreaseDuration(0.1f)
    , m_pulseDoublePeakDuration(0.1f)
    , m_pulseCycleDuration_SingleLum(0.2f)
    , m_pulseCycleDuration_MultipleLums(0.3f)
    , m_scoreMultiplierParticleID("FX_HUD_X4_1")
    , m_scoreMultiplierValue(2)
    , m_lumNormalScale(1.f)
    , m_pressStartHeight(0.3f)
    , m_pressStartPos(0,-0.25f)
    , m_grayedAlpha(0.5f)
    , m_idleAlpha(0.2f)
    , m_pressStartPulsePeriod(10)
    , m_pressStartFadeInDuration(1)
    , m_pressStartFadeOutDuration(1)
    , m_pressStartMinAlpha(0.2f)
    , m_pressStartMaxAlpha(1.f)
    , m_relativeSpacing(0.01f)
    , m_lumBoxLocalPosition(0,0)
    , m_lumBoxLocalSize(0,0)
    , m_headBoxLocalSize(0.8f,0.8f)

	, m_relicPos(Vec2d::Zero)
	, m_relicTextPos(Vec2d::Zero)
	, m_relicSize(Vec2d::One)
	, m_relicBoxLocalPosition(0,0)
	, m_relicBoxLocalSize(0,0)
	, m_relicTextColor(Color::white())
	, m_relicTextShadowOffset(Vec2d(3.5f,3.5f))
	, m_relicTextShadowColor(COLOR_BLACK)
   {
    }

	///////////////////////////////////////////////////////////////////////////////////////////
	//Tarik::Fresco
	Vec2d Ray_PlayerHudScoreComponent::getRelicPos( )
	{
		Ray_PlayerHudScoreComponent* instance = m_instances[0];
		if (!instance)
		{
			return Vec2d::Zero;
		}

		f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();
		f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();

		 if (RAY_GAMEMANAGER->isTimeAttack())
		 {
			return Vec2d(screenWidth*instance->getTemplate()->getRelicPos().m_x, screenHeight*(instance->getTemplate()->getRelicPos().m_y + 0.25f));
		 }
		 else
		 {
			 return Vec2d(screenWidth*instance->getTemplate()->getRelicPos().m_x, screenHeight*instance->getTemplate()->getRelicPos().m_y);
		 }

	}

	//////////////////////////////////////////////////////////////////////////
	bbool Ray_PlayerHudScoreComponent::canShowRelicCaption()
	{
		if (isWorldMap())
			return bfalse;
		if (!m_relicCaptionEnabled)
			return bfalse;
		if (RAY_GAMEMANAGER->getCurrentLevelName().GetValue()== ITF_GET_STRINGID_CRC(prologue,291121491))
			return bfalse;
		const StringID &mapConfigType = RAY_GAMEMANAGER->getMapConfigType(RAY_GAMEMANAGER->getCurrentLevelName());
		if (mapConfigType==s_MapConfig_Empty)
			return bfalse;
		return btrue;
	}

	void Ray_PlayerHudScoreComponent::initRelicText()
	{
		String8 pressStartText;

		
		//TextInfo relicText (LOCALISATIONMANAGER->getText(textId).cStr(), COLOR_WHITE, 10, StringID("PressToStart"), LocalisationId::Invalid);
		TextInfo relicText;
		if(m_isNewCaption)
		{
			LocalisationId textId = getTemplate()->getNewRelicTextID();/*4620*/
			relicText = TextInfo (LOCALISATIONMANAGER->getText(textId).cStr(), COLOR_WHITE, 10, StringID("New"), LocalisationId::Invalid);
		}
		else
		{
			LocalisationId textId = getTemplate()->getDuplicateRelicTextID();/*4621*/
			relicText = TextInfo (LOCALISATIONMANAGER->getText(textId).cStr(), COLOR_WHITE, 10, StringID("Duplicate"), LocalisationId::Invalid);
		}

		m_relicTextTab.clear();
		m_relicTextTab.push_back(relicText);
	}
};
