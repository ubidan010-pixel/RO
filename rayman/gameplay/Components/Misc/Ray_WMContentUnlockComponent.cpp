#include "precompiled_gameplay_rayman.h"

#ifndef _ITF_RAY_WMCONTENTUNLOCKCOMPONENT_H_
#include "rayman/gameplay/Components/Misc/Ray_WMContentUnlockComponent.h"
#endif //_ITF_RAY_WMCONTENTUNLOCKCOMPONENT_H_

#ifndef _ITF_INGAMETEXTCOMPONENT_H_
#include "gameplay/Components/UI/InGameTextComponent.h"
#endif //_ITF_INGAMETEXTCOMPONENT_H_

#ifndef _ITF_RAY_GAMEMANAGER_H_
#include "rayman/gameplay/Ray_GameManager.h"
#endif //_ITF_RAY_GAMEMANAGER_H_

#ifndef _ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_
#include "rayman/gameplay/Components/Player/Ray_PlayerCostumeManagerComponent.h"
#endif //_ITF_RAY_PLAYERCOSTUMEMANAGERCOMPONENT_H_

#ifndef _ITF_UICOMPONENT_H_
#include "gameplay/components/UI/UIComponent.h"
#endif //_ITF_UICOMPONENT_H_

#ifndef _ITF_LOCALISATIONMANAGER_H_
#include "engine/localisation/LocalisationManager.h"
#endif //_ITF_LOCALISATIONMANAGER_H_

#ifndef _ITF_UITEXTMANAGER_H_
#include "engine/actors/managers/UITextManager.h"
#endif //_ITF_UITEXTMANAGER_H_

#ifndef _ITF_ACTORSMANAGER_H_
#include "engine/actors/managers/actorsmanager.h"
#endif //_ITF_ACTORSMANAGER_H_

namespace ITF {

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WMContentUnlockComponent)
BEGIN_SERIALIZATION_CHILD(Ray_WMContentUnlockComponent)

    BEGIN_CONDITION_BLOCK(ESerializeGroup_DataEditable)
        SERIALIZE_MEMBER("RELATIVEPOS",m_relativePos);
    END_CONDITION_BLOCK()

END_SERIALIZATION()

BEGIN_VALIDATE_COMPONENT(Ray_WMContentUnlockComponent)

    VALIDATE_COMPONENT_PARAM("Ray_InGameTextComponent", m_inGameTextComponent, "Ray_InGameTextComponent mandatory");
    VALIDATE_COMPONENT_PARAM("AnimatedComponent", m_animatedComponent, "AnimatedComponent mandatory");

END_VALIDATE_COMPONENT()

Ray_WMContentUnlockComponent::Ray_WMContentUnlockComponent()
: m_inGameTextComponent(NULL)
, m_animatedComponent(NULL)
, m_iconRef(ITF_INVALID_OBJREF)
, m_relativePos(0.5f,0.5f)
, m_electoonBefore(0)
, m_electoonAfter(0)
, m_teethBefore(0)
, m_teethAfter(0)
, m_currentStage(STAGE_NONE)
, m_sentenceIndex(0)
, m_isFinished(bfalse)
, m_playNext(bfalse)
, m_hiding(bfalse)
{
}

Ray_WMContentUnlockComponent::~Ray_WMContentUnlockComponent()
{
}

void Ray_WMContentUnlockComponent::onActorClearComponents()
{
    Super::onActorClearComponents();

    Actor* icon = m_iconRef.getActor();

    if ( icon )
    {
        icon->requestDestruction();
    }

    EVENTMANAGER->unregisterEvent(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonToothScore,116023213), this);

    if ( RAY_GAMEMANAGER->getWMUnlockMessageActor() == m_actor->getRef() )
    {
        RAY_GAMEMANAGER->setWMUnlockMessageActor(ITF_INVALID_OBJREF);
    }

    GAMEMANAGER->removeInputListener(this);
}

