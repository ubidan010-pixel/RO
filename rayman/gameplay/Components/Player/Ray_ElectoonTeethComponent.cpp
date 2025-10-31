#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_ELECTOONTEETHCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_ElectoonTeethComponent.h"
#endif //_ITF_RAY_ELECTOONTEETHCOMPONENT_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_GFX_ADAPTER_H_
#include "engine/AdaptersInterfaces/GFXAdapter.h"
#endif //_ITF_GFX_ADAPTER_H_

#ifndef _ITF_LOADINGSCREEN_H_
#include "engine/loading/loadingscreen.h"
#endif //_ITF_LOADINGSCREEN_H_


#ifndef _ITF_VIEW_H_
#include "engine/display/View.h"
#endif //_ITF_VIEW_H_

#ifndef _ITF_SOUNDCOMPONENT_H_
#include "engine/actors/components/SoundComponent.h"
#endif //_ITF_SOUNDCOMPONENT_H_

//#define MOVEMENT_DURATION 1.f

#define ELECTOON_INCREASE_SOUNDID   ITF_GET_STRINGID_CRC(electoonCountIncreased,3501604895)
#define TOOTH_INCREASE_SOUNDID      ITF_GET_STRINGID_CRC(toothCountIncreased,807041642)
#define ELECTOON_FLIGHT_SOUNDID     ITF_GET_STRINGID_CRC(electoonFlight,442763701)
#define TOOTH_FLIGHT_SOUNDID        ITF_GET_STRINGID_CRC(toothFlight,736675992)

namespace ITF
{    
    IMPLEMENT_OBJECT_RTTI(Ray_ElectoonTeethComponent)

    BEGIN_SERIALIZATION_CHILD(Ray_ElectoonTeethComponent)
    END_SERIALIZATION()

    BEGIN_VALIDATE_COMPONENT(Ray_ElectoonTeethComponent)
    END_VALIDATE_COMPONENT()

    Ray_ElectoonTeethComponent *Ray_ElectoonTeethComponent::s_instance = NULL;

    Ray_ElectoonTeethComponent::Ray_ElectoonTeethComponent()
        : Super()
        , m_lastElectoonCount(0)
        , m_lastToothCount(0)
        , m_nextElectoonCount(0)
        , m_nextToothCount(0)
        , m_scoreFontIndex(0)
        , m_stateChangeTimer_Tooth(MTH_HUGE)
        , m_stateChangeTimer_Electoon(MTH_HUGE)
        , m_alpha(1.f)
        , m_electoonScale(1.f)
        , m_toothScale(1.f)
        , m_isActive(btrue)
        , m_soundComponent(NULL)
    {
    }