void Ray_WMContentUnlockComponent::onActorLoaded(Pickable::HotReloadType _hotReload)
{
    Super::onActorLoaded(_hotReload);

    m_inGameTextComponent = m_actor->GetComponent<InGameTextComponent>();
    m_animatedComponent = m_actor->GetComponent<AnimatedComponent>();

    EVENTMANAGER_REGISTER_EVENT_LISTENER(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonToothScore,116023213),this);

    RAY_GAMEMANAGER->setWMUnlockMessageActor(m_actor->getRef());

    if ( !getTemplate()->getIconPath().isEmpty() &&
          getTemplate()->getIconPoint().isValid() )
    {
        Actor* iconActor = ACTORSMANAGER->spawnActor(m_actor->getPos(),m_actor->getScene(),getTemplate()->getIconPath(),btrue);

        if ( iconActor )
        {
            m_iconRef = iconActor->getRef();

            AnimLightComponent* animComponent = iconActor->GetComponent<AnimLightComponent>();

            if ( animComponent )
            {
                animComponent->setRank2D(UI_TEXTMANAGER->getAnimDepthRank()-1);
            }

            iconActor->disable();
        }
    }

    updatePosition();
}

void Ray_WMContentUnlockComponent::onBecomeActive()
{
    Super::onBecomeActive();

    m_currentStage = STAGE_NONE;
    m_sentenceIndex = 0;
    m_playNext = bfalse;

    Actor* icon = m_iconRef.getActor();

    if ( icon )
    {
        icon->enable();
    }

    prepareSentences();

    setNextStage();

    if ( !m_isFinished )
    {
        GAMEMANAGER->addGamePlayInputListener(this);
    }
}

void Ray_WMContentUnlockComponent::onBecomeInactive()
{
    Super::onBecomeInactive();

    m_currentStage = STAGE_NONE;
    m_sentenceIndex = 0;
    m_inGameTextComponent->setText(LocalisationId::Invalid);

    Actor* icon = m_iconRef.getActor();

    if ( icon )
    {
        icon->disable();
    }

    GAMEMANAGER->removeInputListener(this);
}

void Ray_WMContentUnlockComponent::prepareSentences()
{
    for ( u32 i = 0; i < MAX_STAGES; i++ )
    {
        m_stageSentences[i].clear();
    }

    m_isFinished = btrue;

    if ( m_electoonBefore == m_electoonAfter &&
         m_teethBefore == m_teethAfter )
    {
        return;
    }

    const Ray_GameManagerConfig_Template* gameManagerTemplate = RAY_GAMEMANAGER->getTemplate();

    if ( !gameManagerTemplate )
    {
        return;
    }

    const ITF_VECTOR<Ray_GameManagerConfig_Template::MapConfig>& mapList = gameManagerTemplate->getLevelsInfo();
    u32 numMaps = mapList.size();

    for ( u32 i = 0; i < numMaps; i++ )
    {
        const Ray_GameManagerConfig_Template::MapConfig& config = mapList[i];

        if ( config.m_minElectoons )
        {
            if ( m_electoonBefore < config.m_minElectoons && m_electoonAfter >= config.m_minElectoons )
            {
                if ( config.m_type.GetValue() == ITF_GET_STRINGID_CRC(chest,2256079071) )
                {
                    ESpot_State spotState = RAY_GAMEMANAGER->getWMSpotState(config.m_tag);

                    if ( spotState == SPOT_STATE_CLOSED )
                    {
                        continue;
                    }

                    StageSentence newSentence;

                    newSentence.m_sentenceId = getTemplate()->getChestLocId();

                    const Ray_GameManagerConfig_Template::MapConfig* worldConfig = RAY_GAMEMANAGER->getMapConfig(config.m_worldTag);

                    if ( worldConfig )
                    {
                        newSentence.m_varId = worldConfig->m_titleId;
                    }
                    else
                    {
                        newSentence.m_varId = LocalisationId::Invalid;
                    }

                    m_stageSentences[STAGE_CHEST].push_back(newSentence);
                }
                else if ( config.m_type.GetValue() == ITF_GET_STRINGID_CRC(bonus,875091126) )
                {
                    ESpot_State spotState = RAY_GAMEMANAGER->getWMSpotState(config.m_tag);

                    if ( spotState == SPOT_STATE_CLOSED )
                    {
                        continue;
                    }

                    StageSentence newSentence;

                    newSentence.m_sentenceId = getTemplate()->getFairyLocId();
                    
                    const Ray_GameManagerConfig_Template::MapConfig* worldConfig = RAY_GAMEMANAGER->getMapConfig(config.m_worldTag);

                    if ( worldConfig )
                    {
                        newSentence.m_varId = worldConfig->m_titleId;
                    }
                    else
                    {
                        newSentence.m_varId = LocalisationId::Invalid;
                    }

                    m_stageSentences[STAGE_FAIRY].push_back(newSentence);
                }
                else if ( config.m_type.GetValue() == ITF_GET_STRINGID_CRC(boss,2488061616) )
                {
                    ESpot_State spotState = RAY_GAMEMANAGER->getWMSpotState(config.m_tag);

                    if ( spotState == SPOT_STATE_CLOSED )
                    {
                        continue;
                    }

                    StageSentence newSentence;

                    // Each boss has a specific unlock message to display
                    if( config.m_worldTag == s_JU_B )
                        newSentence.m_sentenceId = getTemplate()->getBossJungleLocId();
                    else if ( config.m_worldTag == s_MU_B )
                        newSentence.m_sentenceId = getTemplate()->getBossMusicLocId();
                    else if ( config.m_worldTag == s_FO_B )
                        newSentence.m_sentenceId = getTemplate()->getBossFoodLocId();
                    else if ( config.m_worldTag == s_OC_B )
                        newSentence.m_sentenceId = getTemplate()->getBossOceanLocId();

                    const Ray_GameManagerConfig_Template::MapConfig* worldConfig = RAY_GAMEMANAGER->getMapConfig(config.m_worldTag);

                    if ( worldConfig )
                    {
                        newSentence.m_varId = worldConfig->m_titleId;
                    }
                    else
                    {
                        newSentence.m_varId = LocalisationId::Invalid;
                    }

                    m_stageSentences[STAGE_BOSS].push_back(newSentence);
                }
            }
        }
        else if ( config.m_minTeeth && m_stageSentences[STAGE_DEADLAND].size() == 0 )
        {
            if ( m_teethBefore < config.m_minTeeth && m_teethAfter >= config.m_minTeeth )
            {
                StageSentence newSentence;

                newSentence.m_sentenceId = getTemplate()->getDeadLandLocId();
                newSentence.m_varId = LocalisationId::Invalid;

                m_stageSentences[STAGE_COSTUME].push_back(newSentence);
            }
        }
    }

    ObjectRef costumeManagerRef = RAY_GAMEMANAGER->getCostumeManager();
    Actor* costumeMgr = AIUtils::getActor(costumeManagerRef);

    if ( costumeMgr )
    {
        Ray_PlayerCostumeManagerComponent* manager = costumeMgr->GetComponent<Ray_PlayerCostumeManagerComponent>();

        if ( manager )
        {
            const SafeArray <CostumeInfo>& costumesInfo = manager->getCostumesInfo();
            u32 numCostumes = costumesInfo.size();

            for ( u32 i = 0; i < numCostumes; i++ )
            {
                const CostumeInfo& info = costumesInfo[i];

                if ( m_electoonBefore < info.m_prize && m_electoonAfter >= info.m_prize )
                {
                    StageSentence newSentence;

                    newSentence.m_sentenceId = getTemplate()->getCostumeLocId();
                    newSentence.m_varId = LocalisationId::Invalid;

                    m_stageSentences[STAGE_COSTUME].push_back(newSentence);
                    break;
                }
            }
        }
    }
}