    Ray_ElectoonTeethComponent::~Ray_ElectoonTeethComponent()
    {
        s_instance = NULL;
        m_atlas.clear();
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonToothScore,116023213), this);
        EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventFetchElectoonToothScoreInfos,3242513602), this);
    }

    Ray_ElectoonTeethComponent::MovingIcon::MovingIcon(u8 _icon, const Vec3d &_startPos, const Vec2d &_destination2D, f32 _normalSize, f32 _nearZ, f32 _centerExageration, f32 _timeOffset, f32 _flightDuration)
        : m_icon(_icon)
        , m_time(_timeOffset)
        , m_flightDuration(_flightDuration)
        , m_isInVisiblePhase(bfalse)
        , m_justAppeared(bfalse)
    {
        GFX_ADAPTER->compute3DTo2D(_startPos, m_startPosProj);
        m_endPosProj = _destination2D.to3d(m_startPosProj.m_z);
        Vec3d endPos3d;
        GFX_ADAPTER->compute2DTo3D(m_endPosProj, endPos3d);
        Vec3d centerPos3d = CAMERA->getPos();//(endPos3d+_startPos)*0.5f;
        centerPos3d.m_z = CAMERA->getZ()-_nearZ;
        GFX_ADAPTER->compute3DTo2D(centerPos3d, m_centerPosProj);

        f32 invZ = 1/(CAMERA->getZ()-_startPos.m_z);
        m_sizeMultiplier = _normalSize/invZ;
        m_projPos = m_startPosProj;
        m_currentSize = _normalSize;


        Vec2d exageratedCenter = computeExageratedCenterPoint(m_startPosProj, m_endPosProj, _centerExageration);

        m_centerPosProj.m_x = exageratedCenter.m_x;
        m_centerPosProj.m_y = exageratedCenter.m_y;
    }

    Vec2d Ray_ElectoonTeethComponent::MovingIcon::computeExageratedCenterPoint(const Vec3d &_projectedStart, const Vec3d &_projectedEnd, f32 _exageration)
    {
        Vec2d delta((_projectedEnd-_projectedStart).truncateTo2D());
        Vec2d normal = -delta.getPerpendicular();
        normal.normalize();
        Vec2d center(GFX_ADAPTER->getScreenWidth()*0.5f, GFX_ADAPTER->getScreenHeight()*0.5f);
        f32 proj = (center-_projectedStart.truncateTo2D()).dot(normal);
        
        if (proj<0)
        {
            normal = -normal;
        }
        f32 dist = GFX_ADAPTER->getScreenWidth()*_exageration;
        if (proj<dist)
            return center+normal*dist;
        else
            return center;
    }

    bbool Ray_ElectoonTeethComponent::MovingIcon::update(f32 _dt)
    {        
        m_time += _dt;

        f32 lambda = m_time/m_flightDuration;
        lambda = f32_Clamp(lambda,0,1);
        bbool wasVisible = m_isInVisiblePhase;
        m_isInVisiblePhase = m_time>0;
        m_justAppeared = !wasVisible && m_isInVisiblePhase;

        Vec3d direction = m_centerPosProj-m_startPosProj;
        Vec3d line1Pos = m_startPosProj + direction*2.f*lambda;
        Vec3d line2PosStart = m_startPosProj + direction;
        Vec3d line2Pos = line2PosStart + (m_endPosProj-line2PosStart)*lambda;

        f32 interp = computeInterpolator(lambda);

        Vec3d projPos = line1Pos*(1-interp)+line2Pos*interp;
        projPos.m_z = f32_Clamp(projPos.m_z, 0.0001f, 0.9999f);
        Vec3d currentPos;
        GFX_ADAPTER->compute2DTo3D(projPos, currentPos);
        f32 invZ = 1.f/(CAMERA->getZ()-currentPos.m_z);
        m_currentSize = m_sizeMultiplier*invZ;
        m_projPos = projPos;
        return lambda>=1;
    }

    void Ray_ElectoonTeethComponent::MovingIcon::draw(AtlasDynamicObject &_atlas)
    {
        const Vec2d &pos = (const Vec2d&) m_projPos;
        Vec2d halfDiagonal(m_currentSize, m_currentSize);
        AABB aabb(pos-halfDiagonal, pos+halfDiagonal);
        f32 alpha=1;
        if (m_time<-LOGICDT*4) //We want to display at lambda 0 for 1st frames
            return;
        _atlas.addQuad(m_icon, aabb.getMin(),aabb.getMinXMaxY(), aabb.getMax(), aabb.getMaxXMinY(), 0.001f,Color(alpha,1,1,1).getAsU32());
    }

    void Ray_ElectoonTeethComponent::addNewIcon(u8 _icon, const Vec3d &_startPos3d, f32 _timeOffset)
    {
        Vec3d projPos;
        AABB electoonAABB,toothAABB;

        GFX_ADAPTER->compute2DTo3D(_startPos3d, projPos);
        
        computeScoreItemAABB(electoonAABB, toothAABB);
        Vec2d dest2d;
        switch(_icon)
        {
        case icon_Electoon: dest2d = electoonAABB.getCenter(); break;
        default: dest2d = toothAABB.getCenter(); break;
        }

        f32 normalSize = getTemplate()->getRelativeNormalSize()*GFX_ADAPTER->getScreenWidth();

        f32 exageration = getTemplate()->getMovementCurveDistance();
        MovingIcon newIcon(_icon, _startPos3d, dest2d, normalSize, getTemplate()->getNearestRelativeZ(), exageration, _timeOffset, getTemplate()->getFlightDuration());
        m_movingIcons.push_back(newIcon);
    }
    void Ray_ElectoonTeethComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
    {
        Super::onActorLoaded(_hotReload);

        m_stateChangeTimer_Tooth = MTH_HUGE;
        m_stateChangeTimer_Electoon = MTH_HUGE;

        m_soundComponent = m_actor->GetComponent<SoundComponent>();

        u32 electoonDest = RAY_GAMEMANAGER->getWMLastElectoonCount();
        u32 toothDest = RAY_GAMEMANAGER->getWMLastToothCount();

        if (electoonDest == U32_INVALID)
        {
            electoonDest = RAY_GAMEMANAGER->getElectoonsCount();
        }
        if (toothDest == U32_INVALID)
        {
            toothDest = RAY_GAMEMANAGER->getTotalTakenToothCount();
        }

        m_lastToothCount = (f32)toothDest;
        m_nextToothCount = (u32)toothDest;
        m_lastElectoonCount = (f32)electoonDest;
        m_nextElectoonCount = (u32)electoonDest;
        m_atlas.initialize(m_actor->getResourceGroup(),getTemplate()->getAtlasPath(),32);

        TextInfo curText ("0", COLOR_WHITE, 10, StringID("score"), LocalisationId::Invalid);
        m_electoonTextTab.clear();
        m_electoonTextTab.push_back(curText);
        m_toothTextTab.clear();
        m_toothTextTab.push_back(curText);

        m_electoonTextTab[0].m_text.setTextFormat("%d", (int)m_lastElectoonCount);
        m_toothTextTab[0].m_text.setTextFormat("%d", (int)m_lastToothCount);

        m_isActive = bfalse;

        // Load the font
        if (!getTemplate()->getFontName().isEmpty())
            m_scoreFontIndex = GFX_ADAPTER->loadFont(getTemplate()->getFontName());

        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonToothScore,116023213),this);
        EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventFetchElectoonToothScoreInfos,3242513602),this);

        s_instance = this;
    }

    void Ray_ElectoonTeethComponent::onEvent( Event* _event )
    {
        if (Ray_EventShowElectoonToothScore *showScore = _event->DynamicCast<Ray_EventShowElectoonToothScore>(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonToothScore,116023213)))
        {
            addMissingIcons(showScore->getSourcePos());
            m_isActive = showScore->getShow();

            // notify game manager
            showScore->setReceived(btrue);
        }
        else if (Ray_EventFetchElectoonToothScoreInfos *fetch = _event->DynamicCast<Ray_EventFetchElectoonToothScoreInfos>(ITF_GET_STRINGID_CRC(Ray_EventFetchElectoonToothScoreInfos,3242513602)))
        {
            fetch->setHasIconsInFlow(m_movingIcons.size()!=0);
        }
    }

    void Ray_ElectoonTeethComponent::computeScoreItemAABB(AABB &_electoonAABB, AABB &_toothAABB)
    {
        Vec2d safeMarginVec, safeSize;
        GFX_ADAPTER->computeSafeFrame(safeMarginVec, safeSize);
        //compute width of the score
        f32 panelX= getTemplate()->getXFromRight() * GFX_ADAPTER->getScreenWidth();
        f32 panelY = getTemplate()->getYFromTop() * GFX_ADAPTER->getScreenHeight();
        f32 panelItemWidth = getTemplate()->getItemWidth()*GFX_ADAPTER->getScreenWidth();
        f32 panelItemHeight = getTemplate()->getItemHeight()*GFX_ADAPTER->getScreenWidth();
        f32 iconSpacing = getTemplate()->getSpacing()*GFX_ADAPTER->getScreenWidth();

        Vec2d topRight(safeMarginVec+Vec2d(safeSize.m_x-panelX, panelY));
        Vec2d topLeft2ndIcon(topRight-Vec2d(panelItemWidth, 0));
        Vec2d topRight1stIcon(topLeft2ndIcon-Vec2d(iconSpacing,0));
        Vec2d topLeft1stIcon(topRight1stIcon-Vec2d(panelItemWidth,0));

        _electoonAABB = AABB(topLeft1stIcon, topLeft1stIcon+Vec2d(panelItemWidth,panelItemHeight));
        _toothAABB = AABB(topLeft2ndIcon, topLeft2ndIcon+Vec2d(panelItemWidth,panelItemHeight));
        _electoonAABB.ScaleFromCenter(Vec2d(m_electoonScale, m_electoonScale));
        _toothAABB.ScaleFromCenter(Vec2d(m_toothScale, m_toothScale));

    }

    void Ray_ElectoonTeethComponent::Draw2D()
    {
        if (m_alpha<=0)
            return;
       

        m_atlas.clearVertices();
        Texture *atlasTexture = m_atlas.getAtlasTexture();
        if (!atlasTexture)
            return;

        AABB electoonAABB, toothAABB;
        computeScoreItemAABB(electoonAABB, toothAABB);

        m_atlas.addQuad(0, electoonAABB.getMin(),electoonAABB.getMinXMaxY(), electoonAABB.getMax(), electoonAABB.getMaxXMinY(), 0.001f,Color(m_alpha,1,1,1).getAsU32());
        m_atlas.addQuad(1, toothAABB.getMin(),toothAABB.getMinXMaxY(), toothAABB.getMax(), toothAABB.getMaxXMinY(), 0.001f,Color(m_alpha,1,1,1).getAsU32());
        u32 iconCount = m_movingIcons.size();
        for (u32 i=0; i<iconCount; i++)
        {
            m_movingIcons[i].draw(m_atlas);
        }
        m_atlas.draw2d(UI_TEXTMANAGER->getAnimDepthRank(), btrue);

        displayScore(m_electoonTextTab, getTemplate()->getElectoonScorePos(), getTemplate()->getElectoonScoreHeight(), (u32)m_lastElectoonCount);
        displayScore(m_toothTextTab, getTemplate()->getToothScorePos(), getTemplate()->getToothScoreHeight(), (u32)m_lastToothCount);
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_ElectoonTeethComponent::displayScore(ITF_VECTOR<TextInfo> &_textTab, const Vec2d & _scorePos, f32 _height, u32 _count)
    {
        Vec2d scorePosScaled(_scorePos*(f32)GFX_ADAPTER->getScreenWidth());

        Vec2d safeMarginVec, safeSize;
        GFX_ADAPTER->computeSafeFrame(safeMarginVec, safeSize);
        scorePosScaled = safeMarginVec+Vec2d(safeSize.m_x,0)+Vec2d(-scorePosScaled.m_x, scorePosScaled.m_y);
        _height *= GFX_ADAPTER->getScreenWidth();

        FontCall fontcall;

        _textTab[0].m_size = _height;
        fontcall.setMode( FONT_ALIGN_LEFT );
        fontcall.setFont(GFX_ADAPTER->getFontByIndex(m_scoreFontIndex));
        fontcall.setPosition(scorePosScaled.to3d(0.001f));
        fontcall.setCount(0);
        fontcall.setAppend(0);
        const Color &scoreColor = getTemplate()->getScoreColor();
        u32 color = Color(m_alpha, scoreColor.m_r,scoreColor.m_g,scoreColor.m_b).getAsU32();
        u32 shadowColor = Color(m_alpha*m_alpha, 0,0,0).getAsU32();
        fontcall.setColor(color);
        _textTab[0].m_color = color;
        fontcall.setUseStyle(btrue);
        fontcall.setHeight(_height);
        fontcall.setBoxWidth((f32)GFX_ADAPTER->getScreenWidth());
        fontcall.setBoxHeight(_height*2);
        fontcall.setTextTab(&_textTab);
        fontcall.setTextTabStart(0);
        fontcall.setTextTabStop(U32_INVALID);
        fontcall.setBackGroundTexture(ResourceID::Invalid);
        fontcall.setBackGroundTextureStartUV(Vec2d::Zero);
        fontcall.setBackGroundTextureEndUV(Vec2d::Zero);
        fontcall.setBackGroundColour(COLOR_ZERO);
        fontcall.setTextureOffset2D(Vec2d::Zero);
        fontcall.setScale(1.f);
        fontcall.setShadowColor(shadowColor);
        fontcall.setAutoScale(bfalse);
        fontcall.setShadowOffset(Vec2d( 3.5, 3.5 ));
        fontcall.setUseBackground(bfalse);
        i32 rank = UI_TEXTMANAGER->getScoreTextDepthRank();
        GFX_ADAPTER->drawTextWithStyle(&fontcall, rank);

    }
    static bbool test=bfalse;
    ///////////////////////////////////////////////////////////////////////////////////////////
    void Ray_ElectoonTeethComponent::Update( f32 _dt )
    {
        Super::Update(_dt);

        if (m_isActive)
            m_alpha += getTemplate()->getFadeSpeed()*_dt;
        else
            m_alpha -= getTemplate()->getFadeSpeed()*_dt;

        m_alpha = f32_Clamp(m_alpha,0,1);


        if (m_alpha<=0)
            return; //We don't want any operation when transparent

        //////////////////////////////////////////////////////////////////////////
        m_stateChangeTimer_Electoon += _dt;
        m_stateChangeTimer_Tooth += _dt;

        updatePulse(m_stateChangeTimer_Electoon, m_electoonScale);
        updatePulse(m_stateChangeTimer_Tooth, m_toothScale);

        updateCount(_dt);

        i32 iconCount = (i32)m_movingIcons.size();
        for (i32 i=0; i<iconCount; i++)
        {
            //moving the icons
            MovingIcon &movingIcon = m_movingIcons[i];
            bbool endedMove = movingIcon.update(_dt);
            //flight sound
            if (movingIcon.m_justAppeared && m_soundComponent)
            {
                if (movingIcon.m_icon==icon_Tooth)
                    m_soundComponent->playSound(TOOTH_FLIGHT_SOUNDID);
                else
                    m_soundComponent->playSound(ELECTOON_FLIGHT_SOUNDID);

            }
            //end of movement
            if (endedMove)
            {
                if (movingIcon.m_icon==icon_Tooth)
                    m_nextToothCount++; 
                else
                    m_nextElectoonCount++; 

                m_movingIcons.eraseKeepOrder((u32)i);
                iconCount = (i32)m_movingIcons.size();
                i--;
            }
        }

        
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_ElectoonTeethComponent::addMissingIcons(const Vec3d &_startPos)
    {
        u32 electoonDest = RAY_GAMEMANAGER->getElectoonsCount();
        u32 toothDest = RAY_GAMEMANAGER->getTotalTakenToothCount();
        u32 lastElectoonCount = RAY_GAMEMANAGER->getWMLastElectoonCount();
        u32 lastToothCount = RAY_GAMEMANAGER->getWMLastToothCount();
        i32 deltaElectoons, deltaTeeth;

        if (lastElectoonCount==U32_INVALID)
            lastElectoonCount = electoonDest;
        if (lastToothCount==U32_INVALID)
            lastToothCount = toothDest;
        deltaElectoons = (i32)electoonDest-(i32)lastElectoonCount;
        deltaTeeth = (i32)toothDest-(i32)lastToothCount;


        f32 delay = 1;
        f32 delayStep = getTemplate()->getInterIconDelay();

        for (i32 i=0; i<deltaElectoons; i++)
        {
            addNewIcon(icon_Electoon, _startPos, -delay);
            delay += delayStep;
        }
        for (i32 i=0; i<deltaTeeth; i++)
        {
            addNewIcon(icon_Tooth, _startPos, -delay);
            delay += delayStep;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Ray_ElectoonTeethComponent::updatePulse(f32 _t, f32 &_scale)
    {
        f32 maxT = getTemplate()->getPulseDuration();

        if (_t>maxT)
        {
            _scale = 1;
        }
        else
        {
            f32 sine = f32_Sin(_t/maxT*MTH_PI);
            _scale = 1+sine*getTemplate()->getPulseAmplitude();
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bbool Ray_ElectoonTeethComponent::reachScoreValue(f32 _dt, f32 &_value, f32 _goal)
    {
        f32 step = getTemplate()->getIncrementSpeed()*_dt;
        if (_value<_goal)
        {
            if (_value<(_goal-1))
                _value = _goal-1;

            _value += step;
            if (_value>=_goal)
            {
                _value = _goal;
                return btrue;
            }
        }
        else
        {
            _value = _goal;
            return btrue;
        }
        return bfalse;
    }

    //////////////////////////////////////////////////////////////////////////
    void Ray_ElectoonTeethComponent::updateCount(f32 _dt)
    {
        f32 electoonDest = (f32)m_nextElectoonCount;
        f32 toothDest = (f32)m_nextToothCount;

        if (electoonDest!=m_lastElectoonCount)
        {
            if (reachScoreValue(_dt, m_lastElectoonCount, electoonDest))
            {
                m_stateChangeTimer_Electoon = 0;
                if (m_soundComponent)
                {
                    m_soundComponent->playSound(ELECTOON_INCREASE_SOUNDID);
                }
            }

            m_electoonTextTab[0].m_text.setTextFormat("%d", (int)m_lastElectoonCount);
        }
        
        if (toothDest!=m_lastToothCount)
        {
            if (reachScoreValue(_dt, m_lastToothCount, toothDest))
            {
                m_stateChangeTimer_Tooth = 0;
                if (m_soundComponent)
                {
                    m_soundComponent->playSound(TOOTH_INCREASE_SOUNDID);
                }
            }
            
            m_toothTextTab[0].m_text.setTextFormat("%d", (int)m_lastToothCount);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    IMPLEMENT_OBJECT_RTTI(Ray_ElectoonTeethComponent_Template)
    BEGIN_SERIALIZATION(Ray_ElectoonTeethComponent_Template)
    SERIALIZE_MEMBER("xFromRight", m_xFromRight);
    SERIALIZE_MEMBER("yFromTop", m_yFromTop);
    SERIALIZE_MEMBER("itemWidth", m_itemWidth);
    SERIALIZE_MEMBER("itemHeight", m_itemHeight);
    SERIALIZE_MEMBER("spacing", m_spacing);
    SERIALIZE_MEMBER("fontName", m_fontName);
    SERIALIZE_MEMBER("atlasPath", m_atlasPath);
    SERIALIZE_MEMBER("electoonScorePos", m_electoonScorePos);
    SERIALIZE_MEMBER("electoonScoreHeight", m_electoonScoreHeight);
    SERIALIZE_MEMBER("toothScorePos", m_toothScorePos);
    SERIALIZE_MEMBER("toothScoreHeight", m_toothScoreHeight);
    SERIALIZE_MEMBER("scoreColor", m_scoreColor);
    SERIALIZE_MEMBER("pulseDuration", m_pulseDuration);
    SERIALIZE_MEMBER("pulseAmplitude", m_pulseAmplitude);
    SERIALIZE_MEMBER("fadeSpeed", m_fadeSpeed);
    SERIALIZE_MEMBER("incrementSpeed", m_incrementSpeed);
    SERIALIZE_MEMBER("movementCurveDistance", m_movementCurveDistance);
    SERIALIZE_MEMBER("relativeNormalSize", m_relativeNormalSize);
    SERIALIZE_MEMBER("nearestRelativeZ", m_nearestRelativeZ);
    SERIALIZE_MEMBER("interIconDelay", m_interIconDelay);
    SERIALIZE_MEMBER("flightDuration", m_flightDuration);
    END_SERIALIZATION()



    //////////////////////////////////////////////////////////////////////////
    Ray_ElectoonTeethComponent_Template::Ray_ElectoonTeethComponent_Template()
    : m_xFromRight(0.01f)
    , m_yFromTop(0.01f)
    , m_itemWidth(0.1f)
    , m_itemHeight(0.1f)
    , m_spacing(0.01f)
    , m_electoonScorePos(0.15f,0.1f)
    , m_electoonScoreHeight(0.01f)
    , m_toothScorePos(0.05f,0.1f)
    , m_toothScoreHeight(0.1f)
    , m_scoreColor(Color::white())
    , m_pulseDuration(0.5f)
    , m_pulseAmplitude(1.f)
    , m_fadeSpeed(1)
    , m_incrementSpeed(4.f)
    , m_movementCurveDistance(0.1f)
    , m_relativeNormalSize(0.05f)
    , m_nearestRelativeZ(10.f)
    , m_interIconDelay(0.5f)
    , m_flightDuration(1.f)
    {
    }
};