void Ray_WMContentUnlockComponent::setNextStage()
{
    m_isFinished = bfalse;

    bbool endSearch = bfalse;

    while ( !endSearch )
    {
        if ( m_currentStage == STAGE_NONE )
        {
            m_currentStage = 0;
            m_sentenceIndex = 0;

            if ( m_sentenceIndex < m_stageSentences[m_currentStage].size() )
            {
                endSearch = btrue;
            }
        }
        else
        {
            m_sentenceIndex++;

            if ( m_sentenceIndex >= m_stageSentences[m_currentStage].size() )
            {
                m_sentenceIndex = 0;
                m_currentStage++;

                if ( m_currentStage < MAX_STAGES && m_sentenceIndex < m_stageSentences[m_currentStage].size() )
                {
                    endSearch = btrue;
                }
            }
            else
            {
                endSearch = btrue;
            }
        }

        if ( m_currentStage >= MAX_STAGES )
        {
            m_currentStage = STAGE_NONE;
            m_sentenceIndex = 0;
            endSearch = btrue;
            m_isFinished = btrue;
        }
    }

    if ( !m_isFinished )
    {
        const StageSentence& stageSentence = m_stageSentences[m_currentStage][m_sentenceIndex];
        if ( stageSentence.m_varId != LocalisationId::Invalid )
        {
            UI_TEXTMANAGER->addVariable(ITF_GET_STRINGID_CRC(unlockWorld,2971423938),LOCALISATIONMANAGER->getText(stageSentence.m_varId));
        }
        m_inGameTextComponent->setText(stageSentence.m_sentenceId);
        m_inGameTextComponent->playAppear();
    }
}

void Ray_WMContentUnlockComponent::Update( f32 _dt )
{
    Super::Update(_dt);

    if ( m_playNext )
    {
        m_inGameTextComponent->playDisappear();
        m_playNext = bfalse;
        m_hiding = btrue;
    }
    else if ( m_hiding )
    {
        if ( m_animatedComponent->isMainNodeFinished() )
        {
            setNextStage();
            m_hiding = bfalse;
        }
    }

    updatePosition();
    updateIcon();
}

void Ray_WMContentUnlockComponent::updatePosition()
{
    Vec2d pos;
    f32 screenWidth = (f32)GFX_ADAPTER->getScreenWidth();
    f32 screenHeight = (f32)GFX_ADAPTER->getScreenHeight();

    pos.m_x = screenWidth * m_relativePos.m_x;
    pos.m_y = screenHeight * m_relativePos.m_y;

    m_actor->set2DPos(pos);

    Vec2d newScale = getTemplate()->getAnimSize();
    Vec2d screenRatio = UIComponent::getScreenRatio();
    newScale *= screenRatio;

    m_actor->setScale(newScale);
}

void Ray_WMContentUnlockComponent::updateIcon()
{
    Actor* icon = m_iconRef.getActor();

    if ( icon )
    {
        Vec2d screenRatio = UIComponent::getScreenRatio();
        Vec2d newScale = getTemplate()->getIconAnimSize() * screenRatio;
        icon->setScale(newScale);

        AnimPolyline* pPolyline = NULL;
        u32 pointIndex;
        const Vec2d* pointsBuffer = m_animatedComponent->getCurrentFirstPolylinePoint(getTemplate()->getIconPoint(), &pPolyline, &pointIndex);
        Vec3d pos;

        if (pointsBuffer)
        {
            Vec2d p = pointsBuffer[pointIndex];

            p.m_y *= -1.f;
            p = (p*m_actor->getScale()).Rotate(m_actor->getAngle()) + m_actor->get2DPos();

            pos = p.to3d(m_actor->getPos().m_z);
        }
        else
        {
            pos = m_actor->getPos();
        }

        icon->setPos(pos);
    }
}

void Ray_WMContentUnlockComponent::onEvent( Event* _event )
{
    if (Ray_EventShowElectoonToothScore *showScore = _event->DynamicCast<Ray_EventShowElectoonToothScore>(ITF_GET_STRINGID_CRC(Ray_EventShowElectoonToothScore,116023213)))
    {
        m_electoonAfter = RAY_GAMEMANAGER->getElectoonsCount();
        m_teethAfter = RAY_GAMEMANAGER->getTotalTakenToothCount();
        m_electoonBefore = RAY_GAMEMANAGER->getWMLastElectoonCount();
        m_teethBefore = RAY_GAMEMANAGER->getWMLastToothCount();
    }
}

void Ray_WMContentUnlockComponent::Receive( u32 _deviceID, f32 _axis, const StringID& _action )
{
    if ( GAMEMANAGER->getMainIndexPlayer() != _deviceID ||
         GAMEMANAGER->isInPause() ||
        !m_animatedComponent->isMainNodeFinished() ||
         m_hiding )
    {
        return; 
    }

    if ( _action == ITF_GET_STRINGID_CRC(WM_ENTER,2267322818) )
    {
        m_playNext = btrue;
    }
}

#ifdef ITF_SUPPORT_EDITOR
void Ray_WMContentUnlockComponent::onEditorCreated( class Actor* _original )
{
    Super::onEditorCreated(_original);

    // Calculate the relative position
    m_relativePos.m_x = m_actor->getPos().m_x / (f32)GFX_ADAPTER->getScreenWidth();
    m_relativePos.m_y = m_actor->getPos().m_y / (f32)GFX_ADAPTER->getScreenHeight();
}

void Ray_WMContentUnlockComponent::onEditorMove(bbool _modifyInitialPos)
{
    Super::onEditorMove(_modifyInitialPos);

    // Calculate the relative position
    m_relativePos.m_x = m_actor->getPos().m_x / (f32)GFX_ADAPTER->getScreenWidth();
    m_relativePos.m_y = m_actor->getPos().m_y / (f32)GFX_ADAPTER->getScreenHeight();
}

#endif // ITF_SUPPORT_EDITOR

//------------------------------------------------------------------------------
IMPLEMENT_OBJECT_RTTI(Ray_WMContentUnlockComponent_Template)
BEGIN_SERIALIZATION(Ray_WMContentUnlockComponent_Template)

    SERIALIZE_MEMBER("fairyLocId",m_fairyLocId);
    SERIALIZE_MEMBER("chestLocId",m_chestLocId);
    SERIALIZE_MEMBER("costumeLocId",m_costumeLocId);
    SERIALIZE_MEMBER("deadLandLocId",m_deadLandLocId);
    SERIALIZE_MEMBER("bossJungleLocId",m_bossJungleLocId);
    SERIALIZE_MEMBER("bossMusicLocId",m_bossMusicLocId);
    SERIALIZE_MEMBER("bossFoodLocId",m_bossFoodLocId);
    SERIALIZE_MEMBER("bossOceanLocId",m_bossOceanLocId);
    SERIALIZE_MEMBER("animSize",m_animSize);
    SERIALIZE_MEMBER("iconPath",m_iconPath);
    SERIALIZE_MEMBER("iconPoint",m_iconPoint);
    SERIALIZE_MEMBER("iconAnimSize",m_iconAnimSize);

END_SERIALIZATION()

//------------------------------------------------------------------------------
Ray_WMContentUnlockComponent_Template::Ray_WMContentUnlockComponent_Template()
: m_animSize(300.f,300.f)
, m_iconAnimSize(32.f,32.f)
{
}

//------------------------------------------------------------------------------
Ray_WMContentUnlockComponent_Template::~Ray_WMContentUnlockComponent_Template()
{
}

} // namespace ITF
